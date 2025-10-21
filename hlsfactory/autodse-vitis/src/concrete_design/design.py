# concrete_design/design.py
import os
import shutil
from pathlib import Path
import subprocess
import time
import re
from .input_c_analyzer import InputCAnalyzer
from .csynth_analyzer import CSynthAnalyzer
from .llvm_ir_parser import LLVMIRParser
from .c_to_rtl_mapper import CToRTLMapper
from typing import Optional

# Import TeeLogger - handle import gracefully
try:
    from ..design_space.logger import TeeLogger
except ImportError:
    TeeLogger = None

class ConcreteDesign:
    def __init__(
        self, 
        source_dir: str, 
        work_dir: str, 
        tcl_filename: str = "project.tcl", 
        force_overwrite: bool = False
    ):
        """
        Args:
            source_dir: Path to original source files (e.g., template/test1_concrete_design)
            work_dir: Path to working directory for synthesis output (will be copied into)
            tcl_filename: Name of the TCL file to use (default: 'project.tcl')
            force_overwrite : If True, overwrite existing work_dir if it exists, 
                                otherwise does not copy if work_dir already exists.
        """
        self.source_dir = Path(source_dir).resolve()
        self.work_dir = Path(work_dir).resolve()

        self.source_files = list(self.source_dir.glob("*.cpp")) + list(self.source_dir.glob("*.h"))
        self.tcl_file = self.source_dir / tcl_filename
        assert self.tcl_file.exists(), f"{tcl_filename} not found in {source_dir}"

        # Logger will be set externally
        self.logger = None
        
        # Check if work_dir already exists
        if self.work_dir.exists():
            if force_overwrite:
                if self.logger:
                    self.logger.print(f"[ConcreteDesign] Overwriting existing work_dir: {self.work_dir}")
                else:
                    print(f"[ConcreteDesign] Overwriting existing work_dir: {self.work_dir}")
                shutil.rmtree(self.work_dir)
                shutil.copytree(self.source_dir, self.work_dir, dirs_exist_ok=False)
            else:
                if self.logger:
                    self.logger.print(f"[ConcreteDesign] work_dir {self.work_dir} already exists. Use exstisting files or set force_overwrite=True to overwrite.")
                else:
                    print(f"[ConcreteDesign] work_dir {self.work_dir} already exists. Use exstisting files or set force_overwrite=True to overwrite.")
        else:
            if self.logger:
                self.logger.print(f"[ConcreteDesign] Creating new work_dir: {self.work_dir}")
            else:
                print(f"[ConcreteDesign] Creating new work_dir: {self.work_dir}")
            shutil.copytree(self.source_dir, self.work_dir, dirs_exist_ok=False)

    def path(self) -> Path:
        """Return working directory path for this design."""
        return self.work_dir

    def set_logger(self, logger):
        """Set the logger for this ConcreteDesign."""
        self.logger = logger
    
    def __str__(self):
        return f"ConcreteDesign from {self.source_dir} into {self.work_dir}"

    def run_vitis_hls(self, timeout_min=10) -> str:
        """
        Run Vitis HLS synthesis in the working directory.

        Args:
            timeout_min: Maximum time allowed for synthesis (in minutes).

        Returns:
            "success" if synthesis finishes normally,
            "timeout" if time limit exceeded,
            "hls_failed" if HLS failed for another reason.
        """
        max_sec = timeout_min * 60
        tcl_name = self.tcl_file.name
        orig_dir = os.getcwd()
        os.chdir(self.work_dir)

        try:
            if self.logger:
                self.logger.print(f"[ConcreteDesign] Running Vitis HLS in {self.work_dir} using {tcl_name}...")
            else:
                print(f"[ConcreteDesign] Running Vitis HLS in {self.work_dir} using {tcl_name}...")
            subprocess.run(["vitis-run", "--mode", "hls", "--tcl", tcl_name], check=True, timeout=max_sec)
            return "success"
        except subprocess.TimeoutExpired:
            if self.logger:
                self.logger.print(f"[ConcreteDesign] ⏱ Timeout after {timeout_min} min in {self.work_dir}")
            else:
                print(f"[ConcreteDesign] ⏱ Timeout after {timeout_min} min in {self.work_dir}")
            return "timeout"
        except subprocess.CalledProcessError as e:
            if self.logger:
                self.logger.print(f"[ConcreteDesign] ❌ Vitis HLS failed in {self.work_dir}: {e}")
            else:
                print(f"[ConcreteDesign] ❌ Vitis HLS failed in {self.work_dir}: {e}")
            return "hls_failed"
        finally:
            os.chdir(orig_dir)

    def analyze_source(self):
        if self.logger:
            self.logger.print(f"[ConcreteDesign] Analyzing source code in {self.work_dir}...")
        else:
            print(f"[ConcreteDesign] Analyzing source code in {self.work_dir}...")        
        
        # Dynamically find source files from TCL configuration
        tcl_info = self.find_source_files_from_tcl()
        
        if not tcl_info['cpp_with_top_function']:
            if self.logger:
                self.logger.print(f"[ConcreteDesign] ❌ Could not find .cpp file containing top function '{tcl_info['top_function']}'")
            else:
                print(f"[ConcreteDesign] ❌ Could not find .cpp file containing top function '{tcl_info['top_function']}'")
            return
        
        if self.logger:
            self.logger.print(f"[ConcreteDesign] Found top function '{tcl_info['top_function']}' in {tcl_info['cpp_with_top_function']}")
            self.logger.print(f"[ConcreteDesign] Source files: {[str(f) for f in tcl_info['source_files']]}")
        else:
            print(f"[ConcreteDesign] Found top function '{tcl_info['top_function']}' in {tcl_info['cpp_with_top_function']}")
            print(f"[ConcreteDesign] Source files: {[str(f) for f in tcl_info['source_files']]}")
        
        # Use the found cpp file with top function
        analyzer = InputCAnalyzer(str(tcl_info['cpp_with_top_function']))
        if self.logger:
            analyzer.set_logger(self.logger)
        analyzer.generate_input_c_hierarchy()
        analyzer.save_pretty_print_hierarchy_to_txt()
        analyzer.save_hierarchy_to_json()
        return 

    def analyze_csynth(self):
        csynth_analyzer =  CSynthAnalyzer(self.work_dir)
        if self.logger:
            csynth_analyzer.set_logger(self.logger)
        metrics = csynth_analyzer.parse_csynth_report()
        csynth_analyzer.save_csynth_module_info_to_json()

        csynth_analyzer.parse_transform_messages()
        csynth_analyzer.save_transform_messages_to_json()

        csynth_analyzer.parse_llvm_ir_hierarchy()
        csynth_analyzer.save_llvm_ir_hierarchy_to_json()
        csynth_analyzer.save_pretty_print_llvm_ir_hierarchy_to_txt()
        return

    def _read_tcl_with_sources(self, tcl_file):
        """
        Read TCL file content and recursively read any sourced files.
        """
        tcl_content = tcl_file.read_text()
        
        # Find source commands and recursively read sourced files
        source_pattern = r'source\s+([^\s;]+)'
        for match in re.finditer(source_pattern, tcl_content):
            source_path = match.group(1)
            # Resolve relative paths
            if not source_path.startswith('/'):
                source_path = tcl_file.parent / source_path
            else:
                source_path = Path(source_path)
            
            if source_path.exists():
                sourced_content = self._read_tcl_with_sources(source_path)
                tcl_content += f"\n# === Sourced from {source_path} ===\n"
                tcl_content += sourced_content
                tcl_content += f"\n# === End of {source_path} ===\n"
        
        return tcl_content

    def find_source_files_from_tcl(self):
        """
        Parse the TCL file to find source files and top function.
        Returns a dictionary with 'top_function', 'source_files', and 'cpp_with_top_function'.
        """
        tcl_content = self._read_tcl_with_sources(self.tcl_file)
        
        # Find top function
        top_function_match = re.search(r'set_top\s+"?([^"\s]+)"?', tcl_content)
        top_function = top_function_match.group(1) if top_function_match else None
        
        # Find source files from add_files commands
        source_files = []
        
        # Pattern 1: Direct add_files with .cpp extension
        add_files_pattern1 = r'add_files\s+"?([^"\s]+\.cpp)"?'
        for match in re.finditer(add_files_pattern1, tcl_content):
            source_files.append(match.group(1))
        
        # Pattern 2: add_files without extension (like "3mm")
        add_files_pattern2 = r'add_files\s+"?([^"\s\.]+)"?(?!\.\w+)'
        for match in re.finditer(add_files_pattern2, tcl_content):
            filename = match.group(1)
            # Skip testbench files
            if 'testbench' not in filename.lower():
                source_files.append(f"{filename}.cpp")
        
        # Pattern 3: source_files list definition
        source_files_list_match = re.search(r'set\s+source_files\s+\[list\s+([^\]]+)\]', tcl_content, re.DOTALL)
        if source_files_list_match:
            files_content = source_files_list_match.group(1)
            # Extract file paths with variable substitution like "$src_path/FFT.cpp"
            file_matches = re.findall(r'"?\$[^/\s]*/([\w.]+\.cpp)"?', files_content)
            source_files.extend(file_matches)
            # Also try without variable substitution
            file_matches2 = re.findall(r'"([^"$\s]+\.cpp)"', files_content)
            source_files.extend(file_matches2)
        
        # Remove duplicates and resolve paths
        source_files = list(set(source_files))
        resolved_files = []
        for file_path in source_files:
            # Handle basic variable substitution for $project_path
            if file_path.startswith('$project_path/'):
                file_path = file_path.replace('$project_path/', '')
            resolved_path = self.work_dir / file_path
            if resolved_path.exists():
                resolved_files.append(resolved_path)
        
        # Find which source file contains the top function
        cpp_with_top_function = None
        if top_function:
            for cpp_file in resolved_files:
                if cpp_file.suffix == '.cpp':
                    try:
                        content = cpp_file.read_text()
                        # Look for function definition (not just function calls)
                        # Match: return_type function_name(...) or void function_name(...)
                        function_def_pattern = rf'^\s*(?:[\w\s:<>,*&]+\s+)?{re.escape(top_function)}\s*\([^)]*\)\s*\{{'
                        if re.search(function_def_pattern, content, re.MULTILINE):
                            cpp_with_top_function = cpp_file
                            break
                    except Exception as e:
                        if self.logger:
                            self.logger.print(f"Warning: Could not read {cpp_file}: {e}")
                        else:
                            print(f"Warning: Could not read {cpp_file}: {e}")
        
        return {
            'top_function': top_function,
            'source_files': resolved_files,
            'cpp_with_top_function': cpp_with_top_function
        }

    def map_c_to_rtl(self):
        mapper = CToRTLMapper(self.work_dir / "logs")
        if self.logger:
            mapper.set_logger(self.logger)
        return mapper.run_analysis()
    
    def run_all_analyses(self):
        """
        Run all analyses in sequence:
        - Analyze source code
        - Analyze CSynth output
        - Parse LLVM IR
        - Map C to RTL
        """
        self.analyze_source()
        self.analyze_csynth()
        self.map_c_to_rtl()
        if self.logger:
            self.logger.print("[ConcreteDesign] All analyses completed successfully.")
        else:
            print("[ConcreteDesign] All analyses completed successfully.")
        return  









