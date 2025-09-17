import os
import xml.etree.ElementTree as ET
import json
import shutil
import glob
import re
import subprocess
from pathlib import Path
import sys

from .llvm_ir_parser import LLVMIRParser

# Import TeeLogger - handle import gracefully
try:
    from ..design_space.logger import TeeLogger
except ImportError:
    TeeLogger = None

class CSynthAnalyzer:
    """
    Parses a Vitis HLS csynth report directory by auto-locating the HLS project
    under a given work_dir. It finds a folder containing 'hls.app', then a subfolder
    with both 'syn/' and '.autopilot/', and reads its csynth_report/csynth.xml.
    """
    def __init__(self, work_dir):
        # Logger will be set externally - initialize first
        self.logger = None
        
        self.work_dir = work_dir
        self.log_dir = os.path.join(work_dir, "logs")
        if not os.path.exists(self.log_dir):
            os.makedirs(self.log_dir)
        self.csynth_report_dir = self.find_csynth_report_dir(work_dir)
        self.xml_path = os.path.join(self.csynth_report_dir, "syn", "report", "csynth.xml")
        self.csynth_module_info = {}
        self.transform_messages = {}
        self.llvm_ir_hierarchy = None
    
    def set_logger(self, logger):
        """Set the logger for this analyzer."""
        self.logger = logger

    def find_csynth_report_dir(self, work_dir):
        """
        Searches work_dir/* for a project folder containing 'hls.app',
        then within that project scans subfolders to find one with 'syn/'
        and '.autopilot/'. Returns the path to its 'csynth_report' folder.
        """
        for proj in os.scandir(work_dir):
            if not proj.is_dir():
                continue
            # Look for hls.app marker
            if 'hls.app' not in os.listdir(proj.path):
                continue
            # Within this project, find subfolder with syn/ and .autopilot/
            for sub in os.scandir(proj.path):
                if not sub.is_dir():
                    continue
                entries = os.listdir(sub.path)
                if self.logger:
                    self.logger.print(f"Checking subfolder: {sub.path} with entries: {entries}")
                else:
                    print(f"Checking subfolder: {sub.path} with entries: {entries}")
                if 'syn' in entries and '.autopilot' in entries:
                    # csynth_report is under .autopilot/csynth_report
                    return os.path.join(sub.path)
        raise FileNotFoundError(
            f"Could not locate csynth_report under '{work_dir}': "
            "ensure a project folder with 'hls.app' exists and contains a subfolder "
            "with both 'syn/' and '.autopilot/'"
        )

    def parse_hierarchy_instance(self, instance_elem):
        """
        Recursively parse an <Instance> element and return a dictionary
        with keys: 'InstName', 'ModuleName', and 'Children'.
        """
        node = {}
        inst_name_elem = instance_elem.find("InstName")
        module_name_elem = instance_elem.find("ModuleName")
        node["InstName"] = inst_name_elem.text.strip() if inst_name_elem is not None else "UnknownInst"
        node["ModuleName"] = module_name_elem.text.strip() if module_name_elem is not None else "UnknownModule"
        
        # Recursively parse children under InstancesList
        children = []
        instances_list_elem = instance_elem.find("InstancesList")
        if instances_list_elem is not None:
            for child in instances_list_elem.findall("Instance"):
                children.append(self.parse_hierarchy_instance(child))
        node["Children"] = children
        return node

    def parse_top_module_hierarchy(self, rtl_design_hierarchy_elem):
        """
        Parse the top module hierarchy from <RTLDesignHierarchy>.
        Returns a tree with the top module and its children.
        """
        top_module_elem = rtl_design_hierarchy_elem.find("TopModule")
        if top_module_elem is None:
            return None
        node = {}
        module_name_elem = top_module_elem.find("ModuleName")
        node["ModuleName"] = module_name_elem.text.strip() if module_name_elem is not None else "UnknownTopModule"
        node["Attributes"] = top_module_elem.attrib  # e.g., MAX_DEPTH
        children = []
        instances_list_elem = top_module_elem.find("InstancesList")
        if instances_list_elem is not None:
            for inst in instances_list_elem.findall("Instance"):
                children.append(self.parse_hierarchy_instance(inst))
        node["Children"] = children
        return node

    def parse_error_log(self, log_path):
        """
        Parse the synthesis log file and extract error messages.
        Returns a list of error lines.
        """
        if not os.path.exists(log_path):
            return ["Error log not found."]
        
        errors = []
        with open(log_path, 'r') as f:
            for line in f:
                if line.startswith("ERROR:"):
                    errors.append(line.strip())
        return errors

    def parse_csynth_report(self):
        """
        Parses csynth.xml to extract performance estimates and resource utilization.
        Populates self.csynth_module_info and returns it.
        """
        if not os.path.isfile(self.xml_path):
            error_lines = self.parse_error_log(os.path.join(self.csynth_report_dir, "solution.log"))
            self.csynth_module_info = {
                'Error': 'csynth.xml not found',
                'Details': error_lines,
            }
            return self.csynth_module_info


        tree = ET.parse(self.xml_path)
        root = tree.getroot()

        # Extract top module information and hierarchy from RTLDesignHierarchy
        top_module = {}
        hierarchy = None
        rtl_design_hierarchy = root.find('RTLDesignHierarchy')
        if rtl_design_hierarchy is not None:
            top_elem = rtl_design_hierarchy.find('TopModule')
            if top_elem is not None:
                mod_name_elem = top_elem.find('ModuleName')
                if mod_name_elem is not None:
                    top_module['ModuleName'] = mod_name_elem.text.strip()
                top_module['MAX_DEPTH'] = top_elem.attrib.get('MAX_DEPTH', 'N/A')
            else:
                top_module['ModuleName'] = "Unknown"
            # Parse the hierarchy tree
            hierarchy = self.parse_top_module_hierarchy(rtl_design_hierarchy)
        else:
            top_module['ModuleName'] = "Unknown"


        module_metrics = {}
        # Extract module-level metrics from ModuleInformation
        mod_info_elem = root.find('ModuleInformation')
        if mod_info_elem is None:
            if self.logger:
                self.logger.print("No ModuleInformation found in the XML.")
            else:
                print("No ModuleInformation found in the XML.")
            return top_module, module_metrics, hierarchy

        # First pass: collect all modules and their basic information
        all_modules = {}
        loop_to_instance_map = {}
        
        for module in mod_info_elem.findall('Module'):
            name_elem = module.find('Name')
            if name_elem is None:
                continue
            module_name = name_elem.text.strip()
            metrics = {}

            # Extract Performance Estimates: Overall Latency
            perf_elem = module.find('PerformanceEstimates')
            if perf_elem is not None:
                overall_lat = perf_elem.find('./SummaryOfOverallLatency')
                if overall_lat is not None:
                    best_lat = overall_lat.find('Best-caseLatency')
                    avg_lat = overall_lat.find('Average-caseLatency')
                    worst_lat = overall_lat.find('Worst-caseLatency')
                    ii = overall_lat.find('PipelineInitiationInterval')
                    metrics['Best-caseLatency'] = best_lat.text.strip() if best_lat is not None else None
                    metrics['Average-caseLatency'] = avg_lat.text.strip() if avg_lat is not None else None
                    metrics['Worst-caseLatency'] = worst_lat.text.strip() if worst_lat is not None else None
                    # metrics['PipelineInitiationInterval'] = ii.text.strip() if ii is not None else None
                    # if ii is a range, such as 8526 ~ 8910, choose the max one
                    metrics['PipelineInitiationInterval'] = ( max(map(int, ii.text.strip().split('~'))) if ii is not None and ii.text else None )
                
                # Collect loop-to-instance mappings for child modules
                loop_lat_elem = perf_elem.find('./SummaryOfLoopLatency')
                if loop_lat_elem is not None:
                    loops_info = []
                    for loop_elem in loop_lat_elem:
                        loop_id = loop_elem.tag  # e.g., "Loop1", "Loop2", etc.
                        loop_name_elem = loop_elem.find('Name')
                        loop_display_name = loop_name_elem.text.strip() if loop_name_elem is not None else loop_id
                        
                        # Get instances referenced by this loop
                        instance_list_elem = loop_elem.find('InstanceList')
                        loop_instances = []
                        if instance_list_elem is not None:
                            for instance_elem in instance_list_elem.findall('Instance'):
                                if instance_elem.text:
                                    loop_instances.append(instance_elem.text.strip())
                        
                        loop_info = {
                            'loop_id': loop_id,
                            'loop_name': loop_display_name,
                            'instances': loop_instances
                        }
                        
                        # Extract loop-specific metrics
                        slack = loop_elem.find('Slack')
                        trip_count = loop_elem.find('TripCount')
                        latency = loop_elem.find('Latency')
                        abs_time_lat = loop_elem.find('AbsoluteTimeLatency')
                        iter_lat = loop_elem.find('IterationLatency')
                        pipeline_ii = loop_elem.find('PipelineII')
                        pipeline_depth = loop_elem.find('PipelineDepth')
                        pipeline_type = loop_elem.find('PipelineType')
                        
                        loop_info.update({
                            'Slack': slack.text.strip() if slack is not None else None,
                            'TripCount': trip_count.text.strip() if trip_count is not None else None,
                            'Loop-Best-caseLatency': latency.text.strip() if latency is not None else None,
                            'AbsoluteTimeLatency': abs_time_lat.text.strip() if abs_time_lat is not None else None,
                            'IterationLatency': iter_lat.text.strip() if iter_lat is not None else None,
                            'PipelineII': pipeline_ii.text.strip() if pipeline_ii is not None else None,
                            'PipelineDepth': pipeline_depth.text.strip() if pipeline_depth is not None else None,
                            'PipelineType': pipeline_type.text.strip() if pipeline_type is not None else None
                        })
                        
                        loops_info.append(loop_info)
                        
                        # Map loop instances to their parent module
                        for inst in loop_instances:
                            loop_to_instance_map[inst] = {
                                'parent_module': module_name,
                                'loop_info': loop_info
                            }
                    
                    metrics['loops'] = loops_info
                        
            # Extract Area Estimates: Resource Utilization
            area_elem = module.find('AreaEstimates')
            if area_elem is not None:
                res_elem = area_elem.find('Resources')
                if res_elem is not None:
                    bram = res_elem.find('BRAM_18K')
                    dsp = res_elem.find('DSP')
                    ff = res_elem.find('FF')
                    lut = res_elem.find('LUT')
                    uram = res_elem.find('URAM')
                    metrics['BRAM_18K'] = int(bram.text.strip()) if bram is not None and bram.text.strip().isdigit() else 0
                    metrics['DSP'] = int(dsp.text.strip()) if dsp is not None and dsp.text.strip().isdigit() else 0
                    metrics['FF'] = int(ff.text.strip()) if ff is not None and ff.text.strip().isdigit() else 0
                    metrics['LUT'] = int(lut.text.strip()) if lut is not None and lut.text.strip().isdigit() else 0
                    metrics['URAM'] = int(uram.text.strip()) if uram is not None and uram.text.strip().isdigit() else 0

            all_modules[module_name] = metrics
            
        # Second pass: identify which modules are loops (referenced by loop instances)
        # and mark them appropriately
        for module_name, metrics in all_modules.items():
            # Check if this module is referenced as a loop instance
            is_loop_module = False
            parent_module = None
            loop_info = None
            
            for inst_name, mapping in loop_to_instance_map.items():
                # Extract module name from instance name (remove _fu_XX suffix)
                inst_module_name = inst_name.split('_fu_')[0].replace('grp_', '')
                if inst_module_name == module_name:
                    is_loop_module = True
                    parent_module = mapping['parent_module']
                    loop_info = mapping['loop_info']
                    break
            
            if is_loop_module:
                metrics['Type'] = 'Loop'
                metrics['ParentModule'] = parent_module
                if loop_info:
                    # Add loop-specific performance data to the module
                    for key, value in loop_info.items():
                        if key not in ['instances', 'loop_id', 'loop_name']:
                            metrics[key] = value
                    metrics['LoopID'] = loop_info['loop_id']
                    metrics['LoopName'] = loop_info['loop_name']
            else:
                metrics['Type'] = 'Module'
            
            module_metrics[module_name] = metrics

        self.csynth_module_info['top_module'] = top_module
        self.csynth_module_info['metrics'] = module_metrics
        self.csynth_module_info['hierarchy'] = hierarchy
        return self.csynth_module_info

    def save_csynth_module_info_to_json(self, output_path=None):
        """
        Saves parsed csynth_module_info as JSON into the report directory.
        """
        if not self.csynth_module_info:
            raise ValueError("Metrics not parsed. Call parse_csynth_report() first.")
                
        if output_path is None:
            output_dir = os.path.join(self.log_dir)
            os.makedirs(output_dir, exist_ok=True)        
        output_path = os.path.join(output_dir, "csynth_module_info.json")
        
        with open(output_path, 'w') as f:
            json.dump(self.csynth_module_info, f, indent=2)
        if self.logger:
            self.logger.print(f"Hierarchy saved to {output_path}")
        else:
            print(f"Hierarchy saved to {output_path}")

    def parse_transform_messages(self):
        # look at 0_examples/0_concrete_design/build/.message_syn.xml for examples of messages
        """
        Parses message_syn.xml for HLS transform events:
          - Unrolling: captures loop label, loop location, function, completion flag, unroll factor, and function location.
          - Inlining: captures callee, caller, source location, target location (if arrow present).
          - Flattening: captures loop label, loop location, and function.
          - legalizing: captures original and legalized function names.
        Raises FileNotFoundError if message_syn.xml is missing.
        Returns a dict with lists under keys 'unrolling', 'inlining', 'flattening'.
        """
        # locate message_syn.xml 
        msg_file = os.path.join(self.csynth_report_dir, '.autopilot', 'db', '.message_syn.xml')
        if not os.path.isfile(msg_file):
            raise FileNotFoundError(f"message_syn.xml not found in '{msg_file}'")

        tree = ET.parse(msg_file)
        root = tree.getroot()

        unrolling = []
        inlining = []
        flattening = []
        legalizing = []
        unparsed = []


        # regex patterns
        ur_complete = re.compile(
            r"Unrolling loop '([^']+)' \(([^)]+)\) in function '([^']+)' completely with a factor of (\d+) \(([^)]+)\)")
        ur_partial = re.compile(
            r"Unrolling loop '([^']+)' \(([^)]+)\) in function '([^']+)' with a factor of (\d+) \(([^)]+)\)")
        inline_hls = re.compile(
            r"Inlining function '([^']+)' into '([^']+)' \(([^)]+)\)")
        inline_xform = re.compile(
            r"Inlining function '([^']+)' into '([^']+)' \(([^:]+):(\d+)->([^:]+):(\d+)\)")
        flatten_re = re.compile(
            r"Flattening (?:a loop nest )?'([^']+)' \(([^)]+)\) in function '([^']+)'")
        legal_re = re.compile(
            r"Legalizing function name '([^']+)' to '([^']+)'")

        for msg in root.findall('Message'):
            prefix = msg.get('prefix', '')
            content = msg.get('content', '')

            if '214-186' in prefix:
                m = ur_complete.search(content)
                if m:
                    loop_file, loop_line, loop_col = m.group(2).rsplit(':', 2)
                    func_file, func_line, func_col = m.group(5).rsplit(':', 2)
                    unrolling.append({
                        'loop': m.group(1),
                        'loop_loc': {'file': os.path.basename(loop_file), 'line': int(loop_line), 'column': int(loop_col)},
                        'func': m.group(3),
                        'complete': True,
                        'factor': int(m.group(4)),
                        'func_loc': {'file': os.path.basename(func_file), 'line': int(func_line), 'column': int(func_col)}
                    })
                else:
                    m2 = ur_partial.search(content)
                    if m2:
                        loop_file, loop_line, loop_col = m.group(2).rsplit(':', 2)
                        func_file, func_line, func_col = m.group(5).rsplit(':', 2)
                        unrolling.append({
                            'loop': m2.group(1),
                            'loop_loc': {'file': os.path.basename(loop_file), 'line': int(loop_line), 'column': int(loop_col)},
                            'func': m2.group(3),
                            'complete': False,
                            'factor': int(m2.group(4)),
                            'func_loc': {'file': os.path.basename(func_file), 'line': int(func_line), 'column': int(func_col)}
                        })
                    else: 
                        unparsed.append({'prefix': prefix,'content': content})
            elif '214-178' in prefix or 'XFORM 203-602' in prefix:
                mx = inline_xform.search(content)
                if mx:
                    inlining.append({
                        'callee': mx.group(1),
                        'callee_inovke_loc': {'file': mx.group(3), 'line': int(mx.group(4))},
                        'caller': mx.group(2),
                        'caller_inovke_loc': {'file': mx.group(5), 'line': int(mx.group(6))}
                    })
                else:
                    mh = inline_hls.search(content)
                    if mh:
                        raw_callee = mh.group(1)  
                        raw_caller = mh.group(2)
                        # drop everything from the first '('
                        callee_name = raw_callee.split('(')[0]
                         # caller may include a return type, e.g. "void FFT_stage..."; 
                        # split on '(' then take the last word before it
                        caller_before_params = raw_caller.split('(')[0].strip()
                        caller_name = caller_before_params.split()[-1]

                        inlining.append({
                            'callee': callee_name,
                            'caller': caller_name,
                            'caller_definition_loc_end_line': mh.group(3)
                        })
            elif 'XFORM 203-541' in prefix:
                mf = flatten_re.search(content)
                loop_file, loop_line, loop_col = mf.group(2).rsplit(':', 2)
                if mf:
                    flattening.append({
                        'loop': mf.group(1),
                        'loop_loc': {'file': os.path.basename(loop_file), 'line': int(loop_line), 'column': int(loop_col)},
                        'func': mf.group(3)
                    })
                else:
                    unparsed.append({'prefix': prefix,'content': content})
                # Legalizing events (SYN 201-103)
            elif '201-103' in prefix:
                ml = legal_re.search(content)
                if ml:
                    legalizing.append({
                        'original': ml.group(1),
                        'legalized': ml.group(2)
                    })
                else:
                    legalizing.append({'message': content})

        self.transform_messages = {
            'unrolling':  unrolling,
            'inlining':   inlining,
            'flattening': flattening,
            'legalizing': legalizing,
            'unparsed':   unparsed
        }
        # Save the transforms to a JSON file in the log directory
        return self.transform_messages

    def save_transform_messages_to_json(self, output_path=None):
        """
        Saves parsed transform messages as JSON into the report directory.
        Call parse_transform_messages() first to populate messages.
        """
        # Ensure messages have been parsed
        if not hasattr(self, 'transform_messages') or not self.transform_messages:
            raise ValueError("Transform messages not parsed. Call parse_transform_messages() first.")
        # Determine output directory and file
        if output_path is None:
            output_dir = self.log_dir
            os.makedirs(output_dir, exist_ok=True)        
        output_path = os.path.join(output_dir, "transform_messages.json")
        # Write JSON
        with open(output_path, 'w') as f:
            json.dump(self.transform_messages, f, indent=2)
        if self.logger:
            self.logger.print(f"Transform messages saved to {output_path}")
        else:
            print(f"Transform messages saved to {output_path}")

    def parse_llvm_ir_hierarchy(self):
        try:
            xilinx_hls = Path(os.environ["XILINX_HLS"])
            if self.logger:
                self.logger.print(f"Using XILINX_HLS: {xilinx_hls} opt tool")
            else:
                print(f"Using XILINX_HLS: {xilinx_hls} opt tool")
        except KeyError:
            sys.exit("❌  XILINX_HLS is not defined in your environment.")

        # opt_bin = xilinx_hls / "lnx64" / "tools" / "clang" / "bin" / "opt"
        # for 2024.1 "/tools/Xilinx/Vitis_HLS/2024.1/lnx64/tools/clang/bin/opt"
        # opt_bin = xilinx_hls / "lnx64" / "tools" / "clang" / "bin" / "opt" 
        # for 2024.2 "/tools/Xilinx/Vitis/2024.2/lnx64/tools/clang-3.1/bin/opt"

        opt_bin = xilinx_hls / "lnx64" / "tools" / "clang-3.1" / "bin" / "opt"
        lib_dir = xilinx_hls / "lib" / "lnx64.o"
        in_bc  = Path(self.csynth_report_dir, ".autopilot/db/a.o.3.bc")
        out_ll = Path(self.log_dir, "a.o.3.bc.ll")
        cmd = [str(opt_bin), "-S", str(in_bc), "-o", str(out_ll)]
        env = os.environ.copy()
        env["LD_LIBRARY_PATH"] = f"{lib_dir}:{env.get('LD_LIBRARY_PATH','')}"
        subprocess.run(cmd, env=env, check=True)

        self.llvm_ir_hierarchy = LLVMIRParser(out_ll, Path(self.log_dir, "input_c_hierarchy.json"))     # path to .ll file
        
        # Get the top module name from parsed csynth report
        top_module_name = "FFT_TOP"  # default fallback
        if hasattr(self, 'csynth_module_info') and self.csynth_module_info:
            if 'top_module' in self.csynth_module_info and 'ModuleName' in self.csynth_module_info['top_module']:
                top_module_name = self.csynth_module_info['top_module']['ModuleName']
                if self.logger:
                    self.logger.print(f"Using top module: {top_module_name}")
                else:
                    print(f"Using top module: {top_module_name}")
        
        self.llvm_ir_hierarchy.build_llvm_ir_hierarchy_from_top(top_module_name)

    def save_llvm_ir_hierarchy_to_json(self, output_path=None):
        self.llvm_ir_hierarchy.save_hierarchy_to_json(Path(self.log_dir, "llvm_ir_hierarchy.json"))
    
    def save_pretty_print_llvm_ir_hierarchy_to_txt(self, output_path=None):
        self.llvm_ir_hierarchy.save_pretty_print_hierarchy_to_txt(Path(self.log_dir, "llvm_ir_hierarchy.txt"))

    def analyze_C_to_RTL_module_mapping(self, input_c_hierarchy_json):
        """
        Parses message_syn.xml to extract C-to-RTL mapping events and links
        them back to the original C hierarchy nodes.
        Raises FileNotFoundError if the input hierarchy JSON is missing.
        Returns dict with 'unrolls', 'inlines', 'flattens', each a list of events
        containing details + 'orig_node' reference.
        """
        # Ensure hierarchy JSON exists
        if not os.path.isfile(input_c_hierarchy_json):
            raise FileNotFoundError(f"Hierarchy JSON not found: {input_c_hierarchy_json}")
        """
        Parses message_syn.xml to extract C-to-RTL mapping events and links
        them back to the original C hierarchy nodes.
        Returns dict with 'unrolls', 'inlines', 'flattens', each a list of events
        containing details + 'orig_node' reference.
        """

        
        
