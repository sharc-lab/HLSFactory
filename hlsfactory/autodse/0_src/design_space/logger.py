#!/usr/bin/env python3
"""
TeeLogger - Logger that writes to both console and file simultaneously.
"""

import json
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Optional, Any


class TeeLogger:
    """Logger that writes to both console and file simultaneously."""
    
    def __init__(self, log_file_path, console_output=True):
        """
        Initialize TeeLogger.
        
        Args:
            log_file_path: Path to the log file
            console_output: Whether to also print to console
        """
        self.log_file_path = Path(log_file_path)
        self.console_output = console_output
        self.log_file = None
        
        # DSE trace file
        self.dse_trace_file = None
        self.dse_trace_initialized = False
        
        # LevelHeap DSE trace file
        self.levelheap_trace_file = None
        self.levelheap_trace_initialized = False
        
        # Final trace JSON file
        self.final_trace_json_file = None
        self.final_trace_json_initialized = False
        
        # Ensure log directory exists
        self.log_file_path.parent.mkdir(parents=True, exist_ok=True)
        
        # Open log file for writing
        self.log_file = open(self.log_file_path, 'w', encoding='utf-8')
        
        # Write header to log file
        self.log_file.write(f"# DSE Test Log - {datetime.now().isoformat()}\n")
        self.log_file.write("=" * 60 + "\n\n")
        self.log_file.flush()
    
    def print(self, *args, **kwargs):
        """Print function that writes to both console and log file."""
        # Convert all arguments to strings and join them
        message = ' '.join(str(arg) for arg in args)
        
        # Handle special kwargs like 'end' and 'sep'
        end = kwargs.get('end', '\n')
        sep = kwargs.get('sep', ' ')
        
        if len(args) > 1:
            message = sep.join(str(arg) for arg in args)
        
        # Write to console if enabled
        if self.console_output:
            print(*args, **kwargs)
        
        # Write to log file
        if self.log_file:
            self.log_file.write(message + end)
            self.log_file.flush()
    
    def close(self):
        """Close the log file."""
        if self.log_file:
            self.log_file.write(f"\n# Log ended - {datetime.now().isoformat()}\n")
            self.log_file.close()
            self.log_file = None
    
    def __enter__(self):
        """Context manager entry."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.close()
    
    def _format_score(self, score):
        """
        Format score for display, handling special cases.
        
        Args:
            score: Score value to format
            
        Returns:
            Formatted string representation of the score
        """
        if score == float('inf'):
            return "EXCEED RESOURCES"  # Defensive handling for legacy float('inf')
        elif isinstance(score, str):
            # Handle all failure types
            if score in ["EXCEED RESOURCES", "TIME OUT", "FAIL SYNTHESIS"]:
                return score
            elif "EXCEED" in score or "RESOURCES" in score:
                return score  # Backward compatibility
        
        if score == "N/A" or score is None:
            return "N/A"
        else:
            return str(score)
    
    def save_dse_trace(self, iteration: int, work_dir: str, bottleneck_module: str, 
                      affecting_pragmas: List[Dict], changed_pragma: Optional[Dict],
                      pragma_locations: List[Dict], current_choices: List[str]):
        """
        Save DSE trace information to dse_trace.log.
        
        Args:
            iteration: Current iteration number
            work_dir: Work directory containing logs
            bottleneck_module: Name of the bottleneck module
            affecting_pragmas: List of pragmas affecting the bottleneck
            changed_pragma: Details of the pragma that was changed
            pragma_locations: All pragma location information
            current_choices: Current pragma choices
        """
        if not self.dse_trace_initialized:
            self._initialize_dse_trace(work_dir)
        
        if not self.dse_trace_file:
            return
            
        logs_dir = Path(work_dir) / "logs"
        csynth_file = logs_dir / "csynth_module_info.json"
        rtl_file = logs_dir / "rtl_c_annotated.json"
        
        with open(self.dse_trace_file, 'a') as f:
            f.write(f"===== Iteration {iteration} =====\n")
            
            # Write module hierarchy table
            if csynth_file.exists() and rtl_file.exists():
                try:
                    with open(csynth_file) as cf:
                        csynth_data = json.load(cf)
                    with open(rtl_file) as rf:
                        rtl_data = json.load(rf)
                    
                    self._write_module_hierarchy_table(f, csynth_data, rtl_data)
                    
                    # Get bottleneck module info
                    modules_performance = csynth_data.get("metrics", {})
                    c_mappings = rtl_data.get("modules", {})
                    
                    if bottleneck_module in modules_performance:
                        perf = modules_performance[bottleneck_module]
                        latency = perf.get("Worst-caseLatency", "N/A")
                        ii = perf.get("PipelineInitiationInterval", "N/A")
                        f.write(f"Bottleneck module: {bottleneck_module} (Latency={latency}, II={ii})\n")
                        
                        # Get C source info
                        c_info = c_mappings.get(bottleneck_module, {}).get("c_source", {})
                        mapping = c_info.get("mapping_type", "?")
                        if c_info.get("c_loops") and isinstance(c_info["c_loops"], list) and len(c_info["c_loops"]) > 0:
                            c_type, c_name = "loops", ", ".join(c_info["c_loops"])
                        elif c_info.get("c_loop"):
                            c_type, c_name = "loop", c_info["c_loop"]
                        elif c_info.get("c_function"):
                            c_type, c_name = "function", c_info["c_function"]
                        else:
                            c_type = c_info.get("c_type", "unknown")
                            c_name = c_info.get("c_name", c_info.get("llvm_module", "unknown"))
                        cline = c_info.get("c_line")
                        extra = f", line {cline}" if cline else ""
                        f.write(f"Bottleneck module → C {c_type}: {c_name} (mapping={mapping}{extra})\n")
                    else:
                        f.write(f"Bottleneck module: {bottleneck_module} (metrics not available)\n")
                        f.write("function unknown\n")
                    
                except Exception as e:
                    f.write("Error reading synthesis results\n")
                    f.write(f"Bottleneck module: {bottleneck_module} (error: {e})\n")
                    f.write("function unknown\n")
            else:
                f.write("Module hierarchy not available (synthesis failed)\n")
                f.write(f"Bottleneck module: {bottleneck_module}\n")
                f.write("function unknown\n")
            
            # Write region of interests pragmas
            f.write("Pragmas (Points of interests):\n")
            if affecting_pragmas:
                for pragma in affecting_pragmas:
                    f.write(f"  {pragma['pragma_type']} in {pragma['file']}:{pragma['line']} -> '{pragma['current_choice']}'\n")
            else:
                f.write("  (none found)\n")
            
            # Write selected changed pragma
            f.write("Selected changed pragma:\n")
            if changed_pragma:
                f.write(f"  {changed_pragma['pragma_type']} in {changed_pragma['file']}:{changed_pragma['line']} -> '{changed_pragma['new_choice']}'\n")
            else:
                f.write("  (none)\n")
            
            f.write("\n")
    
    def log_node_expansion_start(self, iteration: int, deepest_level: int, current_node: Dict, 
                                pragma_locations: List[Dict], focus_pragma_idx: int):
        """Log when a node is selected for expansion."""
        if not self.levelheap_trace_file:
            return
            
        with open(self.levelheap_trace_file, 'a') as f:
            f.write(f"\n[EXPANSION] === Iteration {iteration}: Node Expansion Started ===\n")
            f.write(f"[EXPANSION] Selected node from Level {deepest_level} for expansion:\n")
            
            # Format tuned pragmas with detailed info
            tuned_details = []
            for idx, val in current_node.get('tuned', []):
                if idx < len(pragma_locations):
                    pragma_info = pragma_locations[idx]
                    region = pragma_info.get('region', 'unknown')
                    pragma_detail = f"{idx}({pragma_info['pragma_type']}@{pragma_info['file']}:{pragma_info['line']}, {region})={val}"
                    tuned_details.append(pragma_detail)
                else:
                    tuned_details.append(f"{idx}={val}")
            tuned_str = ", ".join(tuned_details) if tuned_details else "none"
            
            f.write(f"[EXPANSION]   Node Score: {current_node.get('score', 'N/A')}\n")
            f.write(f"[EXPANSION]   Tuned pragmas: [{tuned_str}]\n")
            f.write(f"[EXPANSION]   Next focus pragma: {focus_pragma_idx}")
            
            # Add focus pragma details
            if focus_pragma_idx < len(pragma_locations):
                pragma_info = pragma_locations[focus_pragma_idx]
                region = pragma_info.get('region', 'unknown')
                options = pragma_info.get('options', [])
                f.write(f" ({pragma_info['pragma_type']}@{pragma_info['file']}:{pragma_info['line']}, {region})")
                f.write(f"\n[EXPANSION]   Focus pragma options: {options}\n")
            else:
                f.write(f" (invalid index)\n")
            
            f.write(f"[EXPANSION]   Bottleneck module: {current_node.get('bottleneck_module', 'N/A')}\n")
            f.write("\n")

    def log_child_node_generation(self, iteration: int, focus_pragma_idx: int, old_value: str, 
                                  new_value: str, new_score: float, next_level: int, 
                                  next_focus: int, pragma_locations: List[Dict]):
        """Log when a child node is generated and added to the heap."""
        if not self.levelheap_trace_file:
            return
            
        with open(self.levelheap_trace_file, 'a') as f:
            f.write(f"[EXPANSION] Child node generated for pragma {focus_pragma_idx}: '{old_value}' -> '{new_value}'\n")
            f.write(f"[EXPANSION]   New score: {self._format_score(new_score)}\n")
            f.write(f"[EXPANSION]   Added to Level {next_level}\n")
            
            if next_focus is not None and next_focus < len(pragma_locations):
                pragma_info = pragma_locations[next_focus]
                region = pragma_info.get('region', 'unknown')
                f.write(f"[EXPANSION]   Next focus for child: {next_focus} ({pragma_info['pragma_type']}@{pragma_info['file']}:{pragma_info['line']}, {region})\n")
            else:
                f.write(f"[EXPANSION]   Next focus for child: {next_focus} (end of exploration)\n")
            f.write("\n")

    def log_heap_level_update(self, level_heap: Dict[int, List[Dict]]):
        """Log current heap state showing how many nodes are at each level."""
        if not self.levelheap_trace_file:
            return
            
        with open(self.levelheap_trace_file, 'a') as f:
            f.write(f"[EXPANSION] Updated heap state:\n")
            if level_heap:
                for level in sorted(level_heap.keys()):
                    nodes = level_heap[level]
                    f.write(f"[EXPANSION]   Level {level}: {len(nodes)} nodes\n")
            else:
                f.write(f"[EXPANSION]   Heap is empty\n")
            f.write("\n")

    def save_levelheap_dfs_trace(self, iteration: int, work_dir: str, bottleneck_module: str, 
                                affecting_pragmas: List[Dict], changed_pragma: Optional[Dict],
                                pragma_locations: List[Dict], current_choices: List[str],
                                level_heap: Dict, current_node: Dict, global_best_score: float):
        """
        Save LevelHeap DFS trace information to levelheap_dse_trace.log.
        
        Args:
            iteration: Current iteration number
            work_dir: Work directory containing logs
            bottleneck_module: Name of the bottleneck module
            affecting_pragmas: List of pragmas affecting the bottleneck
            changed_pragma: Details of the pragma that was changed
            pragma_locations: All pragma location information
            current_choices: Current pragma choices
            level_heap: Current state of the level heap
            current_node: The node being expanded
            global_best_score: Current global best score
        """
        if not self.levelheap_trace_initialized:
            self._initialize_levelheap_trace(work_dir)
        
        if not self.levelheap_trace_file:
            return
            
        logs_dir = Path(work_dir) / "logs"
        csynth_file = logs_dir / "csynth_module_info.json"
        rtl_file = logs_dir / "rtl_c_annotated.json"
        
        with open(self.levelheap_trace_file, 'a') as f:
            f.write(f"===== LevelHeap DFS Iteration {iteration} =====\n")
            
            # Write complete LevelHeap state with all levels and nodes
            f.write("===== COMPLETE LEVELHEAP STATE =====\n")
            if level_heap:
                total_nodes = sum(len(nodes) for nodes in level_heap.values())
                f.write(f"Total levels: {len(level_heap)}\n")
                f.write(f"Total nodes: {total_nodes}\n")
                f.write(f"Global best score: {global_best_score}\n\n")
                
                # Print all levels from 0 to max level (including empty levels)
                max_level = max(level_heap.keys()) if level_heap else 0
                for level in range(max_level + 1):
                    nodes = level_heap.get(level, [])
                    f.write(f"Level {level} ({len(nodes)} nodes):\n")
                    
                    if not nodes:
                        f.write("  (empty level)\n")
                    else:
                        for i, node in enumerate(nodes):
                            # Format tuned pragmas with detailed info
                            tuned_details = []
                            for idx, val in node.get('tuned', []):
                                if idx < len(pragma_locations):
                                    pragma_info = pragma_locations[idx]
                                    region = pragma_info.get('region', 'unknown')
                                    pragma_detail = f"{idx}({pragma_info['pragma_type']}@{pragma_info['file']}:{pragma_info['line']}, {region})={val}"
                                    tuned_details.append(pragma_detail)
                                else:
                                    tuned_details.append(f"{idx}={val}")
                            tuned_str = ", ".join(tuned_details) if tuned_details else "none"
                            
                            # Format next focus with detailed info
                            next_focus = node.get('next_focus', None)
                            if next_focus is not None and next_focus < len(pragma_locations):
                                pragma_info = pragma_locations[next_focus]
                                next_focus_str = f"{next_focus}({pragma_info['pragma_type']}@{pragma_info['file']}:{pragma_info['line']})"
                            else:
                                next_focus_str = str(next_focus) if next_focus is not None else 'None'
                            
                            # Format score
                            score = node.get('score', 'N/A')
                            score_str = self._format_score(score)
                            
                            f.write(f"  Node {i+1}:\n")
                            f.write(f"    Score: {score_str}\n")
                            f.write(f"    Tuned pragmas: [{tuned_str}]\n")
                            f.write(f"    Next focus: {next_focus_str}\n")
                            f.write(f"    Bottleneck module: {node.get('bottleneck_module', 'N/A')}\n")
                            
                            # Show configuration vector for this node
                            if 'cfg' in node:
                                cfg_str = ", ".join([
                                    f"[{i}]({pragma_locations[i]['pragma_type']}@{pragma_locations[i]['file']}:{pragma_locations[i]['line']}, {pragma_locations[i].get('region', 'unknown')})={val}"
                                    if i < len(pragma_locations) else f"[{i}]={val}"
                                    for i, val in enumerate(node['cfg'])
                                ])
                                f.write(f"    Configuration: [{cfg_str}]\n")
                            f.write("\n")
                    f.write("\n")
            else:
                f.write("(LevelHeap is empty)\n\n")
            
            # Write current node being explored
            f.write("Current Node:\n")
            if current_node:
                # Format tuned pragmas with detailed info
                tuned_details = []
                for idx, val in current_node.get('tuned', []):
                    if idx < len(pragma_locations):
                        pragma_info = pragma_locations[idx]
                        region = pragma_info.get('region', 'unknown')
                        pragma_detail = f"{idx}({pragma_info['pragma_type']}@{pragma_info['file']}:{pragma_info['line']}, {region})={val}"
                        tuned_details.append(pragma_detail)
                    else:
                        tuned_details.append(f"{idx}={val}")
                tuned_str = ", ".join(tuned_details)
                
                # Format next focus with detailed info
                next_focus = current_node.get('next_focus', None)
                if next_focus is not None and next_focus < len(pragma_locations):
                    pragma_info = pragma_locations[next_focus]
                    region = pragma_info.get('region', 'unknown')
                    next_focus_str = f"{next_focus}({pragma_info['pragma_type']}@{pragma_info['file']}:{pragma_info['line']}, {region})"
                else:
                    next_focus_str = str(next_focus) if next_focus is not None else 'None'
                
                f.write(f"  Score: {self._format_score(current_node.get('score', 'N/A'))}\n")
                f.write(f"  Tuned pragmas: [{tuned_str}]\n")
                f.write(f"  Next focus: {next_focus_str}\n")
                f.write(f"  Bottleneck module: {current_node.get('bottleneck_module', 'N/A')}\n")
            else:
                f.write("  (none)\n")
            
            # Write module hierarchy table
            if csynth_file.exists() and rtl_file.exists():
                try:
                    with open(csynth_file) as cf:
                        csynth_data = json.load(cf)
                    with open(rtl_file) as rf:
                        rtl_data = json.load(rf)
                    
                    self._write_module_hierarchy_table(f, csynth_data, rtl_data)
                    
                    # Get bottleneck module info
                    modules_performance = csynth_data.get("metrics", {})
                    c_mappings = rtl_data.get("modules", {})
                    
                    if bottleneck_module in modules_performance:
                        perf = modules_performance[bottleneck_module]
                        latency = perf.get("Worst-caseLatency", "N/A")
                        ii = perf.get("PipelineInitiationInterval", "N/A")
                        f.write(f"Bottleneck module: {bottleneck_module} (Latency={latency}, II={ii})\n")
                        
                        # Get C source info
                        c_info = c_mappings.get(bottleneck_module, {}).get("c_source", {})
                        mapping = c_info.get("mapping_type", "?")
                        if c_info.get("c_loops") and isinstance(c_info["c_loops"], list) and len(c_info["c_loops"]) > 0:
                            c_type, c_name = "loops", ", ".join(c_info["c_loops"])
                        elif c_info.get("c_loop"):
                            c_type, c_name = "loop", c_info["c_loop"]
                        elif c_info.get("c_function"):
                            c_type, c_name = "function", c_info["c_function"]
                        else:
                            c_type = c_info.get("c_type", "unknown")
                            c_name = c_info.get("c_name", c_info.get("llvm_module", "unknown"))
                        cline = c_info.get("c_line")
                        extra = f", line {cline}" if cline else ""
                        f.write(f"Bottleneck module → C {c_type}: {c_name} (mapping={mapping}{extra})\n")
                    else:
                        f.write(f"Bottleneck module: {bottleneck_module} (metrics not available)\n")
                        f.write("function unknown\n")
                    
                except Exception as e:
                    f.write("Error reading synthesis results\n")
                    f.write(f"Bottleneck module: {bottleneck_module} (error: {e})\n")
                    f.write("function unknown\n")
            else:
                f.write("Module hierarchy not available (synthesis failed)\n")
                f.write(f"Bottleneck module: {bottleneck_module}\n")
                f.write("function unknown\n")
            
            # Write region of interests pragmas
            f.write("Pragmas (Points of interests):\n")
            if affecting_pragmas:
                for pragma in affecting_pragmas:
                    f.write(f"  {pragma['pragma_type']} in {pragma['file']}:{pragma['line']} -> '{pragma['current_choice']}'\n")
            else:
                f.write("  (none found)\n")
            
            # Write selected changed pragma
            f.write("Selected changed pragma:\n")
            if changed_pragma:
                f.write(f"  {changed_pragma['pragma_type']} in {changed_pragma['file']}:{changed_pragma['line']} -> '{changed_pragma['new_choice']}'\n")
            else:
                f.write("  (none)\n")
            
            # Write ordered pragma details
            if current_node and 'ordered_pragmas' in current_node:
                f.write("Ordered pragmas for current bottleneck:\n")
                for i, pragma_dict in enumerate(current_node['ordered_pragmas']):
                    pragma_idx = pragma_dict['index']
                    pragma_type = pragma_dict['pragma_type']
                    current_choice = current_choices[pragma_idx] if pragma_idx < len(current_choices) else "N/A"
                    tuned_mark = " (TUNED)" if any(idx == pragma_idx for idx, _ in current_node.get('tuned', [])) else ""
                    if pragma_idx < len(pragma_locations):
                        pragma_info = pragma_locations[pragma_idx]
                        region = pragma_info.get('region', 'unknown')
                        location = f"@{pragma_info['file']}:{pragma_info['line']}, {region}"
                    else:
                        location = ""
                    f.write(f"  {i+1}. [{pragma_idx}] {pragma_type}{location} -> '{current_choice}'{tuned_mark}\n")
            
            f.write("\n")
    
    def _initialize_levelheap_trace(self, work_dir: str):
        """Initialize LevelHeap DSE trace file with input C hierarchy."""
        if self.levelheap_trace_initialized:
            return
            
        # Create levelheap_dse_trace.log in the same directory as the main log
        trace_dir = Path(self.log_file_path).parent
        self.levelheap_trace_file = trace_dir / "levelheap_dse_trace.log"
        
        # Also initialize final_trace.json in the same directory
        self._initialize_final_trace_json(trace_dir)
        
        try:
            with open(self.levelheap_trace_file, 'w') as f:
                f.write("===== INPUT C HIERARCHY =====\n")
                
                # Try to find and write input_c_hierarchy.txt
                logs_dir = Path(work_dir) / "logs"
                input_c_file = logs_dir / "input_c_hierarchy.txt"
                
                if input_c_file.exists():
                    with open(input_c_file, 'r') as input_f:
                        f.write(input_f.read())
                else:
                    f.write("(Input C hierarchy not available yet - will be written after first synthesis)\n")
                f.write("\n")
                
                f.write("=" * 50 + "\n")
                f.write("LevelHeap DFS Exploration Trace\n")
                f.write("=" * 50 + "\n")
                f.write(f"Generated at: {datetime.now()}\n")
                f.write(f"Work directory: {work_dir}\n\n")
            
            self.levelheap_trace_initialized = True
            self.print(f"[TeeLogger] LevelHeap trace initialized: {self.levelheap_trace_file}")
            
        except Exception as e:
            self.print(f"[TeeLogger] Failed to initialize LevelHeap trace: {e}")
            self.levelheap_trace_file = None
    
    def _initialize_final_trace_json(self, trace_dir: Path):
        """Initialize final_trace.json file in the same directory as levelheap_dse_trace.log."""
        if self.final_trace_json_initialized:
            return
            
        self.final_trace_json_file = trace_dir / "final_trace.json"
        
        try:
            # Initialize with empty JSON object
            with open(self.final_trace_json_file, 'w') as f:
                json.dump({}, f, indent=2)
            
            self.final_trace_json_initialized = True
            self.print(f"[TeeLogger] Final trace JSON initialized: {self.final_trace_json_file}")
            
        except Exception as e:
            self.print(f"[TeeLogger] Failed to initialize final trace JSON: {e}")
            self.final_trace_json_file = None
    
    def update_levelheap_trace_input_hierarchy(self, work_dir: str):
        """Update LevelHeap trace file with actual input C hierarchy after first synthesis."""
        if not self.levelheap_trace_file:
            return
            
        logs_dir = Path(work_dir) / "logs"
        input_c_file = logs_dir / "input_c_hierarchy.txt"
        
        if input_c_file.exists():
            # Read the current trace file
            with open(self.levelheap_trace_file, 'r') as f:
                content = f.read()
            
            # Replace the placeholder input hierarchy
            if "(Input C hierarchy not available yet" in content:
                with open(input_c_file, 'r') as input_f:
                    hierarchy_content = input_f.read()
                
                # Replace placeholder with actual content
                content = content.replace(
                    "(Input C hierarchy not available yet - will be written after first synthesis)\n",
                    hierarchy_content
                )
                
                # Write back the updated content
                with open(self.levelheap_trace_file, 'w') as f:
                    f.write(content)
                
                self.print(f"[TeeLogger] Updated LevelHeap trace with input C hierarchy")
    
    def _initialize_dse_trace(self, work_dir: str):
        """Initialize DSE trace file and write input C hierarchy."""
        if self.dse_trace_initialized:
            return
            
        # Create dse_trace.log in the same directory as the main log
        trace_file_path = self.log_file_path.parent / "dse_trace.log"
        self.dse_trace_file = str(trace_file_path)
        
        # Start the trace file
        with open(self.dse_trace_file, 'w') as f:
            f.write("===== INPUT C HIERARCHY =====\n")
            
            # Try to find and write input_c_hierarchy.txt
            logs_dir = Path(work_dir) / "logs"
            input_c_file = logs_dir / "input_c_hierarchy.txt"
            
            if input_c_file.exists():
                with open(input_c_file, 'r') as input_f:
                    f.write(input_f.read())
            else:
                f.write("(Input C hierarchy not available yet - will be written after first synthesis)\n")
            f.write("\n")
        
        self.dse_trace_initialized = True
    
    def update_dse_trace_input_hierarchy(self, work_dir: str):
        """Update DSE trace file with actual input C hierarchy after first synthesis."""
        if not self.dse_trace_file:
            return
            
        logs_dir = Path(work_dir) / "logs"
        input_c_file = logs_dir / "input_c_hierarchy.txt"
        
        if input_c_file.exists():
            # Read the current trace file
            with open(self.dse_trace_file, 'r') as f:
                content = f.read()
            
            # Replace the placeholder input hierarchy
            if "(Input C hierarchy not available yet" in content:
                with open(input_c_file, 'r') as input_f:
                    hierarchy_content = input_f.read()
                
                # Replace placeholder with actual content
                content = content.replace(
                    "(Input C hierarchy not available yet - will be written after first synthesis)\n",
                    hierarchy_content
                )
                
                # Write back to trace file
                with open(self.dse_trace_file, 'w') as f:
                    f.write(content)
    
    def _write_module_hierarchy_table(self, f, csynth_data: Dict, rtl_data: Dict):
        """Write ASCII table of module hierarchy with dynamic column widths."""
        # Get hierarchy and modules data
        if not ("rtl_hierarchy" in rtl_data and "metrics" in csynth_data):
            f.write("Module hierarchy not available\n")
            return
            
        hierarchy_tree = rtl_data["rtl_hierarchy"]
        modules_performance = csynth_data.get("metrics", {})
        c_mappings = rtl_data.get("modules", {})
        
        # First pass: collect all data to determine column widths
        table_data = []
        headers = ["Name", "Latency", "II", "BRAM", "DSP", "FF", "LUT", "URAM", "C type", "C name"]
        self._collect_hierarchy_data(hierarchy_tree, modules_performance, c_mappings, 0, table_data)
        
        # Calculate column widths
        col_widths = [len(header) for header in headers]  # Start with header lengths
        
        # Check all data rows
        for row in table_data:
            for i, cell in enumerate(row):
                col_widths[i] = max(col_widths[i], len(str(cell)))
        
        # Add padding
        col_widths = [w + 2 for w in col_widths]  # Add 2 for padding
        
        # Write table
        self._write_table_border(f, col_widths)
        self._write_table_row(f, headers, col_widths, is_header=True)
        self._write_table_border(f, col_widths)
        
        for row in table_data:
            self._write_table_row(f, row, col_widths)
        
        self._write_table_border(f, col_widths)
    
    def _collect_hierarchy_data(self, node: Dict, modules_performance: Dict, c_mappings: Dict, depth: int, table_data: List):
        """Recursively collect all hierarchy data for width calculation."""
        module_name = node.get("ModuleName", "")
        
        # Get performance metrics
        perf = modules_performance.get(module_name, {})
        latency = perf.get("Worst-caseLatency", "N/A")
        ii = perf.get("PipelineInitiationInterval", "N/A")
        bram = perf.get("BRAM_18K", "N/A")
        dsp = perf.get("DSP", "N/A")
        ff = perf.get("FF", "N/A")
        lut = perf.get("LUT", "N/A")
        uram = perf.get("URAM", "N/A")
        
        # Get C mapping
        c_info = c_mappings.get(module_name, {}).get("c_source", {})
        if c_info.get("c_loops") and isinstance(c_info["c_loops"], list) and len(c_info["c_loops"]) > 0:
            c_type = "loops"
            c_name = ", ".join(c_info["c_loops"])
        elif c_info.get("c_loop"):
            c_type = "loop"
            c_name = c_info["c_loop"]
        elif c_info.get("c_function"):
            c_type = "function"
            c_name = c_info["c_function"]
        else:
            c_type = c_info.get("c_type", "N/A")
            c_name = c_info.get("c_name", "N/A")
        
        # Create indented name
        indent = " " * depth
        indented_name = f"{indent}{module_name}"
        
        # Add row data
        row = [
            indented_name,
            str(latency),
            str(ii),
            str(bram),
            str(dsp),
            str(ff),
            str(lut),
            str(uram),
            str(c_type),
            str(c_name)
        ]
        table_data.append(row)
        
        # Process children
        for child in node.get("Children", []):
            self._collect_hierarchy_data(child, modules_performance, c_mappings, depth + 1, table_data)
    
    def _write_table_border(self, f, col_widths: List[int]):
        """Write table border with dynamic widths."""
        border = "+"
        for width in col_widths:
            border += "-" * width + "+"
        f.write(border + "\n")
    
    def _write_table_row(self, f, row_data: List[str], col_widths: List[int], is_header: bool = False):
        """Write table row with proper alignment and dynamic widths."""
        row = "|"
        for i, (cell, width) in enumerate(zip(row_data, col_widths)):
            cell_str = str(cell)
            
            # Alignment rules
            if is_header or i == 0:  # Header or Name column (left-aligned)
                formatted_cell = f" {cell_str:<{width-2}} "
            elif i in [1, 2, 3, 4, 5, 6, 7]:  # Numeric columns (right-aligned)
                formatted_cell = f" {cell_str:>{width-2}} "
            else:  # Text columns (left-aligned)
                formatted_cell = f" {cell_str:<{width-2}} "
            
            row += formatted_cell + "|"
        
        f.write(row + "\n")
    
    def save_iteration_trace(self, iteration: int, pragma_locations: List[Dict], 
                           current_choices: List[str], synthesis_status: str,
                           obj_result: Any, obj_details: Dict, max_resources: Dict = None):
        """
        Save iteration data for final trace JSON generation.
        
        Args:
            iteration: Current iteration number
            pragma_locations: All pragma location information  
            current_choices: Current pragma choices
            synthesis_status: Status of synthesis ("success", "timeout", "hls_failed")
            obj_result: Objective evaluation result
            obj_details: Details from objective evaluation including metrics
            max_resources: Resource constraints for calculating utilization ratios
        """
        if not self.final_trace_json_file:
            return
            
        # Build pragma configuration key
        pragma_key_parts = []
        point_dict = {}
        
        # Group pragmas by type to create the key format
        pragma_groups = {
            "__PARA__": [],
            "__PIPE__": [],
            "__TILE__": []
        }
        
        # Process each pragma and group by type
        for i, choice in enumerate(current_choices):
            if i < len(pragma_locations):
                pragma_info = pragma_locations[i]
                pragma_type = pragma_info.get('pragma_type', '')
                region = pragma_info.get('region', '')
                
                # Map pragma types to groups
                if 'unroll' in pragma_type.lower():
                    group_key = "__PARA__"
                elif 'pipeline' in pragma_type.lower():
                    group_key = "__PIPE__"
                elif 'tile' in pragma_type.lower():
                    group_key = "__TILE__"
                else:
                    continue  # Skip unknown pragma types
                    
                # Extract level from region (e.g., "L1" -> "L1")
                level_id = region if region.startswith('L') else f"L{len(pragma_groups[group_key])}"
                
                pragma_groups[group_key].append({
                    'level': level_id,
                    'choice': choice if choice else 'NA',
                    'index': len(pragma_groups[group_key])
                })
                
                # Add to point dictionary
                point_key = f"{group_key}{level_id}"
                if group_key == "__PIPE__":
                    point_dict[point_key] = choice if choice else ""
                else:
                    # For unroll/tile pragmas: preserve actual values
                    if choice and choice.isdigit():
                        point_dict[point_key] = int(choice)
                    elif choice:
                        point_dict[point_key] = choice  # Keep non-numeric choices as string
                    else:
                        point_dict[point_key] = ""  # Empty means no unrolling
        
        # Build the configuration key string
        for group_name in ["__PARA__", "__PIPE__", "__TILE__"]:
            for pragma in pragma_groups[group_name]:
                key_part = f"{group_name}{pragma['level']}-{pragma['choice']}"
                pragma_key_parts.append(key_part)
        
        config_key = ".".join(pragma_key_parts)
        
        # Extract performance and resource data
        perf_value = 0.0
        valid = False
        
        # Resource utilization data
        res_util = {
            "util-URAM": 0,
            "util-BRAM": 0,
            "util-DSP": 0,
            "util-LUT": 0,
            "util-FF": 0,
            "total-URAM": 0,
            "total-BRAM": 0,
            "total-DSP": 0,
            "total-LUT": 0,
            "total-FF": 0
        }
        
        if synthesis_status == "success" and obj_details:
            # Extract latency as performance metric
            perf_value = float(obj_details.get('latency', 0.0))
            
            # Extract resource usage
            res_util["total-URAM"] = obj_details.get('uram_used', 0)
            res_util["total-BRAM"] = obj_details.get('bram_used', 0)
            res_util["total-DSP"] = obj_details.get('dsp_used', 0)
            res_util["total-LUT"] = obj_details.get('lut_used', 0)
            res_util["total-FF"] = obj_details.get('ff_used', 0)
            
            # Calculate utilization ratios if resource constraints are provided
            if max_resources:
                for resource in ['URAM', 'BRAM', 'DSP', 'LUT', 'FF']:
                    total_key = f"total-{resource}"
                    util_key = f"util-{resource}"
                    max_key = f"max_{resource.lower()}"
                    
                    if max_key in max_resources and max_resources[max_key] > 0:
                        ratio = res_util[total_key] / max_resources[max_key]
                        res_util[util_key] = min(1.0, ratio)  # Cap at 1.0
            
            # Check if design is valid (synthesis succeeded and within resource limits)
            valid = (synthesis_status == "success" and 
                    not obj_details.get('resource_violations', []))
        
        # Create iteration data
        iteration_data = {
            "perf": perf_value,
            "point": point_dict,
            "res_util": res_util,
            "valid": valid
        }
        
        # Read existing data, add new iteration, and write back
        try:
            # Read current data
            with open(self.final_trace_json_file, 'r') as f:
                trace_data = json.load(f)
            
            # Add new iteration
            trace_data[config_key] = iteration_data
            
            # Write updated data back
            with open(self.final_trace_json_file, 'w') as f:
                json.dump(trace_data, f, indent=2)
                
        except Exception as e:
            self.print(f"[TeeLogger] Failed to update final trace JSON: {e}")
    
