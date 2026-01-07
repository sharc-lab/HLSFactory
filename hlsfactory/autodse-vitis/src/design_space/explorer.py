#!/usr/bin/env python3
"""
DSE Explorer for Design Space Exploration.
Implements search algorithms to find optimal pragma configurations.
"""

import json
import pickle
import random
from pathlib import Path
from typing import Dict, List, Optional, Tuple
from datetime import datetime
import sys
import os

# Add the parent directory to sys.path to import concrete_design
sys.path.append(str(Path(__file__).parent.parent))
from concrete_design.design import ConcreteDesign
from design_space.container import TemplateContainer
from design_space.logger import TeeLogger
from objective.objectives import ObjectiveFunction, TargetIIObjective


class DSEExplorer:
    """
    Design Space Explorer implementing simple worst-module search strategy.
    """
    
    def __init__(self, container: TemplateContainer, objective_fn: Optional[ObjectiveFunction] = None, target_ii: int = 1):
        """
        Initialize DSE Explorer.
        
        Args:
            container: Template container for generating variants
            objective_fn: Objective function to guide exploration (if None, uses TargetIIObjective)
            target_ii: Target initiation interval for top module (used if objective_fn is None)
        """
        self.container = container
        
        # Set up objective function
        if objective_fn is not None:
            self.objective_fn = objective_fn
        else:
            # Backward compatibility: create TargetIIObjective with provided target_ii
            self.objective_fn = TargetIIObjective(target_ii)
        
        # Keep target_ii for backward compatibility and logging
        self.target_ii = target_ii
        self.template_info = container.load_template()
        self.pragma_locations = self.template_info["pragma_locations"]
        
        # Current state
        self.current_choices = container.get_initial_choices()
        self.history = []  # List of exploration iterations
        self.iteration = 0
        
        # Logger will be set externally
        self.logger = None
    
    def set_logger(self, logger: TeeLogger):
        """Set the logger for this explorer."""
        self.logger = logger
        self.container.set_logger(logger)  # Also set for container
        self.objective_fn.set_logger(logger)  # Also set for objective function
    
    def find_affecting_pragmas(self, bottleneck_module: str, c_source_mappings: Dict) -> List[int]:
        """
        Find pragma locations that could affect the bottleneck module.
        
        Args:
            bottleneck_module: Name of the bottleneck module
            c_source_mappings: C source mappings from rtl_c_annotated.json
            
        Returns:
            List of pragma location indices that could affect this module
        """
        if self.logger:
            self.logger.print(f"[DSEExplorer] Finding pragmas affecting module: {bottleneck_module}")
        else:
            print(f"[DSEExplorer] Finding pragmas affecting module: {bottleneck_module}")
        
        # Find C source information for this module
        module_c_info = None
        if "modules" in c_source_mappings:
            module_data = c_source_mappings["modules"].get(bottleneck_module, {})
            module_c_info = module_data.get("c_source", {})
        
        if not module_c_info:
            if self.logger:
                self.logger.print(f"[DSEExplorer] No C source mapping found for {bottleneck_module}, using random selection")
            else:
                print(f"[DSEExplorer] No C source mapping found for {bottleneck_module}, using random selection")
            return self._sample_random_pragma_indices()
        
        # Extract C function and line information
        c_function = module_c_info.get("c_function", "")
        c_line = module_c_info.get("c_line", 0)
        # Handle both regular and flattened cases (flattened uses "start_lines"/"end_lines")
        start_line = module_c_info.get("start_line") or module_c_info.get("start_lines", c_line)
        end_line = module_c_info.get("end_line") or module_c_info.get("end_lines", c_line)
        c_file = module_c_info.get("c_file", "")  # Get file info from C mapping
        
        if self.logger:
            self.logger.print(f"[DSEExplorer] Module {bottleneck_module} maps to function '{c_function}' at line {c_line} (range: {start_line}-{end_line}) in file '{c_file}'")
        else:
            print(f"[DSEExplorer] Module {bottleneck_module} maps to function '{c_function}' at line {c_line} (range: {start_line}-{end_line}) in file '{c_file}'")
        
        # Debug: Show what we're looking for
        # if self.logger:
        #     self.logger.print(f"[DSEExplorer] Searching for pragmas in file '{c_file}' within lines {start_line}-{end_line}")
        #     self.logger.print(f"[DSEExplorer] Total pragma locations available: {len(self.pragma_locations)}")
        # else:
        #     print(f"[DSEExplorer] Searching for pragmas in file '{c_file}' within lines {start_line}-{end_line}")
        #     print(f"[DSEExplorer] Total pragma locations available: {len(self.pragma_locations)}")
        
        # Find pragma locations that fall within the start_line and end_line range
        affecting_indices = []
        # If line numbers are not available, match all pragmas in the file
        match_all_in_file = (start_line is None or end_line is None) and c_file

        for i, pragma_info in enumerate(self.pragma_locations):
            pragma_file = pragma_info["file"]
            pragma_line = pragma_info["line"]

            # Simple file matching: compare base filenames
            if c_file:
                import os
                pragma_basename = os.path.basename(pragma_file)
                c_basename = os.path.basename(c_file)
                file_matches = (pragma_basename == c_basename)
            else:
                file_matches = True  # No file restriction if mapping doesn't have file info

            if file_matches:
                if match_all_in_file:
                    affecting_indices.append(i)
                    if self.logger:
                        self.logger.print(f"[DSEExplorer] ✓ Found affecting pragma #{i} (file match): {pragma_info['pragma_type']} at {pragma_file}:{pragma_line}")
                elif start_line is not None and end_line is not None:
                    if start_line <= pragma_line <= end_line:
                        affecting_indices.append(i)
                        if self.logger:
                            self.logger.print(f"[DSEExplorer] ✓ Found affecting pragma #{i} (line match): {pragma_info['pragma_type']} at {pragma_file}:{pragma_line}")
                    
        if not affecting_indices:
            if self.logger:
                self.logger.print(f"[DSEExplorer] No specific pragmas found, falling back to random selection")
            else:
                print(f"[DSEExplorer] No specific pragmas found, falling back to random selection")
            return self._sample_random_pragma_indices()
        
        if self.logger:
            self.logger.print(f"[DSEExplorer] Found {len(affecting_indices)} potentially affecting pragmas")
        else:
            print(f"[DSEExplorer] Found {len(affecting_indices)} potentially affecting pragmas")
        return affecting_indices
    
    def _sample_random_pragma_indices(self) -> List[int]:
        """
        Sample random pragma indices as fallback.
        
        Returns:
            List of random pragma indices
        """
        num_pragmas = len(self.pragma_locations)
        sample_size = min(3, num_pragmas)  # Sample up to 3 pragmas
        return random.sample(range(num_pragmas), sample_size)
    
    def order_pragma_lists(self, affecting_indices: List[int]) -> List[Dict]:
        """
        Order pragma lists for DFS exploration using heuristics.
        Priority order: dataflow -> inline -> pipeline -> array_partition -> other pragmas
        
        Args:
            affecting_indices: List of pragma indices that affect the bottleneck
            
        Returns:
            List of pragma information dictionaries with options, ordered by priority
        """
        pragma_lists = []
        for idx in affecting_indices:
            if idx < len(self.pragma_locations):
                pragma_info = self.pragma_locations[idx]
                pragma_lists.append({
                    "index": idx,
                    "pragma_type": pragma_info["pragma_type"],
                    "options": pragma_info["options"]
                })
        
        # Define pragma priority order
        priority_order = {
            "dataflow": 1,
            "performance": 2,
            "unroll": 3,
            "array_partition": 4,
            "inline": 5,
            "pipeline": 6,
            "bind_storage": 7,
            "bind_op": 8,
            "stream": 9,
            # Default priority for unknown pragmas
        }
        
        def get_pragma_priority(pragma_dict):
            pragma_type = pragma_dict["pragma_type"].lower()
            # Extract base pragma type (remove prefixes like HLS_)
            if pragma_type.startswith("hls_"):
                pragma_type = pragma_type[4:]
            
            return priority_order.get(pragma_type, 999)  # 999 for unknown pragmas
        
        # Sort pragmas by priority
        pragma_lists.sort(key=get_pragma_priority)
        
        if self.logger:
            self.logger.print(f"[DSEExplorer] Ordered pragmas by priority: {[p['pragma_type'] for p in pragma_lists]}")
        else:
            print(f"[DSEExplorer] Ordered pragmas by priority: {[p['pragma_type'] for p in pragma_lists]}")
        
        return pragma_lists
    

    def order_pragma_lists_hierarchical(self, affecting_indices: List[int], csynth_data: Dict, rtl_data: Dict, work_dir: str, hierarchy_file_path: str = None) -> List[Dict]:
        """
        Order pragma lists using hierarchical latency-based ranking with proper loop hierarchy.
        
        Uses the actual loop hierarchy from input_c_hierarchy.json to correctly order
        sub-loops (L6, L8) within their parent loop context, not at the end.
        
        Args:
            affecting_indices: List of pragma indices that affect the bottleneck
            csynth_data: Synthesis data containing module latencies
            rtl_data: RTL data containing module mappings
            work_dir: The current working directory for this iteration.
            hierarchy_file_path: Optional path to hierarchy file
            
        Returns:
            List of pragma information dictionaries ordered by hierarchical priority
        """
        if self.logger:
            self.logger.print(f"[DSEExplorer] Using hierarchical pragma ordering with loop hierarchy")
        
        # Step 1: Load loop hierarchy from logs directory
        hierarchy_data = self._load_loop_hierarchy(work_dir, hierarchy_file_path)
        
        # Step 2: Map pragmas to their loop/module contexts
        loop_pragma_mapping = {}  # loop_id -> list of pragma indices
        
        for idx in affecting_indices:
            if idx < len(self.pragma_locations):
                pragma_info = self.pragma_locations[idx]
                region = pragma_info.get("region", "")
                
                # Extract loop identifier (L0, L1, L2, etc.)
                if region and region.startswith("L"):
                    loop_id = region
                    if loop_id not in loop_pragma_mapping:
                        loop_pragma_mapping[loop_id] = []
                    loop_pragma_mapping[loop_id].append(idx)
                else:
                    # If no region, associate with top module
                    if "top_module" not in loop_pragma_mapping:
                        loop_pragma_mapping["top_module"] = []
                    loop_pragma_mapping["top_module"].append(idx)
        
        if self.logger:
            self.logger.print(f"[DSEExplorer] Loop pragma mapping: {loop_pragma_mapping}")
        
        # Step 3: Get module latency ordering for prioritization
        modules_performance = csynth_data.get("metrics", {})
        module_latencies = {}
        
        for module_name, perf in modules_performance.items():
            latency = perf.get("Worst-caseLatency", 0)
            # Convert to int if it's a string
            if isinstance(latency, str):
                try:
                    latency = int(latency)
                except ValueError:
                    latency = 0
            module_latencies[module_name] = latency
        
        # Map modules to their loop hierarchies using C source mappings (not pattern matching)
        loop_modules = []

        # Helper to get loops from C source mapping
        def get_module_loops(module_name):
            if "modules" in rtl_data and module_name in rtl_data["modules"]:
                c_src = rtl_data["modules"][module_name].get("c_source", {})
                if c_src:
                    if c_src.get("c_loops"):
                        return c_src["c_loops"]
                    elif c_src.get("c_loop"):
                        return [c_src["c_loop"]]
            return None

        for module_name, latency in module_latencies.items():
            loops = get_module_loops(module_name)
            if loops:
                loop_modules.append((module_name, latency, loops))

        # Sort by RTL hierarchy if available, otherwise by flat latency
        if "rtl_hierarchy" in rtl_data:
            # Collect modules in hierarchical order (depth-first, worst latency first at each level)
            hierarchical_order = []

            def traverse_hierarchy(node, depth=0):
                mod_name = node.get("ModuleName", "")
                loops = get_module_loops(mod_name)
                lat = module_latencies.get(mod_name, 0)
                if loops:
                    hierarchical_order.append((mod_name, lat, loops))

                # Sort children by latency before recursing
                children = node.get("Children", [])
                children_with_lat = [(c, module_latencies.get(c.get("ModuleName", ""), 0)) for c in children]
                children_with_lat.sort(key=lambda x: x[1], reverse=True)

                for child, _ in children_with_lat:
                    traverse_hierarchy(child, depth + 1)

            traverse_hierarchy(rtl_data["rtl_hierarchy"])
            loop_modules = hierarchical_order
        else:
            # Fallback: sort by latency
            loop_modules.sort(key=lambda x: x[1], reverse=True)
        
        if self.logger:
            self.logger.print(f"[DSEExplorer] Module latency ranking (highest first):")
            for module_name, latency, loop_indices in loop_modules:
                self.logger.print(f"  {module_name}: {latency} (loops: {loop_indices})")
        
        # Step 4: Load loop hierarchy and create hierarchical ordering
        hierarchy_data = self._load_loop_hierarchy(work_dir, hierarchy_file_path)
        ordered_pragma_indices = []
        processed_loops = set()
        
        # Process modules in hierarchical order
        for module_name, latency, loops in loop_modules:
            if self.logger:
                loop_str = ", ".join(loops) if isinstance(loops, list) else str(loops)
                self.logger.print(f"  Processing {module_name} (latency: {latency}) with loops: {loop_str}")

            # Process loops innermost first
            for loop_id in reversed(loops):
                if loop_id in loop_pragma_mapping and loop_id not in processed_loops:
                    self._add_children_first_then_parent(loop_id, loop_pragma_mapping,
                                                        processed_loops, ordered_pragma_indices,
                                                        hierarchy_data, module_name)
        
        # Add any remaining loops not covered by modules (should be fewer now)
        remaining_loops = []
        for loop_id in loop_pragma_mapping:
            if loop_id.startswith("L") and loop_id not in processed_loops:
                remaining_loops.append(loop_id)
        
        # Sort remaining loops by numeric order
        remaining_loops.sort(key=lambda x: int(x[1:]) if x[1:].isdigit() else 999)
        
        for loop_id in remaining_loops:
            ordered_pragma_indices.extend(loop_pragma_mapping[loop_id])
            if self.logger:
                self.logger.print(f"    Added pragmas for {loop_id} (remaining loop)")
        
        # Add any remaining top module pragmas
        if "top_module" in loop_pragma_mapping:
            ordered_pragma_indices.extend(loop_pragma_mapping["top_module"])
            if self.logger:
                self.logger.print(f"    Added {len(loop_pragma_mapping['top_module'])} top module pragmas")
        
        # Step 5: Within each group, apply the existing pragma priority ordering
        final_ordered_pragmas = []
        
        # Remove duplicates while preserving order
        seen_indices = set()
        unique_ordered_indices = []
        for idx in ordered_pragma_indices:
            if idx not in seen_indices:
                unique_ordered_indices.append(idx)
                seen_indices.add(idx)
        
        # Group by pragma type priority and sort within each group
        priority_order = {
            "dataflow": 1, "performance": 2, "unroll": 3, "array_partition": 4,
            "inline": 5, "pipeline": 6, "bind_storage": 7, "bind_op": 8, "stream": 9,
        }
        
        def get_pragma_priority(pragma_type):
            pragma_type_lower = pragma_type.lower()
            if pragma_type_lower.startswith("hls_"):
                pragma_type_lower = pragma_type_lower[4:]
            return priority_order.get(pragma_type_lower, 999)
        
        # Group pragmas by their loop context while maintaining hierarchical order
        # This preserves the loop-based grouping from steps 1-4
        loop_grouped_pragmas = {}  # loop_id -> list of pragma indices
        
        for idx in unique_ordered_indices:
            if idx < len(self.pragma_locations):
                pragma_info = self.pragma_locations[idx]
                region = pragma_info.get("region", "top_module")
                
                if region not in loop_grouped_pragmas:
                    loop_grouped_pragmas[region] = []
                loop_grouped_pragmas[region].append(idx)
        
        # Process loops in the order they were added (preserving hierarchical order)
        processed_regions = []
        for idx in unique_ordered_indices:
            if idx < len(self.pragma_locations):
                pragma_info = self.pragma_locations[idx]
                region = pragma_info.get("region", "top_module")
                if region not in processed_regions:
                    processed_regions.append(region)
        
        # For each loop group, sort by pragma type priority, then add to final list
        for region in processed_regions:
            if region in loop_grouped_pragmas:
                region_pragmas = []
                for idx in loop_grouped_pragmas[region]:
                    if idx < len(self.pragma_locations):
                        pragma_info = self.pragma_locations[idx]
                        region_pragmas.append({
                            "index": idx,
                            "pragma_type": pragma_info["pragma_type"],
                            "options": pragma_info["options"],
                            "region": region,
                            "priority": get_pragma_priority(pragma_info["pragma_type"])
                        })
                
                # Sort within this region by pragma type priority
                region_pragmas.sort(key=lambda x: x["priority"])
                final_ordered_pragmas.extend(region_pragmas)
        
        if self.logger:
            self.logger.print(f"[DSEExplorer] Hierarchical pragma order (final):")
            for i, pragma in enumerate(final_ordered_pragmas):
                loop_info = ""
                if pragma["index"] < len(self.pragma_locations):
                    region = self.pragma_locations[pragma["index"]].get("region", "")
                    if region:
                        loop_info = f" ({region})"
                self.logger.print(f"  {i+1}. [{pragma['index']}] {pragma['pragma_type']}{loop_info}")
        
        return final_ordered_pragmas
    
    def _add_children_first_then_parent(self, parent_loop: str, loop_pragma_mapping: Dict, 
                                       processed_loops: set, ordered_pragma_indices: List[int], 
                                       hierarchy_data: Dict, module_name: str):
        """
        Add child loops first (innermost first), then parent loop.
        This ensures innermost-first ordering: L8→L5, L6→L3, etc.
        
        Args:
            parent_loop: Parent loop ID (e.g., "L3", "L5") 
            loop_pragma_mapping: Mapping of loop IDs to pragma indices
            processed_loops: Set of already processed loops
            ordered_pragma_indices: List to append pragma indices to
            hierarchy_data: Loop hierarchy data
            module_name: Name of the module being processed (for logging)
        """
        if not hierarchy_data:
            # No hierarchy data - just add parent
            if parent_loop in loop_pragma_mapping and parent_loop not in processed_loops:
                ordered_pragma_indices.extend(loop_pragma_mapping[parent_loop])
                processed_loops.add(parent_loop)
                if self.logger:
                    self.logger.print(f"    Added pragmas for {parent_loop} from {module_name}")
            return
            
        # First, recursively add all children (innermost first)
        children = self._get_direct_children(parent_loop, hierarchy_data)
        
        for child_loop in children:
            if child_loop in loop_pragma_mapping and child_loop not in processed_loops:
                # Recursively add grandchildren first (deepest first)
                self._add_children_first_then_parent(child_loop, loop_pragma_mapping, 
                                                    processed_loops, ordered_pragma_indices, 
                                                    hierarchy_data, module_name)
        
        # Then add parent loop (after all children have been added)
        if parent_loop in loop_pragma_mapping and parent_loop not in processed_loops:
            ordered_pragma_indices.extend(loop_pragma_mapping[parent_loop])
            processed_loops.add(parent_loop)
            if self.logger:
                child_info = f" (parent of {children})" if children else ""
                self.logger.print(f"    Added pragmas for {parent_loop}{child_info} from {module_name}")
    
    def _get_direct_children(self, parent_loop: str, hierarchy_data: Dict) -> List[str]:
        """
        Get direct children loop IDs of a parent loop from hierarchy data.
        
        Args:
            parent_loop: Parent loop ID (e.g., "L3")
            hierarchy_data: Loop hierarchy data
            
        Returns:
            List of direct child loop IDs
        """
        if not hierarchy_data:
            return []
        
        children = []
        self._find_children_recursive(hierarchy_data.get("children", []), parent_loop, children)
        return children
    
    def _find_children_recursive(self, children_nodes: List[Dict], target_loop: str, result: List[str]):
        """
        Recursively find direct children of target loop in hierarchy.
        
        Args:
            children_nodes: List of child nodes from hierarchy
            target_loop: Loop ID to find children for
            result: List to populate with child loop IDs
        """
        for child in children_nodes:
            if child.get("type") == "loop":
                loop_label = child.get("label", "")
                if loop_label == target_loop:
                    # Found target loop, add its direct children
                    for grandchild in child.get("children", []):
                        if grandchild.get("type") == "loop":
                            grandchild_label = grandchild.get("label", "")
                            if grandchild_label:
                                result.append(grandchild_label)
                    return  # Found target, don't search further
                else:
                    # Continue searching in children
                    self._find_children_recursive(child.get("children", []), target_loop, result)

    def _load_loop_hierarchy(self, work_dir: str, hierarchy_file_path: str = None) -> Dict:
        """
        Load loop hierarchy from input_c_hierarchy.json.
        
        Args:
            work_dir: The current working directory for this iteration.
            hierarchy_file_path: Optional direct path to hierarchy file. If provided,
                                 this takes priority over automatic discovery.
        
        Returns:
            Dictionary containing loop hierarchy data, or empty dict if not found
        """
        # If a specific path is provided, try that first
        if hierarchy_file_path:
            try:
                import json
                with open(hierarchy_file_path, 'r') as f:
                    hierarchy_data = json.load(f)
                    if self.logger:
                        self.logger.print(f"[DSEExplorer] Loaded loop hierarchy from {hierarchy_file_path}")
                    return hierarchy_data[0] if isinstance(hierarchy_data, list) and len(hierarchy_data) > 0 else hierarchy_data
            except (FileNotFoundError, json.JSONDecodeError, IndexError) as e:
                if self.logger:
                    self.logger.print(f"[DSEExplorer] Failed to load hierarchy from {hierarchy_file_path}: {e}")

        # Construct the expected path within the current work directory
        hierarchy_file = Path(work_dir) / "logs" / "input_c_hierarchy.json"

        if hierarchy_file.exists():
            try:
                import json
                with open(hierarchy_file, 'r') as f:
                    hierarchy_data = json.load(f)
                    if self.logger:
                        self.logger.print(f"[DSEExplorer] Loaded loop hierarchy from {hierarchy_file}")
                    return hierarchy_data[0] if isinstance(hierarchy_data, list) and len(hierarchy_data) > 0 else hierarchy_data
            except (json.JSONDecodeError, IndexError) as e:
                if self.logger:
                    self.logger.print(f"[DSEExplorer] Failed to parse hierarchy file {hierarchy_file}: {e}")

        # If no hierarchy file found, return empty structure
        if self.logger:
            self.logger.print("[DSEExplorer] No loop hierarchy file found, using fallback ordering")
        return {}
    
    def _order_loops_by_hierarchy(self, loop_parts: List[str], hierarchy_data: Dict) -> List[str]:
        """
        Order loop parts based on their hierarchical relationship.
        
        Args:
            loop_parts: List of loop IDs (e.g., ["L1", "L4", "L7"])
            hierarchy_data: Loop hierarchy data from input_c_hierarchy.json
            
        Returns:
            List of loop IDs ordered from innermost to outermost (deepest first)
        """
        if not hierarchy_data:
            # Fallback: sort by numeric order
            return sorted(loop_parts, key=lambda x: int(x[1:]) if x[1:].isdigit() else 999)
        
        # Build hierarchy map
        loop_to_depth = {}
        self._build_loop_depth_map(hierarchy_data.get("children", []), loop_to_depth, 0)
        
        # Sort by depth (innermost first - deepest loops have higher priority)
        loop_parts_with_depth = [(loop, loop_to_depth.get(loop, 999)) for loop in loop_parts]
        loop_parts_with_depth.sort(key=lambda x: x[1], reverse=True)  # Sort by depth descending (innermost first)
        
        return [loop for loop, depth in loop_parts_with_depth]
    
    def _add_loop_and_children_recursively(self, loop_id: str, loop_pragma_mapping: Dict, 
                                          processed_loops: set, ordered_pragma_indices: List[int], 
                                          hierarchy_data: Dict, module_name: str):
        """
        Add a loop and all its children recursively in hierarchical order (children first).
        
        Args:
            loop_id: Loop ID to process
            loop_pragma_mapping: Mapping of loop IDs to pragma indices
            processed_loops: Set of already processed loops
            ordered_pragma_indices: List to append pragma indices to
            hierarchy_data: Loop hierarchy data
            module_name: Name of the module being processed (for logging)
        """
        # First, recursively add all children (innermost first)
        child_loops = self._get_child_loops(loop_id, hierarchy_data)
        for child_loop in child_loops:
            if child_loop not in processed_loops:
                self._add_loop_and_children_recursively(child_loop, loop_pragma_mapping, 
                                                       processed_loops, ordered_pragma_indices, 
                                                       hierarchy_data, module_name)
        
        # Then add the current loop's pragmas after its children
        if loop_id in loop_pragma_mapping and loop_id not in processed_loops:
            ordered_pragma_indices.extend(loop_pragma_mapping[loop_id])
            processed_loops.add(loop_id)
            if self.logger:
                self.logger.print(f"    Added pragmas for {loop_id} from {module_name}")

    def _get_failure_score(self, result_status: str, objective_result=None) -> str:
        """
        Determine the appropriate failure score based on the synthesis status and objective result.

        Args:
            result_status: Status from ConcreteDesign.run_vitis_hls() or analysis
                          ("success", "timeout", "hls_failed", "unanalyzable", "missing_files")
            objective_result: Result from objective function evaluation

        Returns:
            Appropriate failure message string
        """
        from objective.objectives import ObjectiveResult

        # If synthesis failed, use synthesis-specific failure type
        if result_status == "timeout":
            return "TIME OUT"
        elif result_status == "hls_failed":
            return "FAIL SYNTHESIS"
        elif result_status == "unanalyzable":
            return "UNANALYZABLE"
        elif result_status == "missing_files":
            return "MISSING FILES"
        elif result_status != "success":
            return "FAIL SYNTHESIS"  # Fallback for any other non-success status

        # If synthesis succeeded but objective evaluation shows infeasible
        if objective_result == ObjectiveResult.INFEASIBLE:
            return "EXCEED RESOURCES"

        # This shouldn't happen in normal flow, but provide fallback
        return "FAIL SYNTHESIS"

    def _is_valid_score(self, score) -> bool:
        """
        Check if a score is valid (not an error indicator).

        Args:
            score: Score value to check

        Returns:
            True if score is a valid numeric value, False otherwise
        """
        if isinstance(score, str):
            # All failure types are invalid scores
            return score not in ["EXCEED RESOURCES", "TIME OUT", "FAIL SYNTHESIS", "UNANALYZABLE", "MISSING FILES"]
        return isinstance(score, (int, float)) and score != float('inf')
    
    def _is_better_score(self, new_score, current_best_score) -> bool:
        """
        Compare two scores, handling invalid scores properly.
        
        Args:
            new_score: New score to compare
            current_best_score: Current best score
            
        Returns:
            True if new_score is better than current_best_score
        """
        # If new score is invalid, it's never better
        if not self._is_valid_score(new_score):
            return False
        
        # If current best is invalid but new score is valid, new score is better
        if not self._is_valid_score(current_best_score):
            return True
        
        # Both are valid, compare normally (lower is better)
        return new_score < current_best_score

    def _get_child_loops(self, parent_loop: str, hierarchy_data: Dict) -> List[str]:
        """
        Get all direct child loops of a parent loop.
        
        Args:
            parent_loop: Parent loop ID (e.g., "L3")
            hierarchy_data: Loop hierarchy data from input_c_hierarchy.json
            
        Returns:
            List of direct child loop IDs
        """
        if not hierarchy_data:
            return []
        
        children = []
        self._find_loop_children(hierarchy_data.get("children", []), parent_loop, children)
        return children
    
    def _build_loop_depth_map(self, children: List[Dict], depth_map: Dict[str, int], current_depth: int):
        """
        Recursively build a map of loop ID to its nesting depth.
        
        Args:
            children: List of child loop nodes
            depth_map: Map to populate with loop_id -> depth
            current_depth: Current nesting depth
        """
        for child in children:
            if child.get("type") == "loop":
                loop_label = child.get("label", "")
                if loop_label:
                    depth_map[loop_label] = current_depth
                # Recursively process children
                self._build_loop_depth_map(child.get("children", []), depth_map, current_depth + 1)
    
    def _find_loop_children(self, children: List[Dict], target_loop: str, result: List[str]):
        """
        Recursively find direct children of a target loop.
        
        Args:
            children: List of child loop nodes
            target_loop: Loop ID to find children for
            result: List to populate with child loop IDs
        """
        for child in children:
            if child.get("type") == "loop":
                loop_label = child.get("label", "")
                if loop_label == target_loop:
                    # Found target loop, add its direct children
                    for grandchild in child.get("children", []):
                        if grandchild.get("type") == "loop":
                            grandchild_label = grandchild.get("label", "")
                            if grandchild_label:
                                result.append(grandchild_label)
                    return  # Found target, don't need to search further
                else:
                    # Continue searching in children
                    self._find_loop_children(child.get("children", []), target_loop, result)
    
    def _get_module_c_function(self, module_name: str, rtl_data: Dict) -> Optional[str]:
        """
        Extract C function name from module using RTL data.
        
        Args:
            module_name: Hardware module name
            rtl_data: RTL data containing C source mappings
            
        Returns:
            C function name or None if not found
        """
        if "modules" not in rtl_data:
            return None
            
        module_data = rtl_data["modules"].get(module_name, {})
        module_c_info = module_data.get("c_source", {})
        if module_c_info is None:
            return None
        return module_c_info.get("c_function", None)
    
    def _reset_focus(self, module: str, rtl_data: Dict):
        """
        Reset DFS focus to a new module.
        
        Args:
            module: Module name to focus on
            rtl_data: RTL data containing C source mappings
        """
        affecting = self.find_affecting_pragmas(module, rtl_data)
        self.current_module_pragmas = self.order_pragma_lists(affecting)
        self.current_pragma_index = 0
        self.current_option_index = 0
        self.current_option_results = []
        
        if self.logger:
            self.logger.print(f"[DFS Debug] Reset focus to module: {module}")
            self.logger.print(f"[DFS Debug] Found {len(affecting)} affecting pragmas: {affecting}")
            self.logger.print(f"[DFS Debug] Ordered pragmas: {[p['pragma_type'] for p in self.current_module_pragmas]}")

    def _validate_resume_state(self, level_heap: Dict, current_node: Optional[Dict] = None,
                               deepest_level: Optional[int] = None,
                               focus_pragma_idx: Optional[int] = None) -> bool:
        """
        Validate that the resume state is consistent and can be used for resuming.

        Args:
            level_heap: The loaded LevelHeap structure
            current_node: The current node being expanded (optional)
            deepest_level: The current deepest level (optional)
            focus_pragma_idx: The current focus pragma index (optional)

        Returns:
            True if state is valid, False otherwise
        """
        if not level_heap:
            if self.logger:
                self.logger.print("❌ Resume validation failed: Empty heap")
            return False

        # Validate all nodes in the heap have required fields
        required_fields = ["cfg", "tuned", "next_focus", "score"]
        for level, nodes in level_heap.items():
            if not isinstance(nodes, list):
                if self.logger:
                    self.logger.print(f"❌ Resume validation failed: Level {level} nodes is not a list")
                return False

            for i, node in enumerate(nodes):
                for field in required_fields:
                    if field not in node:
                        if self.logger:
                            self.logger.print(f"❌ Resume validation failed: Node at level {level}, index {i} missing field '{field}'")
                        return False

                # Validate cfg length matches pragma_locations
                if len(node["cfg"]) != len(self.pragma_locations):
                    if self.logger:
                        self.logger.print(f"❌ Resume validation failed: Node at level {level}, index {i} has cfg length {len(node['cfg'])}, expected {len(self.pragma_locations)}")
                    return False

        # If we have mid-node expansion state, validate it
        if current_node is not None:
            # Validate current_node has required fields
            for field in required_fields:
                if field not in current_node:
                    if self.logger:
                        self.logger.print(f"❌ Resume validation failed: Current node missing field '{field}'")
                    return False

            # Validate deepest_level is in heap (should be there after re-insertion)
            if deepest_level is not None and deepest_level not in level_heap:
                if self.logger:
                    self.logger.print(f"❌ Resume validation failed: Deepest level {deepest_level} not in heap (available levels: {list(level_heap.keys())})")
                return False

            # Validate focus_pragma_idx is valid
            if focus_pragma_idx is not None and focus_pragma_idx >= len(self.pragma_locations):
                if self.logger:
                    self.logger.print(f"❌ Resume validation failed: Focus pragma index {focus_pragma_idx} >= {len(self.pragma_locations)}")
                return False

        if self.logger:
            self.logger.print("✅ Resume state validation passed")
        return True

    def _is_child_of(self, child_module: str, parent_module: str, rtl_data: Dict) -> bool:
        """
        Check if child module is a child of parent module based on C function hierarchy.
        
        Args:
            child_module: Child module name
            parent_module: Parent module name
            rtl_data: RTL data containing C source mappings
            
        Returns:
            True if child is a child of parent based on C function analysis
        """
        if not parent_module:
            return False
            
        child_c_func = self._get_module_c_function(child_module, rtl_data)
        parent_c_func = self._get_module_c_function(parent_module, rtl_data)
        
        if not child_c_func or not parent_c_func:
            # Fallback to module name comparison if C function info unavailable
            return child_module.startswith(parent_module) and child_module != parent_module
        
        # Check if child function is called within parent function
        # For now, use simple containment check, but this could be enhanced
        # with actual call hierarchy analysis
        if child_c_func == parent_c_func:
            return False  # Same function, not child
            
        # Simple heuristic: if child function name contains parent function name,
        # it might be a nested/inlined function
        is_child = (parent_c_func in child_c_func and 
                   child_c_func != parent_c_func and
                   len(child_c_func) > len(parent_c_func))
        
        if self.logger:
            self.logger.print(f"[DFS Debug] C function hierarchy check: {child_c_func} child of {parent_c_func}? {is_child}")
        
        return is_child

    def _update_focus(self, new_module: str, rtl_data: Dict):
        """
        Update DFS focus based on new bottleneck module using C function correspondence.
        
        Args:
            new_module: New bottleneck module name
            rtl_data: RTL data containing C source mappings
        """
        # Get C function for new module
        new_c_func = self._get_module_c_function(new_module, rtl_data)
        
        if self.last_bottleneck_module is None:
            # First time - initialize focus
            if self.logger:
                self.logger.print(f"[DFS Debug] Initial focus setup for module: {new_module}")
            self._reset_focus(new_module, rtl_data)
            
        else:
            # Compare using stored C function (not looking up from current rtl_data)
            if self.logger:
                self.logger.print(f"[DFS Debug] Focus comparison: {new_module}({new_c_func}) vs {self.last_bottleneck_module}({self.last_bottleneck_c_function})")
            
            if new_module == self.last_bottleneck_module:
                # Same module - continue current exploration
                if self.logger:
                    self.logger.print("[DFS Debug] Same module - continuing current exploration")
                pass
                
            elif new_c_func and self.last_bottleneck_c_function and new_c_func == self.last_bottleneck_c_function:
                # Same C function - continue current exploration
                if self.logger:
                    self.logger.print("[DFS Debug] Same C function - continuing current exploration")
                pass
                
            elif self._is_child_of(new_module, self.last_bottleneck_module, rtl_data):
                # Child relationship - refine focus to more specific module
                if self.logger:
                    self.logger.print("[DFS Debug] Child relationship detected - refining focus")
                self._reset_focus(new_module, rtl_data)
                
            else:
                # Different function/module - switch exploration branch
                if self.logger:
                    self.logger.print("[DFS Debug] Different function/module - switching exploration branch")
                self._reset_focus(new_module, rtl_data)
                
        # Store both module name and C function for next iteration
        self.last_bottleneck_module = new_module
        self.last_bottleneck_c_function = new_c_func
    
    def save_state(self, output_dir: str, iteration: int, choices: List[str],
                   result_status: str, metrics: Optional[Dict] = None,
                   bottleneck_module: Optional[str] = None,
                   affecting_pragma_indices: Optional[List[int]] = None,
                   changed_pragma_index: Optional[int] = None,
                   level_heap: Optional[Dict] = None,
                   global_best_cfg: Optional[List[str]] = None,
                   global_best_score: Optional[int] = None,
                   current_node: Optional[Dict] = None,
                   current_deepest_level: Optional[int] = None,
                   current_focus_pragma_idx: Optional[int] = None,
                   explored_options: Optional[List[str]] = None):
        """
        Save exploration state to pickle and text files.

        Args:
            output_dir: Output directory for saving state
            iteration: Current iteration number
            choices: Current pragma choices
            result_status: Status of synthesis ("success", "timeout", "hls_failed")
            metrics: Performance metrics if available
            bottleneck_module: Name of the identified bottleneck module
            affecting_pragma_indices: List of pragma indices that could affect the bottleneck
            changed_pragma_index: Index of the pragma that was actually changed
            level_heap: LevelHeap state for resuming heap search
            global_best_cfg: Global best configuration found so far
            global_best_score: Global best score achieved so far
            current_node: Current node being expanded (for mid-expansion resume)
            current_deepest_level: Current deepest level being explored
            current_focus_pragma_idx: Current pragma being explored
            explored_options: List of options already explored for current pragma
        """
        output_path = Path(output_dir)
        output_path.mkdir(parents=True, exist_ok=True)
        
        # Create history entry
        history_entry = {
            "iter": iteration,
            "status": result_status,
            "new_launch": iteration == 0,
            "pragma_choices": choices.copy(),
            "timestamp": datetime.now().isoformat()
        }
        
        if metrics:
            history_entry["metrics"] = metrics
            
        # Add code region and pragma tracking information
        if bottleneck_module:
            history_entry["bottleneck_module"] = bottleneck_module
            
        if affecting_pragma_indices:
            history_entry["affecting_pragma_indices"] = affecting_pragma_indices
            # Add detailed pragma information for the affecting indices
            affecting_pragmas_details = []
            for idx in affecting_pragma_indices:
                if idx < len(self.pragma_locations):
                    pragma_info = self.pragma_locations[idx]
                    affecting_pragmas_details.append({
                        "index": idx,
                        "file": pragma_info["file"],
                        "line": pragma_info["line"],
                        "pragma_type": pragma_info["pragma_type"],
                        "category": pragma_info["category"],
                        "current_choice": choices[idx] if idx < len(choices) else ""
                    })
            history_entry["affecting_pragmas_details"] = affecting_pragmas_details
            
        if changed_pragma_index is not None:
            history_entry["changed_pragma_index"] = changed_pragma_index
            # Add details about the changed pragma
            if changed_pragma_index < len(self.pragma_locations):
                pragma_info = self.pragma_locations[changed_pragma_index]
                history_entry["changed_pragma_details"] = {
                    "index": changed_pragma_index,
                    "file": pragma_info["file"],
                    "line": pragma_info["line"],
                    "pragma_type": pragma_info["pragma_type"],
                    "category": pragma_info["category"],
                    "new_choice": choices[changed_pragma_index] if changed_pragma_index < len(choices) else ""
                }
        
        self.history.append(history_entry)

        # Save pickle checkpoint
        state = {
            "next_iter": iteration + 1,
            "cur_pragmas": choices,
            "history": self.history,
            "target_ii": self.target_ii,
            "objective_description": self.objective_fn.get_target_description(),
            "level_heap": level_heap,
            "global_best_cfg": global_best_cfg,
            "global_best_score": global_best_score,
            # Node expansion state for mid-node resume
            "current_node": current_node,
            "current_deepest_level": current_deepest_level,
            "current_focus_pragma_idx": current_focus_pragma_idx,
            "explored_options": explored_options if explored_options else []
        }
        
        pickle_file = output_path / "pragma_state.pkl"
        with open(pickle_file, 'wb') as f:
            pickle.dump(state, f)
        
        # Save human-readable text
        self._write_text_summary(output_path)
        
        if self.logger:
            self.logger.print(f"[DSEExplorer] State saved for iteration {iteration}")
        else:
            print(f"[DSEExplorer] State saved for iteration {iteration}")


    def _write_text_summary(self, output_dir: Path):
        """
        Write human-readable summary of exploration state including all pickle data.

        Args:
            output_dir: Directory to write summary
        """
        txt_file = output_dir / "pragma_state.txt"
        pickle_file = output_dir / "pragma_state.pkl"

        # Load pickle data to display comprehensive state information
        state_data = None
        if pickle_file.exists():
            with open(pickle_file, 'rb') as f:
                state_data = pickle.load(f)

        with open(txt_file, 'w') as f:
            f.write(f"# Pragma search log — generated {datetime.now()}\n")
            f.write(f"# Objective: {self.objective_fn.get_target_description()}\n")

            # Display global state from pickle
            if state_data:
                f.write("\n" + "="*80 + "\n")
                f.write("GLOBAL STATE (from pragma_state.pkl)\n")
                f.write("="*80 + "\n\n")

                f.write(f"🔢 Next Iteration: {state_data.get('next_iter', 'N/A')}\n")
                f.write(f"🎯 Target II: {state_data.get('target_ii', 'N/A')}\n")
                f.write(f"📝 Objective Description: {state_data.get('objective_description', 'N/A')}\n")

                # Display global best configuration if available
                global_best_score = state_data.get('global_best_score')
                global_best_cfg = state_data.get('global_best_cfg')

                if global_best_score is not None:
                    f.write(f"\n🏆 Global Best Score: {global_best_score}\n")

                if global_best_cfg:
                    f.write(f"🏆 Global Best Configuration ({len(global_best_cfg)} pragmas):\n")
                    for idx, choice in enumerate(global_best_cfg):
                        f.write(f"    [{idx}]: {choice}\n")

                # Display LevelHeap state if available
                level_heap = state_data.get('level_heap')
                if level_heap:
                    f.write(f"\n🗂️  LevelHeap State ({len(level_heap)} levels):\n")
                    for level in sorted(level_heap.keys()):
                        nodes = level_heap[level]
                        f.write(f"  Level {level}: {len(nodes)} node(s)\n")
                        for i, node in enumerate(nodes):
                            f.write(f"    Node {i+1}:\n")
                            f.write(f"      Score: {node.get('score', 'N/A')}\n")
                            f.write(f"      Next Focus: {node.get('next_focus', 'N/A')}\n")
                            f.write(f"      Tuned Pragmas: {len(node.get('tuned', []))} ({node.get('tuned', [])})\n")
                            f.write(f"      Bottleneck Module: {node.get('bottleneck_module', 'N/A')}\n")

                # Display current pragma choices
                cur_pragmas = state_data.get('cur_pragmas')
                if cur_pragmas:
                    f.write(f"\n🔧 Current Pragma Choices ({len(cur_pragmas)} pragmas):\n")
                    for idx, choice in enumerate(cur_pragmas):
                        f.write(f"    [{idx}]: {choice}\n")

                # Display node expansion resume state
                current_node = state_data.get('current_node')
                if current_node is not None:
                    f.write(f"\n🔄 Mid-Node Expansion State (for resume):\n")
                    f.write(f"  Current Deepest Level: {state_data.get('current_deepest_level', 'N/A')}\n")
                    f.write(f"  Current Focus Pragma Index: {state_data.get('current_focus_pragma_idx', 'N/A')}\n")

                    explored_options = state_data.get('explored_options', [])
                    f.write(f"  Explored Options: {len(explored_options)} ({explored_options})\n")

                    f.write(f"  Current Node Details:\n")
                    f.write(f"    Score: {current_node.get('score', 'N/A')}\n")
                    f.write(f"    Next Focus: {current_node.get('next_focus', 'N/A')}\n")
                    f.write(f"    Tuned Pragmas: {len(current_node.get('tuned', []))} ({current_node.get('tuned', [])})\n")
                    f.write(f"    Bottleneck Module: {current_node.get('bottleneck_module', 'N/A')}\n")

                f.write("\n" + "="*80 + "\n")
                f.write("ITERATION HISTORY\n")
                f.write("="*80 + "\n\n")
            else:
                f.write("\n⚠️  No pickle state file found\n\n")

            # Display iteration history
            for entry in self.history:
                f.write(f"=== Iteration {entry['iter']} — {entry['status']} - new_launch {entry['new_launch']} ===\n")

                # Show timestamp if available
                if 'timestamp' in entry:
                    f.write(f"  ⏰ Timestamp: {entry['timestamp']}\n")

                # Show bottleneck module and code region analysis
                if 'bottleneck_module' in entry:
                    f.write(f"  🎯 Bottleneck Module: {entry['bottleneck_module']}\n")

                if 'affecting_pragmas_details' in entry:
                    f.write(f"  📍 Pragmas of Interest ({len(entry['affecting_pragmas_details'])} found):\n")
                    for pragma in entry['affecting_pragmas_details']:
                        f.write(f"    [{pragma['index']}] {pragma['pragma_type']} in {pragma['file']}:{pragma['line']} -> '{pragma['current_choice']}'\n")

                if 'changed_pragma_details' in entry:
                    changed = entry['changed_pragma_details']
                    f.write(f"  ✏️  Changed Pragma: [{changed['index']}] {changed['pragma_type']} in {changed['file']}:{changed['line']} -> '{changed['new_choice']}'\n")

                if 'metrics' in entry:
                    # Write objective evaluation if available
                    if 'objective_result' in entry['metrics']:
                        f.write(f"  🎯 Objective Result: {entry['metrics']['objective_result']}\n")

                        if 'objective_details' in entry['metrics']:
                            obj_details = entry['metrics']['objective_details']
                            f.write("  📋 Objective Details:\n")
                            for key, value in obj_details.items():
                                if key != "bottleneck_module":  # Already shown above
                                    f.write(f"    {key}: {value}\n")

                    f.write("  📊 Metrics: " + json.dumps(entry['metrics'], indent=4) + "\n")

                f.write("  🔧 All Pragma Choices:\n")
                for idx, choice in enumerate(entry['pragma_choices']):
                    marker = " 👈 CHANGED" if 'changed_pragma_index' in entry and entry['changed_pragma_index'] == idx else ""
                    f.write(f"    [{idx}]: {choice}{marker}\n")

                f.write("\n")



    def load_state(self, output_dir: str) -> Tuple[int, List[str], Dict, List[str], int, Dict, int, int, List[str]]:
        """
        Load exploration state from checkpoint.

        Args:
            output_dir: Directory containing checkpoint files

        Returns:
            Tuple of (start_iteration, current_choices, level_heap, global_best_cfg, global_best_score,
                     current_node, current_deepest_level, current_focus_pragma_idx, explored_options)
        """
        checkpoint_file = Path(output_dir) / "pragma_state.pkl"

        if checkpoint_file.exists():
            with open(checkpoint_file, 'rb') as f:
                state = pickle.load(f)

            self.history = state.get("history", [])
            start_iter = state.get("next_iter", 0)
            current_choices = state.get("cur_pragmas", self.container.get_initial_choices())
            level_heap = state.get("level_heap", {})
            global_best_cfg = state.get("global_best_cfg", None)
            global_best_score = state.get("global_best_score", "FAIL SYNTHESIS")

            # Load node expansion state
            current_node = state.get("current_node", None)
            current_deepest_level = state.get("current_deepest_level", None)
            current_focus_pragma_idx = state.get("current_focus_pragma_idx", None)
            explored_options = state.get("explored_options", [])

            if self.logger:
                self.logger.print(f"[DSEExplorer] Resumed from iteration {start_iter}")
                if level_heap:
                    self.logger.print(f"[DSEExplorer] Loaded LevelHeap with {len(level_heap)} levels")
                    for level in sorted(level_heap.keys()):
                        self.logger.print(f"  Level {level}: {len(level_heap[level])} nodes")
                if global_best_cfg is not None:
                    self.logger.print(f"[DSEExplorer] Global best score: {global_best_score}")
                if current_node is not None:
                    self.logger.print(f"[DSEExplorer] Resuming mid-node expansion:")
                    self.logger.print(f"  Deepest level: {current_deepest_level}")
                    self.logger.print(f"  Focus pragma: {current_focus_pragma_idx}")
                    self.logger.print(f"  Explored options: {len(explored_options)} ({explored_options})")
            else:
                print(f"[DSEExplorer] Resumed from iteration {start_iter}")
                if level_heap:
                    print(f"[DSEExplorer] Loaded LevelHeap with {len(level_heap)} levels")
                if global_best_cfg is not None:
                    print(f"[DSEExplorer] Global best score: {global_best_score}")

            return start_iter, current_choices, level_heap, global_best_cfg, global_best_score, \
                   current_node, current_deepest_level, current_focus_pragma_idx, explored_options
        else:
            if self.logger:
                self.logger.print("[DSEExplorer] No checkpoint found, starting fresh")
            else:
                print("[DSEExplorer] No checkpoint found, starting fresh")
            return 0, self.container.get_initial_choices(), {}, None, "FAIL SYNTHESIS", \
                   None, None, None, []
      
    def run_exploration_LevelHeap_DFS(self, max_iterations: int, output_dir: str, timeout_min: int = 10, max_exploration_hours: Optional[float] = None) -> List[Dict]:
        """
        Run LevelHeap DFS exploration as described in the requirements.
        
        Args:
            max_iterations: Maximum number of iterations to run (can be float('inf') for time-based exploration)
            output_dir: Directory for saving variants and results
            timeout_min: Timeout for each HLS synthesis in minutes
            max_exploration_hours: Maximum time to run exploration in hours (overrides max_iterations if set)
            
        Returns:
            List of iteration results
        """
        # Set up time-based exploration if specified
        import time
        start_time = time.time()
        max_exploration_seconds = max_exploration_hours * 3600 if max_exploration_hours else None
        
        if self.logger:
            if max_exploration_hours:
                self.logger.print(f"[DSEExplorer] Starting LevelHeap DFS exploration: {max_exploration_hours} hours, timeout {timeout_min}min")
            else:
                self.logger.print(f"[DSEExplorer] Starting LevelHeap DFS exploration: {max_iterations} iterations, timeout {timeout_min}min")
        else:
            if max_exploration_hours:
                print(f"[DSEExplorer] Starting LevelHeap DFS exploration: {max_exploration_hours} hours, timeout {timeout_min}min")
            else:
                print(f"[DSEExplorer] Starting LevelHeap DFS exploration: {max_iterations} iterations, timeout {timeout_min}min")
        
        # Load previous state if available
        start_iter, self.current_choices, loaded_level_heap, loaded_global_best_cfg, loaded_global_best_score, \
        loaded_current_node, loaded_deepest_level, loaded_focus_pragma_idx, loaded_explored_options = self.load_state(output_dir)

        # LevelHeap DFS state - each level contains nodes to explore
        # Level 0 contains initial configuration, Level N contains nodes with N pragmas tuned
        # Use loaded state if available, otherwise initialize fresh
        level_heap = loaded_level_heap if loaded_level_heap else {}  # level -> list of nodes
        global_best_cfg = loaded_global_best_cfg if loaded_global_best_cfg else None
        global_best_score = loaded_global_best_score if loaded_global_best_score != "FAIL SYNTHESIS" else "FAIL SYNTHESIS"  # Start with worst possible score

        # Resume state for mid-node expansion
        resume_current_node = loaded_current_node
        resume_deepest_level = loaded_deepest_level
        resume_focus_pragma_idx = loaded_focus_pragma_idx
        resume_explored_options = loaded_explored_options if loaded_explored_options else []
        
        # Node structure: {
        #   "cfg": current pragma choices,
        #   "tuned": list of (pragma_index, value) pairs that have been tuned,
        #   "next_focus": pragma index to focus on next,
        #   "score": objective score (latency for ResourceConstrainedLatencyObjective)
        # }
        
        # Initialize with default configuration for fresh start
        # The first iteration is expected to be success,
        #     otherwise, either the timeout is too short or the design is uncompilable or unsynthesizable
        if start_iter == 0:
            # Set all unroll pragmas to 1 for initial configuration
            for i, pragma_info in enumerate(self.pragma_locations):
                pragma_type = pragma_info.get("pragma_type", "").lower()
                # Check if this is an unroll pragma
                if "unroll" in pragma_type:
                    # Set to 1 instead of default ""
                    if "1" in pragma_info.get("options", []):
                        self.current_choices[i] = "1"
                    elif 1 in pragma_info.get("options", []):
                        self.current_choices[i] = 1

            if self.logger:
                unroll_count = sum(1 for i, pragma_info in enumerate(self.pragma_locations)
                                 if "unroll" in pragma_info.get("pragma_type", "").lower())
                self.logger.print(f"[DSEExplorer] Set {unroll_count} unroll pragmas to 1 for initial configuration")

            # Evaluate default configuration first
            iteration = 0
            if self.logger:
                self.logger.print(f"\n=== LevelHeap DFS Iteration {iteration} (Default Configuration) ===")
            else:
                print(f"\n=== LevelHeap DFS Iteration {iteration} (Default Configuration) ===")

            # Generate and evaluate default variant
            variant_dir = self.container.generate_variant(self.current_choices, iteration, output_dir)
            work_dir = f"{output_dir}/work_{iteration}"
            design = ConcreteDesign(variant_dir, work_dir, force_overwrite=True)
            if self.logger:
                design.set_logger(self.logger)
            
            
            result_status = design.run_vitis_hls(timeout_min=timeout_min)
            
            if result_status == "success":
                design.run_all_analyses()
                
                # Load synthesis results
                logs_dir = Path(work_dir) / "logs"
                csynth_file = logs_dir / "csynth_module_info.json"
                rtl_file = logs_dir / "rtl_c_annotated.json"
                
                if csynth_file.exists() and rtl_file.exists():
                    with open(csynth_file) as f:
                        csynth_data = json.load(f)
                    with open(rtl_file) as f:
                        rtl_data = json.load(f)
                    
                    # Evaluate objective function
                    objective_result, objective_details = self.objective_fn.evaluate(csynth_data, rtl_data)
                    bottleneck_module = objective_details.get("bottleneck_module", "FFT_TOP")
                    
                    # Check if resources are exceeded for ResourceConstrainedLatencyObjective
                    from objective.objectives import ObjectiveResult
                    if objective_result == ObjectiveResult.INFEASIBLE:
                        # Design failed due to resource constraints
                        score = self._get_failure_score(result_status, objective_result)
                        if self.logger:
                            violations = objective_details.get('resource_violations', [])
                            self.logger.print(f"❌ Default configuration exceeds resource constraints: {violations}")
                    else:
                        # Extract score based on objective type
                        if hasattr(objective_details, 'get'):
                            raw_score = objective_details.get('latency', objective_details.get('current_ii', objective_details.get('combined_score', 1000)))
                            try:
                                score = float(raw_score)
                            except (ValueError, TypeError):
                                score = 1000.0  # Default fallback if conversion fails
                        else:
                            score = 1000.0  # Default fallback
                    
                    # Find affecting pragmas for bottleneck analysis
                    affecting_indices = self.find_affecting_pragmas(bottleneck_module, rtl_data)
                    # Order ALL pragmas by hierarchical importance (not just bottleneck-affecting ones)
                    all_pragma_indices = list(range(len(self.pragma_locations)))
                    ordered_pragmas = self.order_pragma_lists_hierarchical(all_pragma_indices, csynth_data, rtl_data, work_dir)
                    
                    # Determine next focus pragma from the ordered list (contains ALL pragmas ranked by importance)
                    next_focus = None
                    for pragma_dict in ordered_pragmas:
                        if pragma_dict["index"] not in []:
                            next_focus = pragma_dict["index"]
                            break
                    
                    # Create initial node
                    initial_node = {
                        "cfg": list(self.current_choices),
                        "tuned": [],
                        "next_focus": next_focus,
                        "score": score,
                        "bottleneck_module": bottleneck_module,
                        "ordered_pragmas": ordered_pragmas
                    }
                    
                    # Add to level 0
                    level_heap[0] = [initial_node]
                    global_best_cfg = list(self.current_choices)
                    global_best_score = score
                    
                    if self.logger:
                        self.logger.print(f"✅ Default config evaluated: score={score}, bottleneck={bottleneck_module}")
                        self.logger.print(f"[LevelHeap] Initialized Level 0 with default config")
                        self.logger.print(f"[LevelHeap] Next focus pragma index: {next_focus}")
                    
                    # Update input hierarchy in LevelHeap trace file (first iteration only)
                    if iteration == 0 and self.logger:
                        self.logger.update_levelheap_trace_input_hierarchy(work_dir)
                    
                    # Save LevelHeap trace for initial configuration
                    if self.logger:
                        self.logger.save_levelheap_dfs_trace(
                            iteration, work_dir, bottleneck_module, 
                            [], None,  # No affecting pragmas or changed pragma for initial config
                            self.pragma_locations, self.current_choices,
                            level_heap, initial_node, global_best_score
                        )
                        
                        # Extract resource constraints from objective function
                        max_resources = {}
                        if hasattr(self.objective_fn, 'max_dsp'):
                            max_resources['max_dsp'] = self.objective_fn.max_dsp
                        if hasattr(self.objective_fn, 'max_bram'):
                            max_resources['max_bram'] = self.objective_fn.max_bram
                        if hasattr(self.objective_fn, 'max_lut'):
                            max_resources['max_lut'] = self.objective_fn.max_lut
                        if hasattr(self.objective_fn, 'max_ff'):
                            max_resources['max_ff'] = self.objective_fn.max_ff
                        if hasattr(self.objective_fn, 'max_uram'):
                            max_resources['max_uram'] = self.objective_fn.max_uram
                        
                        # Save iteration data to final_trace.json
                        self.logger.save_iteration_trace(
                            iteration, self.pragma_locations, self.current_choices,
                            result_status, objective_result, objective_details, max_resources
                        )
                    
                    # Save state
                    enhanced_metrics = csynth_data.copy()
                    enhanced_metrics["objective_result"] = objective_result.value if hasattr(objective_result, 'value') else str(objective_result)
                    enhanced_metrics["objective_details"] = objective_details
                    
                    self.save_state(output_dir, iteration, self.current_choices, result_status, 
                                  enhanced_metrics, bottleneck_module, affecting_indices, None, level_heap, global_best_cfg, global_best_score)
                    
                    start_iter = 1  # Continue from iteration 1
            else:
                if self.logger:
                    self.logger.print(f"❌ Default configuration synthesis failed: {result_status}")
                return self.history
        else:
            # Resuming from checkpoint - use loaded state directly
            if self.logger:
                self.logger.print(f"[LevelHeap] Resuming from iteration {start_iter}")
                self.logger.print(f"[LevelHeap] Loaded heap state: {len(level_heap)} levels")
                for level in sorted(level_heap.keys()):
                    self.logger.print(f"  Level {level}: {len(level_heap[level])} nodes")

            # If we have a mid-node expansion state, we DON'T re-insert it to heap
            # Instead, we'll use it directly in the first iteration of the main loop
            # This avoids unnecessary pop/push operations
            if resume_current_node is not None:
                if self.logger:
                    self.logger.print(f"[LevelHeap] Will resume mid-node expansion at level {resume_deepest_level}")
                    self.logger.print(f"[LevelHeap] Will skip {len(resume_explored_options)} already-explored options")

            # Validate loaded heap state
            if not self._validate_resume_state(level_heap, None, None, None):
                if self.logger:
                    self.logger.print("❌ ERROR: Resume state validation failed, cannot resume")
                return self.history

        # Main LevelHeap DFS exploration loop
        iteration = start_iter

        # Track how many iterations have been completed (for max_iterations counting)
        # max_iterations is the TOTAL number of iterations, not the iteration number
        # Count successful evaluations from history
        completed_iterations = len([h for h in self.history if h.get('status') not in ['iteration_limit_reached', 'time_limit_reached']])

        if self.logger:
            self.logger.print(f"[DEBUG] Starting main loop: iteration={iteration}, max_iterations={max_iterations}")
            self.logger.print(f"[DEBUG] Completed iterations so far: {completed_iterations}")
            self.logger.print(f"[DEBUG] Initial level_heap: {list(level_heap.keys()) if level_heap else 'empty'}")
            self.logger.print(f"[DEBUG] Iterations remaining: {max(0, max_iterations - completed_iterations)}")

        while True:
            # Check continuation conditions
            should_continue = True

            # Check time limit
            if max_exploration_seconds:
                elapsed_time = time.time() - start_time
                if elapsed_time >= max_exploration_seconds:
                    if self.logger:
                        self.logger.print(f"⏰ Time limit reached: {elapsed_time/3600:.2f} hours")
                    should_continue = False

            # Check iteration limit (based on total completed iterations, not iteration number)
            if completed_iterations >= max_iterations:
                if self.logger:
                    self.logger.print(f"🔢 Iteration limit reached: {completed_iterations} completed iterations >= {max_iterations} max")
                    self.logger.print(f"   Note: max_iterations is the TOTAL iteration count (including resumed runs)")
                    self.logger.print(f"   To run more iterations, increase max_iterations to > {completed_iterations}")
                should_continue = False
            
            # Check if there are nodes to explore
            if not level_heap:
                if self.logger:
                    self.logger.print("[LevelHeap] No more nodes to explore - heap is empty")
                    self.logger.print(f"[LevelHeap] Exploration ended after {iteration - start_iter} iterations")
                should_continue = False
            
            if not should_continue:
                break
            # Display time progress for time-based exploration
            if max_exploration_seconds:
                elapsed_time = time.time() - start_time
                remaining_time = max_exploration_seconds - elapsed_time
                if self.logger:
                    self.logger.print(f"\n=== LevelHeap DFS Iteration {iteration} ===")
                    self.logger.print(f"⏰ Time: {elapsed_time/3600:.2f}h elapsed, {remaining_time/3600:.2f}h remaining")
                    self.logger.print(f"[LevelHeap] Current heap levels: {list(level_heap.keys())}")
                else:
                    print(f"\n=== LevelHeap DFS Iteration {iteration} ===")
                    print(f"⏰ Time: {elapsed_time/3600:.2f}h elapsed, {remaining_time/3600:.2f}h remaining")
            else:
                if self.logger:
                    self.logger.print(f"\n=== LevelHeap DFS Iteration {iteration} ===")
                    self.logger.print(f"[LevelHeap] Current heap levels: {list(level_heap.keys())}")
                else:
                    print(f"\n=== LevelHeap DFS Iteration {iteration} ===")
            
            # Check if we're resuming mid-node expansion
            # If so, use the saved node directly instead of selecting from heap
            if resume_current_node is not None:
                # Use the saved node that was being expanded when interrupted
                current_node = resume_current_node
                deepest_level = resume_deepest_level
                if self.logger:
                    self.logger.print(f"[LevelHeap] Resuming expansion of saved node at level {deepest_level}")
                    self.logger.print(f"[LevelHeap] Current node: tuned={current_node['tuned']}, next_focus={current_node['next_focus']}, score={current_node['score']}")
                # Note: We don't pop from heap because this node was never re-inserted
            else:
                # Normal flow: select from heap
                # Find the deepest non-empty level (DFS)
                # Note: level_heap check is already done above in continuation conditions

                deepest_level = max(level_heap.keys())
                current_level_nodes = level_heap[deepest_level]

                if not current_level_nodes:
                    # Remove empty level and continue
                    del level_heap[deepest_level]
                    continue

                # Select the best scoring node from the deepest level
                # Find the node with the best (lowest) valid score
                best_node_idx = 0
                best_score = current_level_nodes[0]["score"]

                for i, node in enumerate(current_level_nodes):
                    node_score = node["score"]
                    if self._is_better_score(node_score, best_score):
                        best_score = node_score
                        best_node_idx = i

                # Remove and return the best node
                current_node = current_level_nodes.pop(best_node_idx)

                if self.logger:
                    self.logger.print(f"[LevelHeap] Exploring BEST node from level {deepest_level} (selected {best_node_idx+1}/{len(current_level_nodes)+1})")
                    self.logger.print(f"[LevelHeap] Current node: tuned={current_node['tuned']}, next_focus={current_node['next_focus']}, score={current_node['score']}")

                # If no more nodes at this level, remove the level
                if not current_level_nodes:
                    del level_heap[deepest_level]
            
            # Check if we have a next focus pragma to explore
            if current_node["next_focus"] is None:
                if self.logger:
                    self.logger.print("[LevelHeap] No next focus pragma, continuing")
                continue
            
            focus_pragma_idx = current_node["next_focus"]
            
            # Log node expansion start
            if self.logger:
                self.logger.log_node_expansion_start(iteration, deepest_level, current_node, 
                                                   self.pragma_locations, focus_pragma_idx)
            
            # Get options for the focused pragma
            if focus_pragma_idx >= len(self.pragma_locations):
                if self.logger:
                    self.logger.print(f"[LevelHeap] Invalid pragma index {focus_pragma_idx}, continuing")
                continue
                
            pragma_info = self.pragma_locations[focus_pragma_idx]
            pragma_options = pragma_info["options"]
            
            if self.logger:
                self.logger.print(f"[LevelHeap] Expanding pragma {focus_pragma_idx} ({pragma_info['pragma_type']}) with {len(pragma_options)} options")

            # Check if we're resuming this specific node's expansion
            # If so, we need to track which options have been explored
            if (resume_current_node is not None and
                resume_deepest_level == deepest_level and
                resume_focus_pragma_idx == focus_pragma_idx and
                current_node == resume_current_node):
                # We're resuming this exact node - use the loaded explored options
                explored_options_for_this_node = resume_explored_options.copy()
                if self.logger:
                    self.logger.print(f"[LevelHeap] Resuming node expansion, will skip {len(explored_options_for_this_node)} options: {explored_options_for_this_node}")
                # Clear resume state since we're now handling it
                resume_current_node = None
            else:
                # Fresh node expansion - no options explored yet
                explored_options_for_this_node = []

            # Explore each option for the focused pragma
            for option in pragma_options:
                # Check iteration limit before processing each option
                # This ensures we don't exceed the total number of HLS syntheses
                if completed_iterations >= max_iterations:
                    if self.logger:
                        self.logger.print(f"🔢 Iteration limit reached: {completed_iterations} completed iterations >= {max_iterations} max")
                    # Don't save state here - we'll save at the end naturally
                    break

                # Check time limit before processing each option
                if max_exploration_seconds:
                    elapsed_time = time.time() - start_time
                    if elapsed_time >= max_exploration_seconds:
                        if self.logger:
                            self.logger.print(f"⏰ Time limit reached during pragma option exploration")
                        # Save current state before breaking (only if we've done some work)
                        if explored_options_for_this_node:
                            self.save_state(output_dir, iteration, new_cfg if 'new_cfg' in locals() else current_node["cfg"],
                                          "time_limit_reached", {}, None, [], None,
                                          level_heap, global_best_cfg, global_best_score,
                                          current_node, deepest_level, focus_pragma_idx, explored_options_for_this_node)
                        break

                # Skip if this option was already explored in a previous run if the DSE was interrupted and resumed
                if option in explored_options_for_this_node:
                    if self.logger:
                        self.logger.print(f"[LevelHeap] Skipping already-explored option: '{option}'")
                    continue

                # Create new configuration
                new_cfg = list(current_node["cfg"])
                old_value = new_cfg[focus_pragma_idx]
                new_cfg[focus_pragma_idx] = option

                # Add to explored options list
                explored_options_for_this_node.append(option)

                if self.logger:
                    self.logger.print(f"[LevelHeap] Trying pragma {focus_pragma_idx}: '{old_value}' -> '{option}'")

                # Check if option is same as current value - skip synthesis and reuse parent results
                skip_synthesis = (option == old_value)

                if skip_synthesis:
                    if self.logger:
                        self.logger.print(f"[LevelHeap] Option '{option}' same as current value - reusing parent node results, skipping synthesis")

                    # Import ObjectiveResult for status (imported here to avoid dependency on normal path)
                    from objective.objectives import ObjectiveResult

                    # Reuse parent node's results (parent is always successful since it's in the heap)
                    result_status = "success"
                    score = current_node["score"]
                    bottleneck_module = current_node.get("bottleneck_module", "unknown")
                    ordered_pragmas = current_node.get("ordered_pragmas", [])
                    objective_result = ObjectiveResult.NEEDS_IMPROVEMENT
                    objective_details = {"latency": score, "bottleneck_module": bottleneck_module}
                    work_dir = None  # No work directory since we skipped synthesis
                    analysis_success = True
                else:
                    # Generate and evaluate variant
                    variant_dir = self.container.generate_variant(new_cfg, iteration, output_dir)
                    work_dir = f"{output_dir}/work_{iteration}"
                    design = ConcreteDesign(variant_dir, work_dir, force_overwrite=True)
                    if self.logger:
                        design.set_logger(self.logger)

                    result_status = design.run_vitis_hls(timeout_min=timeout_min)

                # Handle successful synthesis or skipped synthesis
                if skip_synthesis:
                    # Add node to heap using reused parent data (already set above)
                    if self._is_valid_score(score):
                        # Determine next focus pragma from the ordered list
                        tuned_pragmas = set([pragma_idx for pragma_idx, _ in current_node["tuned"]] + [focus_pragma_idx])
                        next_focus = None
                        for pragma_dict in ordered_pragmas:
                            if pragma_dict["index"] not in tuned_pragmas:
                                next_focus = pragma_dict["index"]
                                break

                        # Create new node for next level
                        new_tuned = current_node["tuned"] + [(focus_pragma_idx, option)]
                        new_node = {
                            "cfg": new_cfg,
                            "tuned": new_tuned,
                            "next_focus": next_focus,
                            "score": score,
                            "bottleneck_module": bottleneck_module,
                            "ordered_pragmas": ordered_pragmas
                        }

                        # Add to next level
                        next_level = len(new_tuned)
                        if next_level not in level_heap:
                            level_heap[next_level] = []
                        level_heap[next_level].append(new_node)

                        if self.logger:
                            self.logger.print(f"✅ Added new node to level {next_level}: score={score}, next_focus={next_focus} (reused parent data)")

                elif result_status == "success":
                    # Try to run all analyses, catch any parsing/analysis errors
                    try:
                        design.run_all_analyses()
                        analysis_success = True
                    except Exception as e:
                        analysis_success = False
                        if self.logger:
                            self.logger.print(f"⚠️  Analysis failed: {str(e)}")
                            self.logger.print(f"   Marking design as unanalyzable and continuing DSE")

                    # Load synthesis results
                    logs_dir = Path(work_dir) / "logs"
                    csynth_file = logs_dir / "csynth_module_info.json"
                    rtl_file = logs_dir / "rtl_c_annotated.json"

                    if analysis_success and csynth_file.exists() and rtl_file.exists():
                        with open(csynth_file) as f:
                            csynth_data = json.load(f)
                        with open(rtl_file) as f:
                            rtl_data = json.load(f)
                        
                        # Evaluate objective function
                        objective_result, objective_details = self.objective_fn.evaluate(csynth_data, rtl_data)
                        bottleneck_module = objective_details.get("bottleneck_module", "FFT_TOP")
                        
                        # Check if design is feasible
                        from objective.objectives import ObjectiveResult
                        if objective_result == ObjectiveResult.INFEASIBLE:
                            # Design failed due to resource constraints
                            score = self._get_failure_score(result_status, objective_result)
                            if self.logger:
                                violations = objective_details.get('resource_violations', [])
                                self.logger.print(f"❌ Resource constraints violated: {violations}")
                        else:
                            # Extract score based on objective type
                            raw_score = objective_details.get('latency', objective_details.get('current_ii', objective_details.get('combined_score', 1000)))
                            try:
                                score = float(raw_score)
                            except (ValueError, TypeError):
                                score = 1000.0  # Default fallback if conversion fails
                        
                        # Update global best if this is better (handle invalid scores)
                        if self._is_better_score(score, global_best_score):
                            global_best_cfg = list(new_cfg)
                            global_best_score = score
                            if self.logger:
                                self.logger.print(f"🏆 New global best: score={score}")
                        
                        # Find affecting pragmas for bottleneck analysis
                        affecting_indices = self.find_affecting_pragmas(bottleneck_module, rtl_data)
                        # Order ALL pragmas by hierarchical importance (not just bottleneck-affecting ones)
                        all_pragma_indices = list(range(len(self.pragma_locations)))
                        ordered_pragmas = self.order_pragma_lists_hierarchical(all_pragma_indices, csynth_data, rtl_data, work_dir)
                        
                        # Check if this node should be added to next level (only valid scores)
                        if self._is_valid_score(score):
                            # Determine next focus pragma from the ordered list (contains ALL pragmas ranked by importance)
                            tuned_pragmas = set([pragma_idx for pragma_idx, _ in current_node["tuned"]] + [focus_pragma_idx])
                            next_focus = None
                            for pragma_dict in ordered_pragmas:
                                if pragma_dict["index"] not in tuned_pragmas:
                                    next_focus = pragma_dict["index"]
                                    break
                            
                            # Create new node for next level
                            new_tuned = current_node["tuned"] + [(focus_pragma_idx, option)]
                            new_node = {
                                "cfg": new_cfg,
                                "tuned": new_tuned,
                                "next_focus": next_focus,
                                "score": score,
                                "bottleneck_module": bottleneck_module,
                                "ordered_pragmas": ordered_pragmas
                            }
                            
                            # Add to next level
                            next_level = len(new_tuned)
                            if next_level not in level_heap:
                                level_heap[next_level] = []
                            level_heap[next_level].append(new_node)
                            
                            if self.logger:
                                self.logger.print(f"✅ Added new node to level {next_level}: score={score}, next_focus={next_focus}")
                                # Log child node generation
                                self.logger.log_child_node_generation(iteration, focus_pragma_idx, old_value, 
                                                                    option, score, next_level, next_focus, 
                                                                    self.pragma_locations)
                        else:
                            # Prune nodes with invalid scores (resource violations, etc.)
                            if self.logger:
                                self.logger.print(f"🚫 Pruning node with invalid score: {score}, will not expand further")
                            
                            # Create temporary node for logging purposes only (not added to heap)
                            new_tuned = current_node["tuned"] + [(focus_pragma_idx, option)]
                            new_node = {
                                "cfg": new_cfg,
                                "tuned": new_tuned,
                                "next_focus": None,  # No next focus since node is pruned
                                "score": score,
                                "bottleneck_module": bottleneck_module,
                                "ordered_pragmas": ordered_pragmas
                            }
                        
                        # Prepare affecting pragmas details for trace
                        affecting_pragmas_details = []
                        for idx in affecting_indices:
                            if idx < len(self.pragma_locations):
                                pragma_info = self.pragma_locations[idx]
                                affecting_pragmas_details.append({
                                    "pragma_type": pragma_info["pragma_type"],
                                    "file": pragma_info["file"],
                                    "line": pragma_info["line"],
                                    "current_choice": new_cfg[idx] if idx < len(new_cfg) else ""
                                })

                        # Prepare changed pragma details for trace
                        changed_pragma_details = None
                        if focus_pragma_idx < len(self.pragma_locations):
                            pragma_info = self.pragma_locations[focus_pragma_idx]
                            changed_pragma_details = {
                                "pragma_type": pragma_info["pragma_type"],
                                "file": pragma_info["file"],
                                "line": pragma_info["line"],
                                "new_choice": new_cfg[focus_pragma_idx] if focus_pragma_idx < len(new_cfg) else ""
                            }
                        
                        # Save LevelHeap trace
                        if self.logger:
                            self.logger.save_levelheap_dfs_trace(
                                iteration, work_dir, bottleneck_module, 
                                affecting_pragmas_details, changed_pragma_details,
                                self.pragma_locations, new_cfg,
                                level_heap, new_node, global_best_score
                            )
                            
                            # Extract resource constraints from objective function
                            max_resources = {}
                            if hasattr(self.objective_fn, 'max_dsp'):
                                max_resources['max_dsp'] = self.objective_fn.max_dsp
                            if hasattr(self.objective_fn, 'max_bram'):
                                max_resources['max_bram'] = self.objective_fn.max_bram
                            if hasattr(self.objective_fn, 'max_lut'):
                                max_resources['max_lut'] = self.objective_fn.max_lut
                            if hasattr(self.objective_fn, 'max_ff'):
                                max_resources['max_ff'] = self.objective_fn.max_ff
                            if hasattr(self.objective_fn, 'max_uram'):
                                max_resources['max_uram'] = self.objective_fn.max_uram
                            
                            # Save iteration data to final_trace.json (skip if synthesis was skipped to avoid duplicates)
                            if not skip_synthesis:
                                self.logger.save_iteration_trace(
                                    iteration, self.pragma_locations, new_cfg,
                                    result_status, objective_result, objective_details, max_resources
                                )
                        
                        # Save state
                        enhanced_metrics = csynth_data.copy()
                        enhanced_metrics["objective_result"] = objective_result.value if hasattr(objective_result, 'value') else str(objective_result)
                        enhanced_metrics["objective_details"] = objective_details

                        self.save_state(output_dir, iteration, new_cfg, result_status,
                                      enhanced_metrics, bottleneck_module, affecting_indices, focus_pragma_idx,
                                      level_heap, global_best_cfg, global_best_score,
                                      current_node, deepest_level, focus_pragma_idx, explored_options_for_this_node)
                    else:
                        # Determine reason for missing files
                        if not analysis_success:
                            # Analysis threw an exception (e.g., parsing error)
                            status_reason = "unanalyzable"
                            if self.logger:
                                self.logger.print(f"⚠️  Synthesis succeeded but analysis failed for iteration {iteration}")
                        else:
                            # Files missing for other reasons
                            status_reason = "missing_files"
                            if self.logger:
                                self.logger.print(f"❌ Missing analysis files for iteration {iteration}")

                        # Failed analysis - prune this node (do not add to next level)
                        score = self._get_failure_score(status_reason)  # Mark as failure
                        bottleneck_module = "unknown"
                        affecting_indices = []

                        if self.logger:
                            self.logger.print(f"🚫 Pruning failed analysis node: score={score}, will not expand further")
                        
                        # Create temporary node for logging purposes only (not added to heap)
                        new_tuned = current_node["tuned"] + [(focus_pragma_idx, option)]
                        new_node = {
                            "cfg": new_cfg,
                            "tuned": new_tuned,
                            "next_focus": None,  # No next focus since node is pruned
                            "score": score,
                            "bottleneck_module": bottleneck_module,
                            "ordered_pragmas": []
                        }
                        
                        # Save LevelHeap trace for failed analysis case
                        if self.logger:
                            self.logger.save_levelheap_dfs_trace(
                                iteration, work_dir, "unknown", [], None,
                                self.pragma_locations, new_cfg,
                                level_heap, new_node, global_best_score
                            )

                            # Save iteration data to final_trace.json (failed analysis case)
                            self.logger.save_iteration_trace(
                                iteration, self.pragma_locations, new_cfg,
                                status_reason, None, {}, {}
                            )

                        self.save_state(output_dir, iteration, new_cfg, status_reason,
                                      {}, bottleneck_module, affecting_indices, focus_pragma_idx,
                                      level_heap, global_best_cfg, global_best_score,
                                      current_node, deepest_level, focus_pragma_idx, explored_options_for_this_node)
                else:
                    if self.logger:
                        self.logger.print(f"❌ Synthesis failed: {result_status}")
                    
                    # Failed synthesis - prune this node (do not add to next level)
                    score = self._get_failure_score(result_status)  # Mark as failure
                    bottleneck_module = "synthesis_failed"
                    affecting_indices = []
                    
                    if self.logger:
                        self.logger.print(f"🚫 Pruning failed synthesis node: score={score}, will not expand further")
                    
                    # Create temporary node for logging purposes only (not added to heap)
                    new_tuned = current_node["tuned"] + [(focus_pragma_idx, option)]
                    new_node = {
                        "cfg": new_cfg,
                        "tuned": new_tuned,
                        "next_focus": None,  # No next focus since node is pruned
                        "score": score,
                        "bottleneck_module": bottleneck_module,
                        "ordered_pragmas": []
                    }
                    
                    # Save LevelHeap trace for synthesis failure case
                    if self.logger:
                        self.logger.save_levelheap_dfs_trace(
                            iteration, work_dir, "synthesis_failed", [], None,
                            self.pragma_locations, new_cfg,
                            level_heap, new_node, global_best_score
                        )
                        
                        # Save iteration data to final_trace.json (synthesis failure case)
                        self.logger.save_iteration_trace(
                            iteration, self.pragma_locations, new_cfg,
                            result_status, None, {}, {}
                        )
                    
                    self.save_state(output_dir, iteration, new_cfg, result_status,
                                  {}, bottleneck_module, affecting_indices, focus_pragma_idx,
                                  level_heap, global_best_cfg, global_best_score,
                                  current_node, deepest_level, focus_pragma_idx, explored_options_for_this_node)
                
                # Move to next iteration (only increment for actual synthesis runs, not skipped ones)
                iteration += 1
                if not skip_synthesis:
                    completed_iterations += 1
                    if self.logger:
                        self.logger.print(f"[DEBUG] Incremented iteration to {iteration}, completed_iterations={completed_iterations}")
                else:
                    if self.logger:
                        self.logger.print(f"[DEBUG] Incremented iteration to {iteration}, completed_iterations unchanged (synthesis skipped)")

            # After exploring options, log completion status
            # Note: We DON'T re-insert the node to the heap
            # - If interrupted: node state is saved (current_node + explored_options), will be used directly on resume
            # - If complete: node is exhausted, DFS continues with children

            remaining_options = [opt for opt in pragma_options if opt not in explored_options_for_this_node]

            if self.logger:
                if not remaining_options:
                    self.logger.print(f"[LevelHeap] All options explored for this node - node expansion complete")
                elif explored_options_for_this_node:
                    self.logger.print(f"[LevelHeap] Explored {len(explored_options_for_this_node)} option(s), continuing DFS with children")
                else:
                    self.logger.print(f"[LevelHeap] No options were explored this iteration")

            # Log updated heap state after processing all options for current node
            if self.logger:
                self.logger.log_heap_level_update(level_heap)
        
        # Calculate final exploration time
        total_exploration_time = time.time() - start_time
        
        if self.logger:
            self.logger.print(f"\n[LevelHeap DFS] Exploration completed")
            self.logger.print(f"[LevelHeap DFS] Total exploration time: {total_exploration_time/3600:.2f} hours")
            self.logger.print(f"[LevelHeap DFS] Total iterations: {len(self.history)}")
            self.logger.print(f"[LevelHeap DFS] Global best score: {global_best_score}")
            self.logger.print(f"[LevelHeap DFS] Global best config: {global_best_cfg}")
            self.logger.print(f"[LevelHeap DFS] Final heap state: {len(level_heap)} levels")
            for level, nodes in level_heap.items():
                self.logger.print(f"  Level {level}: {len(nodes)} nodes")
        else:
            print(f"\n[LevelHeap DFS] Exploration completed: {len(self.history)} iterations")
            print(f"[LevelHeap DFS] Total exploration time: {total_exploration_time/3600:.2f} hours")
        
        return self.history