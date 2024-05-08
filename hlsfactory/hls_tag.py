# from typing import Generic, TypeVar

# import jinja2
# from jinja2.ext import Extension

# Highly experimental code not yet working or might not be used at all


# def xilinx_pipeline(
#     ii: int | None = None,
#     off: bool = False,
#     rewind: bool = False,
#     style: str | None = None,
# ) -> str:
#     line = "#pragma HLS pipeline"
#     if ii:
#         line += f" II={ii}"
#     if off:
#         line += " off"
#     if rewind:
#         line += " rewind"
#     if style:
#         line += f" style={style}"
#     return line


# def xilinx_array_partition(
#     name: str,
#     type: str | None = None,
#     factor: int | None = None,
#     dim: int | None = None,
#     off: bool = False,
# ) -> str:
#     line = "#pragma HLS array_partition"
#     line += f" variable={name}"
#     if type:
#         line += f" type={type}"
#     if factor:
#         line += f" factor={factor}"
#     if dim:
#         line += f" dim={dim}"
#     if off:
#         line += " off=true"
#     return line


# def xilinx_intel(
#     off: bool = False,
#     factor: int | None = None,
#     skip_exit_check: bool = False,
# ) -> str:
#     line = "#pragma HLS unroll"
#     if off:
#         line += " off=true"
#     if factor:
#         line += f" factor={factor}"
#     if skip_exit_check:
#         line += " skip_exit_check"
#     return line


# T = TypeVar("T")


# class Combos(Generic[T]):
#     def __init__(self, *values: T) -> None:
#         self.values = values


# def combo_set(): ...


# class HLSTagExtension(Extension):
#     def __init__(
#         self,
#         environment: jinja2.Environment,
#     ) -> None:
#         super().__init__(environment)

#     def parse(self, parser):
#         # find a call node with the name of one of the tags
#         token = next(parser.stream)
#         tag = token.value
#         print(tag)
