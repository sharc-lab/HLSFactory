import os
import re
import argparse
import json
from clang.cindex import Index, CursorKind

import os
from clang.cindex import Config
lf = os.getenv("LIBCLANG_FILE")
lp = os.getenv("LIBCLANG_PATH")
if lf and os.path.isfile(lf):
    Config.set_library_file(lf)
elif lp and os.path.isdir(lp):
    Config.set_library_path(lp)


# Import TeeLogger - handle import gracefully
try:
    from ..design_space.logger import TeeLogger
except ImportError:
    TeeLogger = None

class InputCAnalyzer:
    def __init__(self, filename):
        self.filename = filename
        self.input_c_hierarchy = None
        # Logger will be set externally
        self.logger = None
    
    def set_logger(self, logger):
        """Set the logger for this analyzer."""
        self.logger = logger

    def generate_input_c_hierarchy(self):
        clang_args = [
            '-std=c++17',
            '-x', 'c++',
            '-fsyntax-only',  # Only check syntax
            '-I.',            # Include dummy headers in current dir
            '-I/tools/Xilinx/Vitis/2024.2/include',  # Optional: define if your HLS code uses guards
            '-Wno-unknown-pragmas',
            '-Wno-pragma-once-outside-header',
        ]

        index = Index.create()
        translation_unit = index.parse(self.filename, args=clang_args)

        # Print diagnostics
        if translation_unit.diagnostics:
            if self.logger:
                self.logger.print("Diagnostics:")
                for diag in translation_unit.diagnostics:
                    self.logger.print(f"  [{diag.severity}] {diag.spelling} (Line {diag.location.line}, Col {diag.location.column})")
            else:
                print("Diagnostics:")
                for diag in translation_unit.diagnostics:
                    print(f"  [{diag.severity}] {diag.spelling} (Line {diag.location.line}, Col {diag.location.column})")

        functions = []

        def print_ast(node, indent=0):
            if self.logger:
                self.logger.print('  ' * indent + f"{node.kind} '{node.spelling}' <{node.location}>")
            else:
                print('  ' * indent + f"{node.kind} '{node.spelling}' <{node.location}>")
            for child in node.get_children():
                print_ast(child, indent + 1)
        
        def extract_children(node):
            if self.logger:
                self.logger.print(f"Visiting node: '{node.spelling}'")
            else:
                print(f"Visiting node: '{node.spelling}'")
            # if (node.kind.name == "FUNCTION_TEMPLATE"):
            #     print_ast(node)
            children = []
            # Find the COMPOUND_STMT (function body)
            compound_stmt = None
            for child in node.get_children():
                if child.kind == CursorKind.COMPOUND_STMT:
                    compound_stmt = child
                    break

            def get_callee_name_from_call_expr(call_expr):
                for c in call_expr.get_children():
                    # Usually wrapped inside a DECL_REF_EXPR
                    if c.kind == CursorKind.DECL_REF_EXPR:
                        for sub in c.get_children():
                            if sub.kind == CursorKind.OVERLOADED_DECL_REF:
                                return sub.spelling
                    elif c.kind == CursorKind.OVERLOADED_DECL_REF:
                        return c.spelling
                return None

            def extract_function (stmt, append_list):
                # # Filter out calls from other files
                # if not stmt.location.file or os.path.abspath(stmt.location.file.name) != os.path.abspath(self.filename):
                #     return
                # skip if no reference or unknown file
                # if not stmt.referenced or not stmt.referenced.location.file:
                #     return  
                # # Compare the source file of the definition with your file
                # # skip if it's defined elsewhere (e.g., math library)
                if stmt.referenced and stmt.referenced.location.file:
                    call_file = os.path.abspath(stmt.referenced.location.file.name)
                    src_file = os.path.abspath(self.filename)
                    if self.logger:
                        self.logger.print("Remove call", call_file, src_file, stmt.spelling )
                    else:
                        print("Remove call", call_file, src_file, stmt.spelling )
                    if call_file != src_file:
                        return

                callee = stmt.spelling or get_callee_name_from_call_expr(stmt)
                #  filters only actual C++ operator names like: operator+, operator-, operator*, but allow function names like operatorCustomCall
                if callee and not re.match(r'^operator(\W|$)', callee):
                    print(f"  -> Found function call: {callee}")
                    append_list.append({'type': 'call', 'name': callee, 'line': stmt.location.line, 'file': stmt.location.file.name if stmt.location.file else None})

            def extract_func_and_loop (stmt):
                # Look for top-level labeled loops
                if stmt.kind == CursorKind.LABEL_STMT:
                    label = stmt.spelling
                    target = next(stmt.get_children(), None)

                    if target and target.kind == CursorKind.FOR_STMT:
                        print(f"  -> Found top-level loop: {label}")

                        # This is the current loop entry
                        loop_entry = {
                            'type': 'loop',
                            'label': label,
                            'line': stmt.location.line,
                            'start_line': stmt.extent.start.line,
                            'end_line': stmt.extent.end.line,
                            'file': stmt.location.file.name if stmt.location.file else None,
                            'children': []
                        }

                        def collect_subloops(subnode, indent=2):
                            subloops = []

                            for c in subnode.get_children():
                                if c.kind == CursorKind.LABEL_STMT:
                                    sublabel = c.spelling
                                    subtarget = next(c.get_children(), None)
                                    if subtarget and subtarget.kind == CursorKind.FOR_STMT:
                                        print("  " * indent + f"-> Subloop: {sublabel}")
                                        subloop = {
                                            'type': 'loop',
                                            'label': sublabel,
                                            'line': c.location.line,
                                            'start_line': c.extent.start.line,
                                            'end_line': c.extent.end.line,
                                            'file': c.location.file.name if c.location.file else None,
                                            'children': collect_subloops(subtarget, indent + 1)
                                        }
                                        subloops.append(subloop)
                                elif c.kind == CursorKind.CALL_EXPR:
                                    extract_function (c, subloops)
                                else:
                                    subloops.extend(collect_subloops(c, indent))
                            return subloops
                        loop_entry['children'] = collect_subloops(target)
                        children.append(loop_entry)
                    
                # Direct function call
                elif stmt.kind == CursorKind.CALL_EXPR:
                    extract_function (stmt, children)
                else:
                    # handle situation when loops at the top level
                    for child in stmt.get_children():
                        extract_func_and_loop(child)

            if compound_stmt:
                for stmt in compound_stmt.get_children():
                    # print(f"-     Visiting: {stmt.kind.name} (spelling: '{stmt.spelling}')")
                    extract_func_and_loop (stmt)

            return children


        def visitor(node):
            # Skip declarations from included headers
            if node.location.file and os.path.abspath(node.location.file.name) != os.path.abspath(self.filename):
                return  

            if node.kind in [CursorKind.FUNCTION_DECL, CursorKind.CXX_METHOD, CursorKind.FUNCTION_TEMPLATE]:
                functions.append({
                    'name': node.spelling,
                    'file': node.location.file.name if node.location.file else None,
                    'start_line': node.extent.start.line,
                    'end_line': node.extent.end.line,
                    'kind': node.kind.name,
                    'pragmas': [],
                    'pragma_loc': [],
                    'children': extract_children(node)
                })

            for child in node.get_children():
                visitor(child)

        visitor(translation_unit.cursor)

        with open(self.filename, 'r') as f:
            lines = f.readlines()
        # Build line -> pragma mapping
        pragma_lines = {}
        for i, line in enumerate(lines):
            if line.strip().startswith("#pragma"):
                pragma_lines[i] = line.strip()        
        # Assign pragmas to functions by line range
        for func in functions:
            for line_no in range(func['start_line'], func['end_line'] + 1):
                if line_no in pragma_lines:
                    func['pragmas'].append(pragma_lines[line_no])  
                    func['pragma_loc'].append( (self.filename, line_no+1) )  # +1 for 1-based line numbers
        
        self.input_c_hierarchy = functions
        return functions

    def pretty_print_input_c_hierarchy(self):
        def print_children(children, indent=2):
            for child in children:
                prefix = " " * indent
                if child['type'] == 'loop':
                    print(f"{prefix}- Loop: {child['label']} (file {os.path.basename(child['file'])}:{child['line']})")
                    print_children(child['children'], indent + 2)
                elif child['type'] == 'call':
                    print(f"{prefix}- Call to: {child['name']} (file {os.path.basename(child['file'])}:{child['line']})")

        print("\n=== Function Hierarchy ===")
        for func in self.input_c_hierarchy:
            print(f"Function '{func['name']}' ({func['kind']}) [file {os.path.basename(func['file'])}:{func['start_line']}–{func['end_line']}]")
            if func['pragma_loc']:
                print("  Pragmas:")
                for file, line in func['pragma_loc']:
                    print(f"    - {os.path.basename(file)}:{line}")
            print_children(func['children'], indent=4)
        print("=== End of Hierarchy ===\n")

    def save_pretty_print_hierarchy_to_txt(self, output_path=None):
        """
        Saves the pretty-printed hierarchy under a text file in 'logs' next to the source file.
        """
        if self.input_c_hierarchy is None:
            raise ValueError("Hierarchy not generated. Call generate_input_c_hierarchy() first.")

        src_dir = os.path.dirname(os.path.abspath(self.filename))
        output_dir = os.path.join(src_dir, 'logs')
        os.makedirs(output_dir, exist_ok=True)

        out_path = os.path.join(output_dir, output_path or "input_c_hierarchy.txt")
        with open(out_path, 'w') as f:
            f.write("=== Function Hierarchy ===\n\n")
            def write_children(children, indent):
                for child in children:
                    prefix = " " * indent
                    if child['type'] == 'loop':
                        f.write(f"{prefix}- Loop: {child['label']} (file {os.path.basename(child['file'])}:{child['line']})\n")
                        write_children(child['children'], indent + 2)
                    elif child['type'] == 'call':
                        f.write(f"{prefix}- Call to: {child['name']} (file {os.path.basename(child['file'])}:{child['line']})\n")

            for func in self.input_c_hierarchy:
                f.write(f"Function '{func['name']}' ({func['kind']}) [file {os.path.basename(func['file'])}:{func['start_line']}–{func['end_line']}]\n")
                if func['pragma_loc']:
                    f.write(f"  Pragmas:")
                    for file, line in func['pragma_loc']:
                        f.write(f"    - {os.path.basename(file)}:{line}\n")
                write_children(func['children'], indent=4)
                f.write("\n")
            f.write("=== End of Hierarchy ===\n")
        print(f"Pretty hierarchy saved to {out_path}")

    def save_hierarchy_to_json(self, output_path=None):
        """
        Saves the cached hierarchy under a subfolder 'output' next to the source file.
        """
        if self.input_c_hierarchy is None:
            raise ValueError("Hierarchy not generated. Call generate_input_c_hierarchy() first.")

        if output_path is None:
            src_dir = os.path.dirname(os.path.abspath(self.filename))
            output_dir = os.path.join(src_dir, 'logs')
        os.makedirs(output_dir, exist_ok=True)        
        output_path = os.path.join(output_dir, "input_c_hierarchy.json")
        
        with open(output_path, 'w') as f:
            json.dump(self.input_c_hierarchy, f, indent=2)
        print(f"Hierarchy saved to {output_path}")




