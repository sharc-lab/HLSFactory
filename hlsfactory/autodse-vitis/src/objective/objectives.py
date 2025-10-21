#!/usr/bin/env python3
"""
Objective functions for Design Space Exploration (DSE).
This file defines various objective functions that can be used to guide DSE exploration.
"""

from abc import ABC, abstractmethod
from typing import Dict, Any, Tuple, Optional, TYPE_CHECKING
from enum import Enum

if TYPE_CHECKING:
    from design_space.logger import TeeLogger


class ObjectiveResult(Enum):
    """Result of objective evaluation."""
    SUCCESS = "success"           # Objective achieved
    NEEDS_IMPROVEMENT = "needs_improvement"  # Objective not met, keep exploring
    INFEASIBLE = "infeasible"     # Design is infeasible (e.g., resource violations)


class ObjectiveFunction(ABC):
    """
    Abstract base class for DSE objective functions.

    An objective function evaluates a design's performance metrics and determines:
    1. Whether the objective is achieved (SUCCESS)
    2. Whether the design should be improved (NEEDS_IMPROVEMENT)
    3. Whether the design is infeasible (INFEASIBLE)

    It also provides guidance on which modules/pragmas to focus on for improvement.
    """

    def __init__(self):
        """Initialize objective function."""
        self.logger: Optional['TeeLogger'] = None

    def set_logger(self, logger: 'TeeLogger'):
        """Set logger for detailed objective evaluation logging."""
        self.logger = logger

    def _log(self, message: str):
        """Log message using logger if available, otherwise print."""
        if self.logger:
            self.logger.print(message)
        else:
            print(message)

    @staticmethod
    def _safe_int(value, default=0) -> int:
        """
        Safely convert a value to int, handling 'undef', '-', and other non-numeric values.

        Args:
            value: Value to convert (can be int, str, or None)
            default: Default value to return if conversion fails

        Returns:
            Integer value or default if conversion not possible
        """
        if value is None:
            return default
        if isinstance(value, int):
            return value
        if isinstance(value, str):
            value = value.strip()
            if value.lower() in ('undef', '-', '', 'none'):
                return default
            try:
                return int(value)
            except (ValueError, TypeError):
                return default
        try:
            return int(value)
        except (ValueError, TypeError):
            return default
    
    @abstractmethod
    def evaluate(self, metrics: Dict[str, Any], rtl_data: Dict[str, Any]) -> Tuple[ObjectiveResult, Dict[str, Any]]:
        """
        Evaluate the objective function against design metrics.
        
        Args:
            metrics: Performance metrics from csynth_module_info.json
            rtl_data: RTL hierarchy and mapping data from rtl_c_annotated.json
            
        Returns:
            Tuple of (result, details) where:
            - result: ObjectiveResult indicating success/needs_improvement/infeasible
            - details: Dict with additional information like bottleneck_module, score, etc.
        """
        pass
    
    @abstractmethod
    def get_target_description(self) -> str:
        """Return a human-readable description of the objective."""
        pass


class TargetIIObjective(ObjectiveFunction):
    """
    Objective: Achieve target initiation interval (II) for top module.
    This is the original hardcoded objective.
    """
    
    def __init__(self, target_ii: int, top_module: str = "FFT_TOP"):
        """
        Initialize target II objective.
        
        Args:
            target_ii: Target initiation interval to achieve
            top_module: Name of the top module to check
        """
        super().__init__()
        self.target_ii = target_ii
        self.top_module = top_module
    
    def evaluate(self, metrics: Dict[str, Any], rtl_data: Dict[str, Any]) -> Tuple[ObjectiveResult, Dict[str, Any]]:
        """Evaluate whether target II is achieved."""
        details = {
            "target_ii": self.target_ii,
            "top_module": self.top_module
        }
        
        # Extract top module II from metrics
        if "metrics" in metrics and self.top_module in metrics["metrics"]:
            top_metrics = metrics["metrics"][self.top_module]
            ii_value = top_metrics.get("PipelineInitiationInterval", float('inf'))
            # Use _safe_int but allow float('inf') as fallback for undefined II
            if ii_value is None or (isinstance(ii_value, str) and ii_value.lower() in ('undef', '-', '')):
                current_ii = float('inf')
            else:
                try:
                    current_ii = int(ii_value) if not isinstance(ii_value, float) else ii_value
                except (ValueError, TypeError):
                    current_ii = float('inf')
            details["current_ii"] = current_ii

            if current_ii <= self.target_ii:
                details["bottleneck_module"] = self.top_module
                return ObjectiveResult.SUCCESS, details
            else:
                # Find bottleneck module using existing logic
                details["bottleneck_module"] = self._find_bottleneck_module(metrics, rtl_data)
                return ObjectiveResult.NEEDS_IMPROVEMENT, details
        else:
            details["current_ii"] = None
            details["bottleneck_module"] = self.top_module
            return ObjectiveResult.INFEASIBLE, details
    
    def _find_bottleneck_module(self, metrics: Dict[str, Any], rtl_data: Dict[str, Any]) -> str:
        """Find the bottleneck module using existing DSE logic."""
        self._log(f"[TargetIIObjective] Finding bottleneck with target II = {self.target_ii}")
        
        # Check if we have the required data structures (same error handling as original)
        if "rtl_hierarchy" not in rtl_data or "modules" not in rtl_data:
            self._log("❌ Required data structures not found in RTL hierarchy")
            return rtl_data.get("top_module", {}).get("ModuleName", self.top_module)
        
        # Start from the hierarchical tree root
        hierarchy_tree = rtl_data["rtl_hierarchy"]
        modules_performance = rtl_data["modules"]
        
        bottleneck_module = self._find_worst_module_recursive(hierarchy_tree, modules_performance, self.target_ii)
        
        self._log(f"[TargetIIObjective] Identified bottleneck module: {bottleneck_module}")
        return bottleneck_module
    
    def _find_worst_module_recursive(self, hierarchy_node: Dict, modules_performance: Dict, target_ii: int) -> str:
        """Recursively find the worst (highest II) module in the hierarchy."""
        module_name = hierarchy_node.get("ModuleName", "")
        
        # Get current module's II from performance data
        current_ii = 0
        if module_name in modules_performance:
            ii_value = modules_performance[module_name].get("performance", {}).get("PipelineInitiationInterval", 0)
            current_ii = self._safe_int(ii_value, 0)
        
        self._log(f"[TargetIIObjective] Checking module {module_name}: II = {current_ii}")
        
        # Check if current module meets target
        if current_ii <= target_ii:
            self._log(f"[TargetIIObjective] Module {module_name} meets target II ({current_ii} <= {target_ii})")
            return module_name  # Good enough, no need to go deeper
        
        # Get children from the hierarchy tree
        children = hierarchy_node.get("Children", [])
        
        if not children:
            # Leaf module - this is our bottleneck
            self._log(f"[TargetIIObjective] Leaf bottleneck found: {module_name} (II = {current_ii})")
            return module_name
        
        # Check if all children meet the target II
        all_children_meet_target = True
        worst_child_node = None
        worst_ii = 0
        
        for child_node in children:
            child_name = child_node.get("ModuleName", "")
            if child_name in modules_performance:
                ii_value = modules_performance[child_name].get("performance", {}).get("PipelineInitiationInterval", 0)
                child_ii = self._safe_int(ii_value, 0)
                if child_ii > target_ii:
                    all_children_meet_target = False
                if child_ii > worst_ii:
                    worst_ii = child_ii
                    worst_child_node = child_node
        
        # If parent doesn't meet goal but all children do, return parent
        if all_children_meet_target and current_ii > target_ii:
            self._log(f"[TargetIIObjective] Parent module {module_name} doesn't meet target but all children do - returning parent as bottleneck")
            return module_name
        
        if worst_child_node:
            # Recursively examine the worst child
            return self._find_worst_module_recursive(worst_child_node, modules_performance, target_ii)
        else:
            # No children with metrics, return current module
            return module_name
    
    def get_target_description(self) -> str:
        """Return description of the target II objective."""
        return f"Achieve target II ≤ {self.target_ii} for {self.top_module}"


class ResourceConstrainedLatencyObjective(ObjectiveFunction):
    """
    Objective: Minimize latency while staying within resource constraints.
    """
    
    def __init__(self, max_dsp: int, max_bram: int, max_lut: int, max_ff: int = None, max_uram: int = None, top_module: str = "FFT_TOP"):
        """
        Initialize resource-constrained latency objective.
        
        Args:
            max_dsp: Maximum DSP usage allowed
            max_bram: Maximum BRAM usage allowed  
            max_lut: Maximum LUT usage allowed
            max_ff: Maximum FF usage allowed (optional)
            max_uram: Maximum URAM usage allowed (optional)
            top_module: Name of the top module to optimize
        """
        super().__init__()
        self.max_dsp = max_dsp
        self.max_bram = max_bram
        self.max_lut = max_lut
        self.max_ff = max_ff
        self.max_uram = max_uram
        self.top_module = top_module
    
    def evaluate(self, metrics: Dict[str, Any], rtl_data: Dict[str, Any]) -> Tuple[ObjectiveResult, Dict[str, Any]]:
        """Evaluate resource constraints and latency."""
        details = {
            "max_dsp": self.max_dsp,
            "max_bram": self.max_bram,
            "max_lut": self.max_lut,
            "max_ff": self.max_ff,
            "max_uram": self.max_uram,
            "top_module": self.top_module
        }
        
        # Extract top module metrics
        if "metrics" in metrics and self.top_module in metrics["metrics"]:
            top_metrics = metrics["metrics"][self.top_module]
            
            # Get resource usage
            dsp_used = top_metrics.get("DSP", 0)
            bram_used = top_metrics.get("BRAM_18K", 0)
            lut_used = top_metrics.get("LUT", 0)
            ff_used = top_metrics.get("FF", 0)
            uram_used = top_metrics.get("URAM", 0)
            latency_raw = top_metrics.get("Worst-caseLatency", float('inf'))
            # Handle 'undef' or other non-numeric latency values (treat as worst case)
            latency = self._safe_int(latency_raw, float('inf'))
            
            details.update({
                "dsp_used": dsp_used,
                "bram_used": bram_used, 
                "lut_used": lut_used,
                "ff_used": ff_used,
                "uram_used": uram_used,
                "latency": latency
            })
            
            # Check resource constraints
            resource_violations = []
            if dsp_used > self.max_dsp:
                resource_violations.append(f"DSP: {dsp_used} > {self.max_dsp}")
            if bram_used > self.max_bram:
                resource_violations.append(f"BRAM: {bram_used} > {self.max_bram}")
            if lut_used > self.max_lut:
                resource_violations.append(f"LUT: {lut_used} > {self.max_lut}")
            if self.max_ff is not None and ff_used > self.max_ff:
                resource_violations.append(f"FF: {ff_used} > {self.max_ff}")
            if self.max_uram is not None and uram_used > self.max_uram:
                resource_violations.append(f"URAM: {uram_used} > {self.max_uram}")
            
            details["resource_violations"] = resource_violations
            
            if resource_violations:
                # Resource constraints violated
                details["bottleneck_module"] = self._find_resource_bottleneck(metrics, rtl_data)
                return ObjectiveResult.INFEASIBLE, details
            else:
                # Resources OK, focus on latency improvement
                details["bottleneck_module"] = self._find_latency_bottleneck(metrics, rtl_data)
                # Always return NEEDS_IMPROVEMENT since we want to minimize latency
                return ObjectiveResult.NEEDS_IMPROVEMENT, details
        else:
            details["bottleneck_module"] = self.top_module
            return ObjectiveResult.INFEASIBLE, details
    
    def _find_resource_bottleneck(self, metrics: Dict[str, Any], rtl_data: Dict[str, Any]) -> str:
        """Find module consuming most resources."""
        # Find bottleneck using latency-based hierarchy analysis similar to TargetIIObjective
        return self._find_latency_bottleneck_recursive(metrics, rtl_data)
    
    def _find_latency_bottleneck(self, metrics: Dict[str, Any], rtl_data: Dict[str, Any]) -> str:
        """Find module contributing most to latency."""
        return self._find_latency_bottleneck_recursive(metrics, rtl_data)
    
    def _find_latency_bottleneck_recursive(self, metrics: Dict[str, Any], rtl_data: Dict[str, Any]) -> str:
        """Find the module with highest latency using hierarchy traversal."""
        if "rtl_hierarchy" not in rtl_data or "modules" not in rtl_data:
            self._log("❌ Required data structures not found in RTL hierarchy")
            return rtl_data.get("top_module", {}).get("ModuleName", self.top_module)
        
        # Start from the hierarchical tree root
        hierarchy_tree = rtl_data["rtl_hierarchy"]
        modules_performance = rtl_data["modules"]
        
        return self._find_worst_latency_module_recursive(hierarchy_tree, modules_performance)
    
    def _find_worst_latency_module_recursive(self, hierarchy_node: Dict, modules_performance: Dict) -> str:
        """Recursively find the worst (highest latency) module in the hierarchy."""
        module_name = hierarchy_node.get("ModuleName", "")

        # Get current module's latency from performance data
        current_latency = 0
        if module_name in modules_performance:
            lat_value = modules_performance[module_name].get("performance", {}).get("Worst-caseLatency", 0)
            current_latency = self._safe_int(lat_value, 0)
        
        self._log(f"[ResourceConstrainedLatencyObjective] Checking module {module_name}: Latency = {current_latency}")
        
        # Get children from the hierarchy tree
        children = hierarchy_node.get("Children", [])
        
        if not children:
            # Leaf module - return this module
            self._log(f"[ResourceConstrainedLatencyObjective] Leaf bottleneck found: {module_name} (Latency = {current_latency})")
            return module_name
        
        # Find child with worst latency
        worst_child_node = None
        worst_latency = 0
        
        for child_node in children:
            child_name = child_node.get("ModuleName", "")
            if child_name in modules_performance:
                lat_value = modules_performance[child_name].get("performance", {}).get("Worst-caseLatency", 0)
                child_latency = self._safe_int(lat_value, 0)
                if child_latency > worst_latency:
                    worst_latency = child_latency
                    worst_child_node = child_node
        
        if worst_child_node:
            # Recursively examine the worst child, as it's the highest contributor at this level.
            return self._find_worst_latency_module_recursive(worst_child_node, modules_performance)
        else:
            # This is a leaf node or no children have metrics, so it's the bottleneck.
            return module_name
    
    def get_target_description(self) -> str:
        """Return description of the resource-constrained latency objective."""
        constraints = [f"DSP≤{self.max_dsp}", f"BRAM≤{self.max_bram}", f"LUT≤{self.max_lut}"]
        if self.max_ff is not None:
            constraints.append(f"FF≤{self.max_ff}")
        if self.max_uram is not None:
            constraints.append(f"URAM≤{self.max_uram}")
        return f"Minimize latency within resource limits ({', '.join(constraints)})"


class ResourceConstrainedIILatencyObjective(ObjectiveFunction):
    """
    Objective: Minimize both II and latency while staying within resource constraints.
    """
    
    def __init__(self, max_dsp: int, max_bram: int, max_lut: int, max_ff: int = None, max_uram: int = None,
                 ii_weight: float = 0.6, latency_weight: float = 0.4, top_module: str = "FFT_TOP"):
        """
        Initialize resource-constrained II+latency objective.
        
        Args:
            max_dsp: Maximum DSP usage allowed
            max_bram: Maximum BRAM usage allowed
            max_lut: Maximum LUT usage allowed
            max_ff: Maximum FF usage allowed (optional)
            max_uram: Maximum URAM usage allowed (optional)
            ii_weight: Weight for II in combined objective (0-1)
            latency_weight: Weight for latency in combined objective (0-1)
            top_module: Name of the top module to optimize
        """
        super().__init__()
        self.max_dsp = max_dsp
        self.max_bram = max_bram
        self.max_lut = max_lut
        self.max_ff = max_ff
        self.max_uram = max_uram
        self.ii_weight = ii_weight
        self.latency_weight = latency_weight
        self.top_module = top_module
        
        # Normalize weights
        total_weight = ii_weight + latency_weight
        if total_weight > 0:
            self.ii_weight = ii_weight / total_weight
            self.latency_weight = latency_weight / total_weight
    
    def evaluate(self, metrics: Dict[str, Any], rtl_data: Dict[str, Any]) -> Tuple[ObjectiveResult, Dict[str, Any]]:
        """Evaluate resource constraints and combined II+latency score."""
        details = {
            "max_dsp": self.max_dsp,
            "max_bram": self.max_bram,
            "max_lut": self.max_lut,
            "max_ff": self.max_ff,
            "max_uram": self.max_uram,
            "ii_weight": self.ii_weight,
            "latency_weight": self.latency_weight,
            "top_module": self.top_module
        }
        
        # Extract top module metrics
        if "metrics" in metrics and self.top_module in metrics["metrics"]:
            top_metrics = metrics["metrics"][self.top_module]
            
            # Get resource usage and performance
            dsp_used = top_metrics.get("DSP", 0)
            bram_used = top_metrics.get("BRAM_18K", 0)
            lut_used = top_metrics.get("LUT", 0)
            ff_used = top_metrics.get("FF", 0)
            uram_used = top_metrics.get("URAM", 0)

            # Handle II value safely
            ii_value = top_metrics.get("PipelineInitiationInterval", float('inf'))
            if ii_value is None or (isinstance(ii_value, str) and ii_value.lower() in ('undef', '-', '')):
                ii = float('inf')
            else:
                try:
                    ii = int(ii_value) if not isinstance(ii_value, float) else ii_value
                except (ValueError, TypeError):
                    ii = float('inf')

            # Handle latency value safely
            latency_value = top_metrics.get("PipelineLatency", float('inf'))
            if latency_value is None or (isinstance(latency_value, str) and latency_value.lower() in ('undef', '-', '')):
                latency = float('inf')
            else:
                try:
                    latency = int(latency_value) if not isinstance(latency_value, float) else latency_value
                except (ValueError, TypeError):
                    latency = float('inf')
            
            details.update({
                "dsp_used": dsp_used,
                "bram_used": bram_used,
                "lut_used": lut_used,
                "ff_used": ff_used,
                "uram_used": uram_used,
                "ii": ii,
                "latency": latency
            })
            
            # Check resource constraints
            resource_violations = []
            if dsp_used > self.max_dsp:
                resource_violations.append(f"DSP: {dsp_used} > {self.max_dsp}")
            if bram_used > self.max_bram:
                resource_violations.append(f"BRAM: {bram_used} > {self.max_bram}")
            if lut_used > self.max_lut:
                resource_violations.append(f"LUT: {lut_used} > {self.max_lut}")
            if self.max_ff is not None and ff_used > self.max_ff:
                resource_violations.append(f"FF: {ff_used} > {self.max_ff}")
            if self.max_uram is not None and uram_used > self.max_uram:
                resource_violations.append(f"URAM: {uram_used} > {self.max_uram}")
            
            details["resource_violations"] = resource_violations
            
            if resource_violations:
                # Resource constraints violated
                details["bottleneck_module"] = self._find_resource_bottleneck(metrics, rtl_data)
                return ObjectiveResult.INFEASIBLE, details
            else:
                # Calculate combined score (lower is better)
                # Normalize II and latency scores (simple approach - could be enhanced)
                combined_score = self.ii_weight * ii + self.latency_weight * latency
                details["combined_score"] = combined_score
                
                # Always return NEEDS_IMPROVEMENT since we want to minimize both
                details["bottleneck_module"] = self._find_performance_bottleneck(metrics, rtl_data, ii, latency)
                return ObjectiveResult.NEEDS_IMPROVEMENT, details
        else:
            details["bottleneck_module"] = self.top_module
            return ObjectiveResult.INFEASIBLE, details
    
    def _find_resource_bottleneck(self, metrics: Dict[str, Any], rtl_data: Dict[str, Any]) -> str:
        """Find module consuming most resources."""
        return self.top_module
    
    def _find_performance_bottleneck(self, metrics: Dict[str, Any], rtl_data: Dict[str, Any], 
                                   ii: float, latency: float) -> str:
        """Find module limiting performance based on II and latency weights."""
        # For simplicity, focus on II bottleneck if II weight is higher, otherwise latency
        if self.ii_weight >= self.latency_weight:
            # Focus on II bottleneck (use existing logic similar to TargetIIObjective)
            return self._find_ii_bottleneck(metrics, rtl_data)
        else:
            # Focus on latency bottleneck
            return self._find_latency_bottleneck(metrics, rtl_data)
    
    def _find_ii_bottleneck(self, metrics: Dict[str, Any], rtl_data: Dict[str, Any]) -> str:
        """Find II bottleneck module."""
        return self.top_module
    
    def _find_latency_bottleneck(self, metrics: Dict[str, Any], rtl_data: Dict[str, Any]) -> str:
        """Find latency bottleneck module."""
        return self.top_module
    
    def get_target_description(self) -> str:
        """Return description of the resource-constrained II+latency objective."""
        constraints = [f"DSP≤{self.max_dsp}", f"BRAM≤{self.max_bram}", f"LUT≤{self.max_lut}"]
        if self.max_ff is not None:
            constraints.append(f"FF≤{self.max_ff}")
        if self.max_uram is not None:
            constraints.append(f"URAM≤{self.max_uram}")
        return f"Minimize II ({self.ii_weight:.1%}) + latency ({self.latency_weight:.1%}) within resource limits ({', '.join(constraints)})"
