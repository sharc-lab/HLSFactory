#!/usr/bin/env python3
"""
Analyze mappings between C input hierarchy and RTL modules through LLVM IR and transformations.
"""

import json
import re
from pathlib import Path
from typing import Dict, List, Optional, Tuple

# Import TeeLogger - handle import gracefully
try:
    from ..design_space.logger import TeeLogger
except ImportError:
    TeeLogger = None

class CToRTLMapper:
    def __init__(self, log_dir: str):
        self.log_dir = Path(log_dir)
        self.input_c_hierarchy = None
        self.llvm_ir_hierarchy = None
        self.transform_messages = None
        self.csynth_module_info = None
        
        # Mapping structures
        self.c_loops_by_line = {}  # line -> loop info
        self.c_functions_by_name = {}  # function name -> function info
        self.llvm_modules = {}  # module name -> module info
        self.final_mappings = {}  # final module name -> original C location/loop
        
        # Logger will be set externally
        self.logger = None
    
    def set_logger(self, logger):
        """Set the logger for this mapper."""
        self.logger = logger
        
    def load_json_files(self):
        """Load all 4 JSON files"""
        try:
            with open(self.log_dir / "input_c_hierarchy.json") as f:
                self.input_c_hierarchy = json.load(f)
            
            with open(self.log_dir / "llvm_ir_hierarchy.json") as f:
                self.llvm_ir_hierarchy = json.load(f)
                
            with open(self.log_dir / "transform_messages.json") as f:
                self.transform_messages = json.load(f)
                
            with open(self.log_dir / "csynth_module_info.json") as f:
                self.csynth_module_info = json.load(f)
                
            if self.logger:
                self.logger.print("✓ All JSON files loaded successfully")
            else:
                print("✓ All JSON files loaded successfully")
            return True
        except Exception as e:
            if self.logger:
                self.logger.print(f"✗ Error loading JSON files: {e}")
            else:
                print(f"✗ Error loading JSON files: {e}")
            return False
    
    def build_c_hierarchy_index(self):
        """Build index of C hierarchy for quick lookup"""
        def index_element(element, parent_func=None):
            if element.get("kind") in ["FUNCTION_DECL", "FUNCTION_TEMPLATE"]:
                func_name = element["name"]
                self.c_functions_by_name[func_name] = element
                parent_func = func_name
            
            # Index loops by line number
            if "children" in element:
                for child in element["children"]:
                    if child.get("type") == "loop":
                        line = child["line"]
                        self.c_loops_by_line[line] = {
                            "label": child["label"],
                            "line": line,
                            "start_line": child.get("start_line", line),
                            "end_line": child.get("end_line", line),
                            "function": parent_func,
                            "file": child["file"]
                        }
                    index_element(child, parent_func)
        
        for func in self.input_c_hierarchy:
            index_element(func)
        
        if self.logger:
            self.logger.print(f"✓ Indexed {len(self.c_functions_by_name)} functions and {len(self.c_loops_by_line)} loops")
        else:
            print(f"✓ Indexed {len(self.c_functions_by_name)} functions and {len(self.c_loops_by_line)} loops")
    
    def build_llvm_hierarchy_index(self):
        """Build index of LLVM IR modules"""
        def index_llvm_modules(node):
            if "name" in node:
                self.llvm_modules[node["name"]] = node
            
            if "callees" in node:
                for callee in node["callees"]:
                    index_llvm_modules(callee)
        
        index_llvm_modules(self.llvm_ir_hierarchy)
        if self.logger:
            self.logger.print(f"✓ Indexed {len(self.llvm_modules)} LLVM IR modules")
        else:
            print(f"✓ Indexed {len(self.llvm_modules)} LLVM IR modules")
    
    def map_simple_cases(self):
        """Handle simple loop name mappings"""
        mapped = 0
        for module_name, module_info in self.llvm_modules.items():
            if "loops" in module_info and module_info["loops"]:
                # Only map to loop if this appears to be a loop-specific module
                # Skip if this looks like a function-level module that contains loops
                if self._should_map_as_loop(module_name, module_info):
                    loop_label = module_info["loops"][0]
                    
                    # Find the original C loop
                    c_loop = None
                    for line, loop_info in self.c_loops_by_line.items():
                        if loop_info["label"] == loop_label:
                            c_loop = loop_info
                            break
                    
                    if c_loop:
                        self.final_mappings[module_name] = {
                            "type": "simple_loop",
                            "c_loop": loop_label,
                            "c_line": c_loop["line"],
                            "start_line": c_loop["start_line"],
                            "end_line": c_loop["end_line"],
                            "c_function": c_loop["function"],
                            "c_file": c_loop["file"],
                            "llvm_module": module_name
                        }
                        mapped += 1
        
        if self.logger:
            self.logger.print(f"✓ Mapped {mapped} simple loop cases")
        else:
            print(f"✓ Mapped {mapped} simple loop cases")
    
    def _should_map_as_loop(self, module_name, module_info):
        """Determine if a module should be mapped as a simple loop or as a function"""
        # If the module has callees (sub-functions), it's likely a function-level module
        if "callees" in module_info and module_info["callees"]:
            return False
        
        # If the module name indicates it's loop-specific (contains "Loop" and "proc")
        if "Loop" in module_name and "proc" in module_name:
            return True
        
        # If the module name is a known C function, don't map as loop
        if module_name in self.c_functions_by_name:
            return False
        
        # Default to True for modules with loops but no clear function indication
        return True


    def map_function_level_cases(self):
        """Handle function-level modules"""
        mapped = 0
        for module_name, module_info in self.llvm_modules.items():
            if module_name in self.final_mappings:
                continue
            
            # Check if module name matches a C function
            if module_name in self.c_functions_by_name:
                func_info = self.c_functions_by_name[module_name]
                self.final_mappings[module_name] = {
                    "type": "function",
                    "c_function": module_name,
                    "c_line": func_info["start_line"],
                    "start_line": func_info["start_line"],
                    "end_line": func_info["end_line"],
                    "c_file": func_info["file"],
                    "llvm_module": module_name
                }
                mapped += 1
        
        if self.logger:
            self.logger.print(f"✓ Mapped {mapped} function-level cases")
        else:
            print(f"✓ Mapped {mapped} function-level cases")
    
    def map_flattened_cases(self):
        """Handle flattened loop combinations using transform messages"""
        mapped = 0
        
        # Build set of flattened loop names from transform messages
        flattened_loop_names = set()
        if "flattening" in self.transform_messages:
            for flat in self.transform_messages["flattening"]:
                loop_name = flat["loop"]
                flattened_loop_names.add(loop_name)
        
        # Look for RTL modules that combine flattened loops
        for module_name, module_info in self.llvm_modules.items():
            if module_name in self.final_mappings:
                continue
            
            if "loops" in module_info and module_info["loops"]:
                loop_pattern = module_info["loops"][0]
                
                # Check if this is a combined loop pattern (contains underscores)
                if "_" in loop_pattern:
                    loop_parts = loop_pattern.split("_")
                    if len(loop_parts) >= 2:
                        # Check if ANY of the loop parts were marked as flattened in transform messages
                        flattened_parts = []
                        for part in loop_parts:
                            if part in flattened_loop_names:
                                flattened_parts.append(part)
                        
                        # If we found flattened loops, find their C source info
                        if flattened_parts:
                            original_loops = []
                            for part in loop_parts:  # Use all parts, not just flattened ones
                                for line, loop_info in self.c_loops_by_line.items():
                                    if loop_info["label"] == part:
                                        original_loops.append(loop_info)
                                        break
                            
                            if original_loops:
                                self.final_mappings[module_name] = {
                                    "type": "flattened",
                                    "c_loops": [loop["label"] for loop in original_loops],
                                    "c_lines": [loop["line"] for loop in original_loops],
                                    # "start_lines": [loop["start_line"] for loop in original_loops],
                                    # "end_lines": [loop["end_line"] for loop in original_loops],
                                    "start_lines": min(loop["start_line"] for loop in original_loops),
                                    "end_lines":   max(loop["end_line"]   for loop in original_loops),
                                    "c_function": original_loops[0]["function"],
                                    "c_file": original_loops[0]["file"],
                                    "llvm_module": module_name,
                                    "combined_pattern": loop_pattern,
                                    "flattened_loops": flattened_parts
                                }
                                mapped += 1
        
        if self.logger:
            self.logger.print(f"✓ Mapped {mapped} flattened cases")
        else:
            print(f"✓ Mapped {mapped} flattened cases")
    
    def map_unmapped_to_parent(self):
        """Map remaining unmapped csynth modules to their parent functions"""
        mapped = 0
        
        # Get all csynth modules
        csynth_modules = set()
        if "metrics" in self.csynth_module_info:
            csynth_modules = set(self.csynth_module_info["metrics"].keys())
        
        # Find unmapped modules
        mapped_modules = set(self.final_mappings.keys())
        unmapped_modules = csynth_modules - mapped_modules
        
        # Build reverse legalization mapping from transform_messages.json
        llvm_to_csynth_map = {}
        if "legalizing" in self.transform_messages:
            for entry in self.transform_messages["legalizing"]:
                llvm_to_csynth_map[entry["original"]] = entry["legalized"]
        
        def find_immediate_parent_in_hierarchy(target_module, node, path=[]):
            """Find the immediate parent of target_module in the LLVM IR hierarchy"""
            current_path = path + [node["name"]]
            
            # Check direct children
            for callee in node.get("callees", []):
                callee_name = callee["name"]
                callee_legalized = llvm_to_csynth_map.get(callee_name, callee_name)
                
                # If we found the target as a direct child, return this node as the parent
                if callee_name == target_module or callee_legalized == target_module:
                    # Found target as direct child - this node is the immediate parent
                    return _create_parent_mapping(node["name"], current_path)
                
                # Recursively search deeper
                result = find_immediate_parent_in_hierarchy(target_module, callee, current_path)
                if result:
                    return result
            
            return None
        
        def find_any_parent_in_hierarchy(target_module, node, path=[]):
            """Fallback: find any parent of target_module in the LLVM IR hierarchy"""
            current_path = path + [node["name"]]
            
            # Check if this node matches our target exactly
            node_name = node["name"]
            if node_name == target_module:
                # Found exact match - return the last known C function in the path
                return _create_parent_mapping_from_path(current_path)
            
            # Check if this LLVM node legalizes to our target csynth module
            if node_name in llvm_to_csynth_map and llvm_to_csynth_map[node_name] == target_module:
                # Found legalized match - return the last known C function in the path
                return _create_parent_mapping_from_path(current_path)
            
            # Recursively search children
            for callee in node.get("callees", []):
                result = find_any_parent_in_hierarchy(target_module, callee, current_path)
                if result:
                    return result
            
            return None
        
        def _create_parent_mapping(node_name, path):
            """Create parent mapping from a node name and path"""
            # Check if this node is already mapped (prefer existing mapping)
            node_legalized = llvm_to_csynth_map.get(node_name, node_name)
            if node_legalized in self.final_mappings:
                existing_mapping = self.final_mappings[node_legalized]
                return {
                    "type": "function",
                    "c_function": existing_mapping["c_function"],
                    "c_line": existing_mapping["c_line"],
                    "start_line": existing_mapping["start_line"],
                    "end_line": existing_mapping["end_line"],
                    "c_file": existing_mapping["c_file"],
                    "llvm_module": node_legalized
                }
            
            # Try to find a C function in the path (working backwards for most immediate)
            for node_in_path in reversed(path):
                if node_in_path in self.c_functions_by_name:
                    func_info = self.c_functions_by_name[node_in_path]
                    return {
                        "type": "function",
                        "c_function": node_in_path,
                        "c_line": func_info["start_line"],
                        "start_line": func_info["start_line"],
                        "end_line": func_info["end_line"],
                        "c_file": func_info["file"],
                        "llvm_module": node_in_path
                    }
            
            return None
        
        def _create_parent_mapping_from_path(path):
            """Create parent mapping from a path (excluding the target itself)"""
            # Look for the last C function in the path (excluding the target)
            for node_name in reversed(path[:-1]):  # Exclude last element (target)
                if node_name in self.c_functions_by_name:
                    func_info = self.c_functions_by_name[node_name]
                    return {
                        "type": "function",
                        "c_function": node_name,
                        "c_line": func_info["start_line"],
                        "start_line": func_info["start_line"],
                        "end_line": func_info["end_line"],
                        "c_file": func_info["file"],
                        "llvm_module": node_name
                    }
            
            return None
        
        for module_name in unmapped_modules:
            # First try to find immediate parent
            parent_mapping = find_immediate_parent_in_hierarchy(module_name, self.llvm_ir_hierarchy)
            
            # If no immediate parent found, try any parent
            if not parent_mapping:
                parent_mapping = find_any_parent_in_hierarchy(module_name, self.llvm_ir_hierarchy)
            
            if parent_mapping:
                self.final_mappings[module_name] = {
                    "type": "unmapped_merged_parent",
                    "c_function": parent_mapping.get("c_function"),
                    "c_line": parent_mapping.get("c_line"),
                    "start_line": parent_mapping.get("start_line"),
                    "end_line": parent_mapping.get("end_line"),
                    "c_file": parent_mapping.get("c_file"),
                    "llvm_module": parent_mapping.get("llvm_module"),
                    "parent_mapping_type": parent_mapping.get("type", "unknown")
                }
                mapped += 1
                if self.logger:
                    self.logger.print(f"  ✓ {module_name} -> {parent_mapping.get('c_function')}")
                else:
                    print(f"  ✓ {module_name} -> {parent_mapping.get('c_function')}")
            else:
                if self.logger:
                    self.logger.print(f"  ✗ No parent found for {module_name}")
                else:
                    print(f"  ✗ No parent found for {module_name}")
        
        if self.logger:
            self.logger.print(f"✓ Mapped {mapped} unmapped modules to parent")
        else:
            print(f"✓ Mapped {mapped} unmapped modules to parent")
    
    def apply_legalization(self):
        """Apply name legalization transformations"""
        legalized = 0
        if "legalizing" in self.transform_messages:
            name_map = {}
            reverse_name_map = {}
            for legal in self.transform_messages["legalizing"]:
                name_map[legal["original"]] = legal["legalized"]
                reverse_name_map[legal["legalized"]] = legal["original"]
            
            # Handle LLVM IR modules that need legalization
            new_mappings = {}
            for module_name, mapping in self.final_mappings.items():
                if module_name in name_map:
                    legalized_name = name_map[module_name]
                    mapping["legalized_name"] = legalized_name
                    new_mappings[legalized_name] = mapping
                    legalized += 1
                else:
                    new_mappings[module_name] = mapping
            
            # Also handle case where csynth has legalized name but we haven't mapped it yet
            # Check for any unmapped modules in csynth that might be legalized versions
            if hasattr(self, 'csynth_module_info') and "metrics" in self.csynth_module_info:
                for csynth_module in self.csynth_module_info["metrics"]:
                    if csynth_module not in new_mappings and csynth_module in reverse_name_map:
                        original_name = reverse_name_map[csynth_module]
                        if original_name in self.llvm_modules:
                            # Create mapping for this legalized module
                            llvm_info = self.llvm_modules[original_name]
                            if "line" in llvm_info and isinstance(llvm_info["line"], int):
                                line = llvm_info["line"]
                                
                                # First try to find as loop
                                if line in self.c_loops_by_line:
                                    c_loop = self.c_loops_by_line[line]
                                    new_mappings[csynth_module] = {
                                        "type": "line_based_legalized",
                                        "c_loop": c_loop["label"],
                                        "c_line": line,
                                        "start_line": c_loop["start_line"],
                                        "end_line": c_loop["end_line"],
                                        "c_function": c_loop["function"],
                                        "c_file": c_loop["file"],
                                        "llvm_module": original_name,
                                        "legalized_name": csynth_module
                                    }
                                    legalized += 1
                                else:
                                    # If not a loop, try function call mapping
                                    # Extract template function name (e.g., FFT_stage_spatial_unroll from FFT_stage_spatial_unroll<3>)
                                    base_func_name = original_name.split('<')[0] if '<' in original_name else original_name
                                    if base_func_name in self.c_functions_by_name:
                                        func_info = self.c_functions_by_name[base_func_name]
                                        new_mappings[csynth_module] = {
                                            "type": "function_call_legalized",
                                            "c_function": base_func_name,
                                            "c_line": line,  # call site line
                                            "start_line": func_info["start_line"],  # function definition boundaries
                                            "end_line": func_info["end_line"],  # function definition boundaries
                                            "function_def_line": func_info["start_line"],  # function definition line
                                            "function_def_start_line": func_info["start_line"],
                                            "function_def_end_line": func_info["end_line"],
                                            "c_file": func_info["file"],  # function definition file
                                            "llvm_module": original_name,
                                            "legalized_name": csynth_module
                                        }
                                        legalized += 1
            
            self.final_mappings = new_mappings
        
        if self.logger:
            self.logger.print(f"✓ Applied legalization to {legalized} modules")
        else:
            print(f"✓ Applied legalization to {legalized} modules")
    
    def validate_against_csynth(self):
        """Validate mappings against final RTL modules"""
        csynth_modules = set()
        if "metrics" in self.csynth_module_info:
            csynth_modules = set(self.csynth_module_info["metrics"].keys())
        
        mapped_modules = set(self.final_mappings.keys())
        
        # Find modules in csynth but not mapped
        unmapped_csynth = csynth_modules - mapped_modules
        
        # Find mapped modules not in csynth
        missing_from_csynth = mapped_modules - csynth_modules
        
        if self.logger:
            self.logger.print(f"\n📊 Validation Results:")
            self.logger.print(f"  Total csynth modules: {len(csynth_modules)}")
            self.logger.print(f"  Total mapped modules: {len(mapped_modules)}")
            self.logger.print(f"  Successfully mapped: {len(mapped_modules & csynth_modules)}")
            self.logger.print(f"  Unmapped csynth modules: {len(unmapped_csynth)}")
            self.logger.print(f"  Mapped but missing from csynth: {len(missing_from_csynth)}")
        else:
            print(f"\n📊 Validation Results:")
            print(f"  Total csynth modules: {len(csynth_modules)}")
            print(f"  Total mapped modules: {len(mapped_modules)}")
            print(f"  Successfully mapped: {len(mapped_modules & csynth_modules)}")
            print(f"  Unmapped csynth modules: {len(unmapped_csynth)}")
            print(f"  Mapped but missing from csynth: {len(missing_from_csynth)}")
        
        if unmapped_csynth:
            if self.logger:
                self.logger.print(f"\n❌ Unmapped csynth modules (first 10):")
                for module in list(unmapped_csynth)[:10]:
                    self.logger.print(f"    {module}")
            else:
                print(f"\n❌ Unmapped csynth modules (first 10):")
                for module in list(unmapped_csynth)[:10]:
                    print(f"    {module}")
        
        if missing_from_csynth:
            if self.logger:
                self.logger.print(f"\n⚠️  Mapped but missing from csynth (first 10):")
                for module in list(missing_from_csynth)[:10]:
                    self.logger.print(f"    {module}")
            else:
                print(f"\n⚠️  Mapped but missing from csynth (first 10):")
                for module in list(missing_from_csynth)[:10]:
                    print(f"    {module}")
        
        return len(unmapped_csynth), len(missing_from_csynth)
    
    def generate_mapping_summary(self):
        """Generate summary of mappings by type"""
        summary = {}
        for module_name, mapping in self.final_mappings.items():
            mapping_type = mapping["type"]
            if mapping_type not in summary:
                summary[mapping_type] = []
            summary[mapping_type].append(mapping)
        
        if self.logger:
            self.logger.print(f"\n📈 Mapping Summary:")
            for mapping_type, mappings in summary.items():
                self.logger.print(f"  {mapping_type}: {len(mappings)} modules")
        else:
            print(f"\n📈 Mapping Summary:")
            for mapping_type, mappings in summary.items():
                print(f"  {mapping_type}: {len(mappings)} modules")
        
        return summary
    
    def export_mapping_data_structure(self, output_file: str = None):
        """Export the complete mapping data structure"""
        if output_file:
            import json
            with open(output_file, 'w') as f:
                json.dump(self.final_mappings, f, indent=2)
            if self.logger:
                self.logger.print(f"\n💾 Exported mapping data structure to {output_file}")
            else:
                print(f"\n💾 Exported mapping data structure to {output_file}")
        
        return self.final_mappings
    
    def show_examples(self):
        """Show examples of each mapping type"""
        if self.logger:
            self.logger.print(f"\n🔍 Mapping Examples:")
        else:
            print(f"\n🔍 Mapping Examples:")
        
        summary = {}
        for module_name, mapping in self.final_mappings.items():
            mapping_type = mapping["type"]
            if mapping_type not in summary:
                summary[mapping_type] = []
            summary[mapping_type].append((module_name, mapping))
        
        for mapping_type, mappings in summary.items():
            if self.logger:
                self.logger.print(f"\n  {mapping_type.upper()} ({len(mappings)} modules):")
                # Show first example
                if mappings:
                    module_name, mapping = mappings[0]
                    self.logger.print(f"    Example: {module_name}")
                    for key, value in mapping.items():
                        if key != "type":
                            self.logger.print(f"      {key}: {value}")
                    
                    # Show a few more names for context
                    if len(mappings) > 1:
                        other_names = [name for name, _ in mappings[1:6]]  # Show up to 5 more
                        self.logger.print(f"    Other examples: {', '.join(other_names)}")
                        if len(mappings) > 6:
                            self.logger.print(f"    ... and {len(mappings) - 6} more")
            else:
                print(f"\n  {mapping_type.upper()} ({len(mappings)} modules):")
                # Show first example
                if mappings:
                    module_name, mapping = mappings[0]
                    print(f"    Example: {module_name}")
                    for key, value in mapping.items():
                        if key != "type":
                            print(f"      {key}: {value}")
                    
                    # Show a few more names for context
                    if len(mappings) > 1:
                        other_names = [name for name, _ in mappings[1:6]]  # Show up to 5 more
                        print(f"    Other examples: {', '.join(other_names)}")
                        if len(mappings) > 6:
                            print(f"    ... and {len(mappings) - 6} more")

    def annotate_csynth_with_c_info(self, output_file: str = None):
        """Annotate csynth module info with C source information"""
        if not output_file:
            output_file = self.log_dir / "rtl_c_annotated.json"
        
        # Generate timestamp
        from datetime import datetime
        timestamp = datetime.now().isoformat()
        
        # Start with csynth structure
        annotated_data = {
            "metadata": {
                "generated_by": "CToRTLMapper",
                "generated_at": timestamp,
                "source_files": ["c_to_rtl_mapping.json", "csynth_module_info.json"]
            },
            "top_module": self.csynth_module_info.get("top_module", {}),
            "rtl_hierarchy": self.csynth_module_info.get("hierarchy", {}),
            "modules": {}
        }
        
        # Get all csynth modules
        csynth_modules = set()
        if "metrics" in self.csynth_module_info:
            csynth_modules = set(self.csynth_module_info["metrics"].keys())
        
        # Get all mapped modules
        mapped_modules = set(self.final_mappings.keys())
        
        unmapped_modules = []
        
        # Annotate each csynth module
        for module_name, metrics in self.csynth_module_info.get("metrics", {}).items():
            module_info = {
                "performance": metrics,
                "c_source": None
            }
            
            # Add C source info if available
            if module_name in self.final_mappings:
                c_info = self.final_mappings[module_name]
                module_info["c_source"] = {
                    "mapping_type": c_info["type"],
                    "c_function": c_info.get("c_function"),
                    "c_line": c_info.get("c_line"),
                    "start_line": c_info.get("start_line"),
                    "end_line": c_info.get("end_line"),
                    "c_file": c_info.get("c_file"),
                    "c_loop": c_info.get("c_loop"),
                    "llvm_module": c_info.get("llvm_module")
                }
                
                # Add transformation-specific info
                if "c_lines" in c_info:  # flattened case
                    module_info["c_source"]["c_lines"] = c_info["c_lines"]
                    module_info["c_source"]["start_lines"] = c_info.get("start_lines", [])
                    module_info["c_source"]["end_lines"] = c_info.get("end_lines", [])
                    module_info["c_source"]["c_loops"] = c_info.get("c_loops", [])
                    module_info["c_source"]["combined_pattern"] = c_info.get("combined_pattern")
                
                if "legalized_name" in c_info:
                    module_info["c_source"]["legalized_name"] = c_info["legalized_name"]
            else:
                unmapped_modules.append(module_name)
            
            annotated_data["modules"][module_name] = module_info
        
        # Add summary
        annotated_data["summary"] = {
            "total_modules": len(csynth_modules),
            "mapped_modules": len(mapped_modules & csynth_modules),
            "unmapped_modules": len(unmapped_modules),
            "unmapped_module_list": unmapped_modules
        }
        
        # Save annotated data
        import json
        with open(output_file, 'w') as f:
            json.dump(annotated_data, f, indent=2)
        
        if self.logger:
            self.logger.print(f"\n📊 C Performance Annotation Results:")
            self.logger.print(f"  Total csynth modules: {len(csynth_modules)}")
            self.logger.print(f"  Successfully annotated: {len(mapped_modules & csynth_modules)}")
            self.logger.print(f"  Unmapped modules: {len(unmapped_modules)}")
        else:
            print(f"\n📊 C Performance Annotation Results:")
            print(f"  Total csynth modules: {len(csynth_modules)}")
            print(f"  Successfully annotated: {len(mapped_modules & csynth_modules)}")
            print(f"  Unmapped modules: {len(unmapped_modules)}")
        
        if unmapped_modules:
            if self.logger:
                self.logger.print(f"\n❌ Modules without C source info:")
                for module in unmapped_modules[:10]:  # Show first 10
                    self.logger.print(f"    {module}")
                if len(unmapped_modules) > 10:
                    self.logger.print(f"    ... and {len(unmapped_modules) - 10} more")
            else:
                print(f"\n❌ Modules without C source info:")
                for module in unmapped_modules[:10]:  # Show first 10
                    print(f"    {module}")
                if len(unmapped_modules) > 10:
                    print(f"    ... and {len(unmapped_modules) - 10} more")
        
        if self.logger:
            self.logger.print(f"\n💾 Saved annotated data to {output_file}")
        else:
            print(f"\n💾 Saved annotated data to {output_file}")
        
        return annotated_data, unmapped_modules
    
    def run_analysis(self):
        """Run the complete mapping analysis"""
        if self.logger:
            self.logger.print("🔍 Starting hierarchy mapping analysis...\n")
        else:
            print("🔍 Starting hierarchy mapping analysis...\n")
        
        if not self.load_json_files():
            return False
        
        self.build_c_hierarchy_index()
        self.build_llvm_hierarchy_index()
        
        self.map_simple_cases()
        self.map_function_level_cases()
        self.map_flattened_cases()
        self.apply_legalization()
        self.map_unmapped_to_parent()
        
        unmapped, missing = self.validate_against_csynth()
        summary = self.generate_mapping_summary()
        self.show_examples()
        
        # Export mapping data structure
        output_file = self.log_dir / "c_to_rtl_mapping.json"
        self.export_mapping_data_structure(str(output_file))
        
        # Create annotated performance data
        self.annotate_csynth_with_c_info()
        
        return unmapped == 0 and missing == 0
