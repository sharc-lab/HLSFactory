import random
import re
from collections import defaultdict


# class OptDSL:
#     def __init__(self, opt_dsl_source: str) -> None:
#         self.pipeline_directives = defaultdict(list)
#         self.unroll_directives = defaultdict(list)
#         self.partition_directives = defaultdict(list)
#         self.parse_opt_dsl_lang(opt_dsl_source)

#     def parse_opt_dsl_lang(self, opt_dsl_source):
#         opt_dsl_source = re.sub(r"pipeline\s*\(", "self.pipeline(", opt_dsl_source)
#         opt_dsl_source = re.sub(r"unroll\s*\(", "self.unroll(", opt_dsl_source)
#         opt_dsl_source = re.sub(r"partition\s*\(", "self.partition(", opt_dsl_source)
#         #############################################################################
#         # exec is potentially unsafe, but seems like the easiest way to go about this
#         #############################################################################
#         exec(opt_dsl_source)

#     def pipeline(self, label: str, function: str) -> None:
#         directive = f"set_directive_pipeline {function}/{label}"
#         self.pipeline_directives[f"{function}/{label}"].append(directive)

#     def unroll(self, label: str, function: str, factor: int) -> None:
#         directive = f"set_directive_unroll -factor {factor} {function}/{label}"
#         self.unroll_directives[f"{function}/{label}"].append(directive)

#     def partition(
#         self, array_var: str, function: str, partition_type: str, factor: int, dim: int
#     ) -> None:
#         directive = f"set_directive_array_partition -type {partition_type} -factor {factor} -dim {dim} {function} {array_var}"
#         self.partition_directives[f"{function}/{array_var}"].append(directive)

#     def get_directives(self):
#         return (
#             self.pipeline_directives,
#             self.unroll_directives,
#             self.partition_directives,
#         )

#     # TODO
#     def add_static_optimization(self, opt_name: str) -> None:
#         raise NotImplementedError


class OptDSL:
    def __init__(self, opt_dsl_source: str) -> None:
        self.static_lines = []
        self.grouped_directives = defaultdict(lambda: defaultdict(list))
        self.pipeline_directives = defaultdict(list)
        self.ungrouped_partition_directives = defaultdict(list)
        self.ungrouped_unroll_directives = defaultdict(list)
        self.parse_opt_dsl_lang(opt_dsl_source)

    def parse_opt_dsl_lang(self, opt_dsl_source):
        lines = opt_dsl_source.splitlines()
        dynamic_lines = []

        for line in lines:
            stripped_line = line.strip()
            if (
                # Extract static directives
                stripped_line.startswith("set_directive_")
                and not any(
                    kw in stripped_line
                    for kw in ["pipeline", "unroll", "array_partition"]
                )
            ):
                self.static_lines.append(stripped_line)
            else:
                dynamic_lines.append(line)

        opt_dsl_source = "\n".join(dynamic_lines)
        opt_dsl_source = re.sub(r"pipeline\s*\(", "self.pipeline(", opt_dsl_source)
        opt_dsl_source = re.sub(r"unroll\s*\(", "self.unroll(", opt_dsl_source)
        opt_dsl_source = re.sub(r"partition\s*\(", "self.partition(", opt_dsl_source)
        #############################################################################
        # exec is potentially unsafe, but seems like the easiest way to go about this
        #############################################################################
        exec(opt_dsl_source)
        self.validate_grouped_directives()

    def pipeline(self, label: str, function: str, optional: bool = False) -> None:
        directive = f"set_directive_pipeline {function}/{label}"
        if optional:
            self.pipeline_directives[f"{function}/{label}"] = [directive, None]
        else:
            self.pipeline_directives[f"{function}/{label}"] = [directive]

    def unroll(self, label: str, function: str, factor, group: str = None) -> None:
        factors = factor if isinstance(factor, list) else [factor]
        for f in factors:
            directive = f"set_directive_unroll -factor {f} {function}/{label}"
            if group:
                self.grouped_directives[group][f].append(directive)
            else:
                self.ungrouped_unroll_directives[f"{function}/{label}"].append(directive)

    def partition(
        self, array_var: str, function: str, partition_type: str, factor, dim: int, group: str = None
    ) -> None:
        factors = factor if isinstance(factor, list) else [factor]
        for f in factors:
            directive = f"set_directive_array_partition -type {partition_type} -factor {f} -dim {dim} {function} {array_var}"
            if group:
                self.grouped_directives[group][f].append(directive)
            else:
                self.ungrouped_partition_directives[f"{function}/{array_var}"].append(directive)

    def validate_grouped_directives(self) -> None:
        # Validate all directive sets in the same group must have same number of lines
        for group, directives_by_factor in self.grouped_directives.items():
            if len(directives_by_factor) < 2:
                continue
            factor_list = list(directives_by_factor.keys())
            reference_len = len(directives_by_factor[factor_list[0]])
            for factor, directives in directives_by_factor.items():
                if len(directives) != reference_len:
                    raise ValueError(
                        f"Inconsistent factor list in group '{group}'"
                    )

    def get_directives(self):
        return (
            self.static_lines,
            self.grouped_directives,            
            self.pipeline_directives,
            self.ungrouped_partition_directives,
            self.ungrouped_unroll_directives
        )

