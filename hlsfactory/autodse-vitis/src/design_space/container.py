#!/usr/bin/env python3
"""
Template Space Container for DSE.
Manages single design template with HLS_AUTO pragma markers.
"""

import os
import re
import shutil
import json
from pathlib import Path
from typing import Dict, List, Tuple, Optional
import random
from .logger import TeeLogger


class TemplateContainer:
    """
    Manages a single design template with HLS_AUTO pragma exploration.
    """
    
    def __init__(self, template_dir: str):
        """
        Initialize container with template directory.
        
        Args:
            template_dir: Path to template directory (e.g., "database/n256_UF4")
        """
        self.template_dir = Path(template_dir).resolve()
        if not self.template_dir.exists():
            raise FileNotFoundError(f"Template directory not found: {template_dir}")
        
        self.pragma_locations = []  # List of (file_path, line_num, pragma_type, options)
        self.pragma_options = {}    # Dict mapping pragma types to available options
        self.template_files = {}    # Dict mapping file names to content
        self.format_type = None     # "in_place_format" or "json_format"
        self.json_options = {}      # Options from JSON file (for json_format)
        
        # Logger will be set externally
        self.logger = None
    
    def set_logger(self, logger: TeeLogger):
        """Set the logger for this container."""
        self.logger = logger
    
    def load_template(self) -> Dict:
        """
        Load template and parse all HLS_AUTO pragma markers.
        
        Returns:
            Dict with template info: {
                "pragma_locations": List of pragma locations,
                "pragma_options": Dict of available options per pragma type,
                "template_files": Dict of file contents
            }
        """
        if self.logger:
            self.logger.print(f"[TemplateContainer] Loading template from {self.template_dir}")
        else:
            print(f"[TemplateContainer] Loading template from {self.template_dir}")
        
        # Reset pragma locations and options to ensure idempotency
        self.pragma_locations = []
        self.pragma_options = {}

        # Load all source files
        cpp_files = list(self.template_dir.glob("*.cpp"))
        h_files = list(self.template_dir.glob("*.h"))
        
        for file_path in cpp_files + h_files:
            with open(file_path, 'r') as f:
                self.template_files[file_path.name] = f.read()
        
        # Detect format type and parse pragmas accordingly
        self.format_type = self._detect_format_type()
        if self.format_type == "json_format":
            self._load_json_options()
            self._parse_json_format_pragmas()
        else:
            self._parse_hls_auto_pragmas()
        
        if self.logger:
            self.logger.print(f"[TemplateContainer] Format type: {self.format_type}")
            self.logger.print(f"[TemplateContainer] Found {len(self.pragma_locations)} pragma locations")
            self.logger.print(f"[TemplateContainer] Pragma types: {list(self.pragma_options.keys())}")
        else:
            print(f"[TemplateContainer] Format type: {self.format_type}")
            print(f"[TemplateContainer] Found {len(self.pragma_locations)} pragma locations")
            print(f"[TemplateContainer] Pragma types: {list(self.pragma_options.keys())}")
        
        return {
            "pragma_locations": self.pragma_locations,
            "pragma_options": self.pragma_options,
            "template_files": self.template_files
        }
    
    def _parse_hls_auto_pragmas(self):
        """
        Parse all HLS_AUTO pragma markers from template files.
        """
        # Regex pattern for HLS_AUTO pragmas
        # Example: #pragma HLS_AUTO dataflow auto{NULL, disable_start_propagation}
        pragma_pattern = r'#pragma\s+HLS_AUTO\s+(\w+)\s+([^{]*?)auto\{([^}]+)\}'
        
        for filename, content in self.template_files.items():
            lines = content.split('\n')
            for line_num, line in enumerate(lines, 1):
                match = re.search(pragma_pattern, line)
                if match:
                    pragma_type = match.group(1)  # e.g., "dataflow", "inline", "performance"
                    pragma_params = match.group(2).strip()  # e.g., "target_ti=", ""
                    options_str = match.group(3)  # e.g., "NULL, disable_start_propagation"
                    
                    # Parse options
                    options = [opt.strip() for opt in options_str.split(',')]
                    
                    # Classify pragma type
                    pragma_category = self._classify_pragma_type(pragma_type)
                    
                    # Store pragma location
                    # Find region (function or loop)
                    region = self._find_region(lines, line_num - 1)

                    pragma_location = {
                        "file": filename,
                        "line": line_num,
                        "pragma_type": pragma_type,
                        "pragma_params": pragma_params,
                        "options": options,
                        "category": pragma_category,
                        "original_line": line,
                        "region": region
                    }
                    
                    self.pragma_locations.append(pragma_location)
                    
                    # Update pragma options registry
                    if pragma_type not in self.pragma_options:
                        self.pragma_options[pragma_type] = set()
                    self.pragma_options[pragma_type].update(options)
        
        # Convert sets to lists for JSON serialization
        for pragma_type in self.pragma_options:
            self.pragma_options[pragma_type] = list(self.pragma_options[pragma_type])
    
    def _detect_format_type(self) -> str:
        """
        Detect whether template uses in_place_format or json_format.
        
        Returns:
            "in_place_format" or "json_format"
        """
        # Check if JSON file exists
        json_files = list(self.template_dir.glob("*.json"))
        if json_files:
            # Check if any source files contain json_format pragma patterns
            json_pragma_pattern = r'#pragma\s+HLS\s+\w+.*?auto\{__\w+__\w+\}'
            for filename, content in self.template_files.items():
                if re.search(json_pragma_pattern, content):
                    return "json_format"
        
        # Check for in_place_format patterns
        in_place_pragma_pattern = r'#pragma\s+HLS_AUTO'
        for filename, content in self.template_files.items():
            if re.search(in_place_pragma_pattern, content):
                return "in_place_format"
        
        # Default to in_place_format if no pragmas found
        return "in_place_format"
    
    def _load_json_options(self):
        """
        Load pragma options from JSON file (for json_format).
        """
        json_files = list(self.template_dir.glob("*.json"))
        if not json_files:
            raise FileNotFoundError(f"JSON format detected but no JSON file found in {self.template_dir}")
        
        # Use the first JSON file found
        json_file = json_files[0]
        with open(json_file, 'r') as f:
            self.json_options = json.load(f)
        
        if self.logger:
            self.logger.print(f"[TemplateContainer] Loaded {len(self.json_options)} pragma option sets from {json_file.name}")
        else:
            print(f"[TemplateContainer] Loaded {len(self.json_options)} pragma option sets from {json_file.name}")
    
    def _parse_json_format_pragmas(self):
        """
        Parse JSON format pragmas (e.g., #pragma HLS pipeline auto{__PIPE__L0}).
        """
        # Regex pattern for JSON format pragmas
        # Handles: #pragma HLS pipeline auto{__PIPE__L0} and #pragma HLS unroll factor=auto{__PARA__L0}
        json_pragma_pattern = r'#pragma\s+HLS\s+(\w+)(\s+[^{]*?)?auto\{(__\w+__\w+)\}'
        
        for filename, content in self.template_files.items():
            lines = content.split('\n')
            for line_num, line in enumerate(lines, 1):
                match = re.search(json_pragma_pattern, line)
                if match:
                    pragma_type = match.group(1)  # e.g., "pipeline", "unroll"
                    pragma_params = match.group(2)  # e.g., " factor=", None
                    placeholder = match.group(3)  # e.g., "__PIPE__L0", "__PARA__L0"
                    
                    if pragma_params:
                        pragma_params = pragma_params.strip()
                    else:
                        pragma_params = ""
                    
                    # Get options from JSON
                    options = self.json_options.get(placeholder, [])
                    
                    # Classify pragma type
                    pragma_category = self._classify_pragma_type(pragma_type)
                    
                    # Store pragma location
                    # Find region (function or loop) by parsing the actual code
                    # Always use _find_region to detect the actual loop label, don't trust placeholder
                    region = self._find_region(lines, line_num - 1)

                    pragma_location = {
                        "file": filename,
                        "line": line_num,
                        "pragma_type": pragma_type,
                        "pragma_params": pragma_params,
                        "placeholder": placeholder,
                        "options": options,
                        "category": pragma_category,
                        "original_line": line,
                        "region": region
                    }
                    
                    self.pragma_locations.append(pragma_location)
                    
                    # Update pragma options registry
                    if pragma_type not in self.pragma_options:
                        self.pragma_options[pragma_type] = set()
                    
                    # Handle both set (first load) and list (reload) cases
                    if isinstance(self.pragma_options[pragma_type], list):
                        # Already converted to list - extend it
                        existing_options = set(self.pragma_options[pragma_type])
                        existing_options.update(options)
                        self.pragma_options[pragma_type] = list(existing_options)
                    else:
                        # Still a set - update normally
                        self.pragma_options[pragma_type].update(options)
        
        # Convert sets to lists for JSON serialization
        for pragma_type in self.pragma_options:
            if isinstance(self.pragma_options[pragma_type], set):
                self.pragma_options[pragma_type] = list(self.pragma_options[pragma_type])
    
    def _classify_pragma_type(self, pragma_type: str) -> str:
        """
        Classify pragma type into categories: function, loop, op, array.
        
        Args:
            pragma_type: The pragma directive name (e.g., "dataflow", "pipeline")
            
        Returns:
            Category string: "function", "loop", "op", or "array"
        """
        function_pragmas = {"dataflow", "inline", "performance"}
        loop_pragmas = {"pipeline", "unroll"}
        op_pragmas = {"bind_op"}
        array_pragmas = {"array_partition", "bind_storage"}
        
        if pragma_type in function_pragmas:
            return "function"
        elif pragma_type in loop_pragmas:
            return "loop"
        elif pragma_type in op_pragmas:
            return "op"
        elif pragma_type in array_pragmas:
            return "array"
        else:
            return "unknown"
    
    def generate_variant(self, pragma_choices: List[str], variant_id: int, output_dir: str) -> str:
        """
        Generate a design variant with specific pragma choices.
        
        Args:
            pragma_choices: List of chosen options for each pragma location (empty string = no pragma)
            variant_id: Unique identifier for this variant
            output_dir: Base output directory for variants
            
        Returns:
            Path to generated variant directory
        """
        variant_dir = Path(output_dir) / f"variant_{variant_id}"
        
        # Remove existing variant directory if it exists
        if variant_dir.exists():
            shutil.rmtree(variant_dir)
        
        # Create variant directory
        variant_dir.mkdir(parents=True, exist_ok=True)
        
        if self.logger:
            self.logger.print(f"[TemplateContainer] Generating variant {variant_id} in {variant_dir}")
        else:
            print(f"[TemplateContainer] Generating variant {variant_id} in {variant_dir}")
        
        # Copy and modify each template file
        for filename, original_content in self.template_files.items():
            modified_content = self._apply_pragma_choices(filename, original_content, pragma_choices)
            
            variant_file = variant_dir / filename
            with open(variant_file, 'w') as f:
                f.write(modified_content)
        
        # Copy TCL files and other non-source files
        for tcl_file in self.template_dir.glob("*.tcl"):
            shutil.copy2(tcl_file, variant_dir)
        
        if self.logger:
            self.logger.print(f"[TemplateContainer] Variant {variant_id} generated successfully")
        else:
            print(f"[TemplateContainer] Variant {variant_id} generated successfully")
        return str(variant_dir)
    
    def _apply_pragma_choices(self, filename: str, content: str, pragma_choices: List[str]) -> str:
        """
        Apply pragma choices to file content by replacing pragma markers.
        
        Args:
            filename: Name of the file being processed
            content: Original file content
            pragma_choices: List of chosen pragma options
            
        Returns:
            Modified file content with pragmas replaced
        """
        lines = content.split('\n')
        
        # Find pragma locations for this file
        file_pragmas = [p for p in self.pragma_locations if p["file"] == filename]
        
        # Apply choices (in reverse order to preserve line numbers)
        for i, pragma_info in enumerate(reversed(file_pragmas)):
            choice_idx = len(self.pragma_locations) - 1 - i
            if choice_idx < len(pragma_choices):
                choice = pragma_choices[choice_idx]
                line_idx = pragma_info["line"] - 1  # Convert to 0-based index
                
                if choice == "" or choice == "NULL" or choice == "off":
                    # Remove pragma entirely
                    lines[line_idx] = ""
                else:
                    # Replace with specific pragma
                    new_pragma = self._build_pragma_line(pragma_info, choice)
                    lines[line_idx] = new_pragma
        
        return '\n'.join(lines)
    
    def _build_pragma_line(self, pragma_info: Dict, choice: str) -> str:
        """
        Build a specific pragma line from template and choice.
        
        Args:
            pragma_info: Pragma location information
            choice: Chosen option for this pragma
            
        Returns:
            Complete pragma line string
        """
        pragma_type = pragma_info["pragma_type"]
        pragma_params = pragma_info["pragma_params"]
        
        if self.format_type == "json_format":
            # For JSON format: #pragma HLS pipeline off  or  #pragma HLS unroll factor=2
            if pragma_params:
                return f"    #pragma HLS {pragma_type} {pragma_params}{choice}"
            else:
                return f"    #pragma HLS {pragma_type} {choice}"
        else:
            # For in-place format: #pragma HLS dataflow  or  #pragma HLS performance target_ti=64
            if pragma_params:
                return f"    #pragma HLS {pragma_type} {pragma_params}{choice}"
            else:
                return f"    #pragma HLS {pragma_type} {choice}"
    
    def get_initial_choices(self) -> List[str]:
        """
        Get initial pragma choices (all empty strings).
        
        Returns:
            List of empty strings for each pragma location
        """
        return [""] * len(self.pragma_locations)
    
    def sample_random_choices(self) -> List[str]:
        """
        Sample random pragma choices for testing.
        
        Returns:
            List of random choices for each pragma location
        """
        choices = []
        for pragma_info in self.pragma_locations:
            options = pragma_info["options"]
            # 50% chance of no pragma, 50% chance of random option
            if random.random() < 0.5:
                choices.append("")
            else:
                choices.append(random.choice(options))
        
        return choices

    def _find_region(self, lines: List[str], pragma_line_index: int) -> str:
        """
        Find the enclosing function or loop name for a pragma.

        Args:
            lines: All lines of the source file
            pragma_line_index: The 0-based index of the pragma line

        Returns:
            Region name (e.g., "function_name", "L1") or "unknown"
        """
        # Regex for C/C++ function definition (simplified)
        # Catches: void function_name(...), int function(...), etc.
        func_pattern = r'\b\w+\s+\**\s*(\w+)\s*\([^)]*\)\s*{'
        
        # Regex for loop statements
        loop_pattern = r'\b(for|while)\s*\([^)]*\)'

        # Regex for labels (e.g., L1:) - can be standalone or on same line as loop
        label_pattern_standalone = r'^\s*(\w+):\s*(//.*)?$'
        label_pattern_with_loop = r'^\s*(\w+):\s*for\s*\('  # e.g., "L1: for (...)"

        # Search backwards from the pragma line for the start of an enclosing scope
        for i in range(pragma_line_index - 1, -1, -1):
            line = lines[i].strip()

            # Is it a loop?
            if re.search(loop_pattern, line):
                # First check for a label on the SAME line as the loop (e.g., "L1: for (...)")
                label_with_loop_match = re.match(label_pattern_with_loop, line)
                if label_with_loop_match:
                    return label_with_loop_match.group(1)

                # Check for a standalone label
                label_match = re.match(label_pattern_standalone, line)
                if label_match:
                    return label_match.group(1)

                # Check previous non-empty line for a standalone label
                for j in range(i - 1, -1, -1):
                    prev_line = lines[j].strip()
                    if prev_line:
                        label_match = re.match(label_pattern_standalone, prev_line)
                        if label_match:
                            return label_match.group(1)
                        # If we find something else, stop looking for a label for this loop
                        break

                # If we found a loop but no label, we will fall back to function name
                break

            # Is it a function definition?
            match = re.search(func_pattern, line)
            if match:
                return match.group(1)

        # Fallback: if we broke from the loop search (found an unlabeled loop), find the function.
        for i in range(pragma_line_index - 1, -1, -1):
            line = lines[i].strip()
            match = re.search(func_pattern, line)
            if match:
                return match.group(1)

        return "unknown"
