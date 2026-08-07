import re
from textwrap import dedent
from pathlib import Path

prompt_pre = dedent(
    """
## Overview
You are a helpful export hardware engineer and software developer who will assist the user with hardware design tasks for high-level synthesis.
The task will center around high-level synthesis (HLS) code written in C++ for a hardware design. The HLS design is written to target the latest Vitis HLS tool from Xilinx, which maps C++ code to a Verilog implementation for FPGAs.
"""
).strip()

prompt_output_format_optdsl = dedent(
    text="""
## Output Format
The generated OptDSL `opt_template.tcl` file should be provided in the following XML format:
```
<OUTPUT_CODE name="opt_template.tcl">
    ...
</OUTPUT_CODE>
```
Please use this XML format and do not use other formats like markdown code blocks or plain text.
Only output the generated TCL code in the XML format and nothing else.
"""
).strip()

prompt_gen_optdsl_v2 = dedent(
    """
## Task Description
You are given an HLS design including its implementation file, header file, testbench and natural language description.

Your task is to generate an OptDSLv2 optimization template file named `design_space.optdsl` to enable design space exploration for the kernel implementation file only. 
The testbench file is not relevant for optimization and should not be considered.

This format replaces TCL directives with a structured Python-like DSL that expresses HLS directives such as `pipeline`, `unroll`, and `partition`.

### OptDSLv2 Semantics:
- This format uses a Python-like DSL to describe directive configurations for:
    - `pipeline(label: str, function: str, optional: bool = False)`
    - `unroll(label: str, function: str, factor: list[int], group: str | None = None)`
    - `partition(array_var: str, function: str, partition_type: str, factor: list[int], dim: int, group: str | None = None)`
        - DO NOT use dimension index 0 in any `partition()` directive. Vivado HLS indexing starts from 1, and `dim=0` is invalid. 
        - The `partition_type` is fixed as `cyclic` for all partitions.

### OptDSL Output Requirements:
1. Resource Directives
   At the beginning of the file, use standard Vivado HLS directives for memory binding and inlining.
   For example:
    ```
    set_directive_resource -core RAM_1P "kernel_name" input_array_1_name
    set_directive_resource -core RAM_1P "kernel_name" input_array_2_name

    set_directive_inline sub_function_name
    ```

2. Grouped Optimizations (Zip-Combination by Factor)
   All `partition()` and `unroll()` calls that use the same `group` name will be zipped together by factor index. 
   For example:
    ```
    partition("input", "kernel", "cyclic", [1, 2, 4, 8], 2, "group_1")
    partition("output", "kernel", "cyclic", [1, 2, 4, 8], 2, "group_1")
    unroll("loop_i", "kernel", [1, 2, 4, 8], "group_1")

    ```
    or
    ```
    partition("A", "kernel", "cyclic", [1, 2, 4, 8], 2, "partition_group_1")
    partition("B", "kernel", "cyclic", [1, 2, 4, 8], 2, "partition_group_1")
    partition("C", "kernel", "cyclic", [1, 2, 4, 8], 2, "partition_group_1")    
 
    ```       
    This creates exactly 4 variants, each using a consistent factor across the grouped elements.

    Important Guidelines:
    - If you have multiple `partition()` directives with the same factor list, group them together using a common `group` name (like `partition_group_1`) to form a zipped block. This reduces redundancy and ensures efficient design space coverage.
    - The partition type is fixed as `cyclic` for all partitions.
    - A group must contain at least two directives. DO NOT create a group with only one directive—such grouping is unnecessary and should be avoided.
      - All directives in the same group MUST use the exact same factor list. Avoid mismatched factor list lengths or inconsistent values across grouped directives.
        For example, this is NOT allowed:
        ```
        partition("input", "kernel", "cyclic", [1, 2, 4, 8], 2, "group_1")
        partition("output", "kernel", "cyclic", [1, 2, 4, 8], 2, "group_1")
        unroll("loop_i", "kernel", [1, 2, 4, 8], "group_1")
        unroll("loop_j", "kernel", [1, 2, 4], "group_1")  # Mismatched factor list
        ```
        Instead, all of them must use the same full list, like [1, 2, 4, 8].
    - To balance design space size and coverage, DO NOT blindly group all unrolls. Choose some directives to group (especially ones with matching behavior or structure), and keep others ungrouped.

3. Ungrouped Optimizations (Full Cross Product)
   Ungrouped `unroll()` or `partition()` calls (i.e., with `group=None`) will be cross-producted with the grouped variants.
   Pipelines can be:
    - Always enabled: `pipeline("loop_label", "kernel")`
    - Optional (explored on/off): `pipeline("loop_label", "kernel", optional=True)`
   For example:
    ```
    pipeline("loop_j", "kernel")
    pipeline("loop_j", "kernel", optional=True)
    unroll("loop_j", "kernel", [1, 2, 4])
    unroll("loop_k", "kernel", [1, 2, 4, 8])
    partition("window", "kernel", "cyclic", [1, 2, 4], 1)

    ```

4. Factor Lists Guidelines
    The list of unroll or partition factors must be compatible with loop bounds or array sizes. 
    DO NOT use symbolic constants, variables, or macros. All elements in the factor list must be explicit integers.
    For example:
    - For loops with bound 3 → recommend `[3]`
    - For loops with bound 64 → recommend `[1, 2, 4, 8]`
    - For larger loops → consider `[2, 4, 8, 16]` if suitable
    If a loop processes an array along a specific dimension, the unroll factor list for that loop and the partition factor list on that array dimension should match to avoid banking conflicts.


The generated `opt_template.tcl` file should:
- Begin with all `set_directive_resource` and `set_directive_inline` lines.
- Use grouped and ungrouped `partition`, `unroll`, and `pipeline` directives appropriately.
  - Apply `partition()` to major I/O and intermediate arrays.
  - Apply `unroll()` and `pipeline()` to key loops, avoiding duplication.
- Provide a diverse, structured design space while avoiding redundancy and overgrowth.

### Constraints
- DO NOT apply `pipeline` or `unroll` to the same loop in both grouped and ungrouped sections.
- DO NOT apply multiple directives of the same kind (e.g., two `pipeline()` calls with and without `optional=True`, or two `partition()` directives with different factors) to the same loop or array.
- Match all loop labels and array names exactly as used in the kernel code.
- Ensure the design space has a balanced spread of performance vs. resource trade-offs.
- Keep the total number of configurations but diverse enough for performance-resource tradeoff analysis.

Ensure the total design space includes a rich spread of latency vs. resource trade-offs.

Your final output must be a complete and syntactically correct OptDSLv2 optimization template file named `design_space.optdsl` that defines a diverse and practical optimization space for the design.
"""
).strip()

def build_prompt_gen_optdsl_zero_shot(
    design_source_files: list[Path],
) -> str:
    p = prompt_pre
    p += "\n\n"
    p += prompt_gen_optdsl_v2
    p += "\n\n"
    p += prompt_output_format_optdsl
    p += "\n\n"

    p += "## Task Inputs\n"
    p += "\n"
    code = build_input_code_prompt_xml(
        {
            file.name: file.read_text()
            for file in design_source_files
        }
    )
    p += code
    p += "\n\n"

    p += "## Task Output\n"
    p += "\n"

    return p




def build_input_code_prompt_xml(code: dict[str, str]) -> str:
    p = ""
    p += "\n"
    for name, content in code.items():
        p += f'<INPUT_CODE name="{name}">\n'
        p += f"{content}\n"
        p += f'</INPUT_CODE name="{name}">\n'
    p += "\n"
    return p


def build_input_code_prompt_md(code: dict[str, str]) -> str:
    p = ""
    p += "\n"
    for name, content in code.items():
        p += f"```{name}\n"
        p += f"{content}\n"
        p += "```\n"
    p += "\n"
    return p


def extract_code_xml_from_llm_output(llm_output: str) -> dict[str, str]:
    code = {}

    tags_matches: list[re.Match] = []
    tags_matches += re.finditer(r"<OUTPUT_CODE name=\".+\">", llm_output)
    tags_matches += re.finditer(r"</OUTPUT_CODE(\s+name=\"(?:\S+)\")?>", llm_output)

    tags = list(map(lambda x: x.group(0), tags_matches))
    tag_locs = list(map(lambda x: x.start(0), tags_matches))
    tag_lengths = [len(tag) for tag in tags]

    if len(tags) % 2 != 0:
        raise ValueError("Invalid number of tags")

    sorted_tags, sorted_tag_locs, sorted_tag_lengths = zip(
        *sorted(zip(tags, tag_locs, tag_lengths), key=lambda x: x[1])
    )

    for i in range(0, len(sorted_tags), 2):
        start = sorted_tag_locs[i] + sorted_tag_lengths[i]
        end = sorted_tag_locs[i + 1]
        name = re.findall(r"name=\"(.+)\"", sorted_tags[i])[0]
        code[name] = llm_output[start:end]

    return code


def extract_code_markdown_from_llm_outout(llm_output: str) -> dict[str, str]:
    code = {}

    tags_matches: list[re.Match] = []
    tags_matches += re.finditer(r"```(?:\S+)\n", llm_output)
    tags_matches += re.finditer(r"```\n", llm_output)

    tags = list(map(lambda x: x.group(0), tags_matches))
    tag_locs = list(map(lambda x: x.start(0), tags_matches))
    tag_lengths = [len(tag) for tag in tags]

    if len(tags) % 2 != 0:
        raise ValueError("Invalid number of tags")

    sorted_tags, sorted_tag_locs, sorted_tag_lengths = zip(
        *sorted(zip(tags, tag_locs, tag_lengths), key=lambda x: x[1])
    )

    for i in range(0, len(sorted_tags), 2):
        start = sorted_tag_locs[i] + sorted_tag_lengths[i]
        end = sorted_tag_locs[i + 1]
        name = re.findall(r"```(\S+)\n", sorted_tags[i])[0]
        code[name] = llm_output[start:end]

    return code


def extract_code_from_markdown_simple(markdown: str) -> str:
    start = markdown.find("```")
    if start == -1:
        raise ValueError("No code block found")
    end = markdown.find("```", start + 3)
    if end == -1:
        raise ValueError("No closing code block found")
    return markdown[start:end].removeprefix("```").strip()


def approx_num_tokens(text: str, char_per_token_guess: int = 3) -> int:
    return len(text) // char_per_token_guess + 1

