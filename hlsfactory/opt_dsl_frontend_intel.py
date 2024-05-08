import hashlib
import itertools
import os
import random
import re
import shutil
from pathlib import Path

from hlsfactory.framework import Design, Frontend


class ArrayPartition:
    # the first line as the input array_settings
    def __init__(self, array_settings: str):
        setting_lists = array_settings.split(",")
        self.num_of_directives = int(setting_lists[1])
        self.factors = setting_lists[2].strip("[]").split()
        self.arr_types = setting_lists[3].strip("[]").split()
        # lines are actual templates to wrtie to tcl files
        self.directives = []

    def get_flattened(self) -> list:
        output = []
        for factor in self.factors:
            for array_type in self.arr_types:
                output.append([factor, array_type])
        return output

    def get_num_of_directives(self):
        return self.num_of_directives

    def append_directives(self, line):
        self.directives.append(line)

    def get_directives(self):
        return self.directives


class LoopOpt:
    def __init__(self, loop_settings):
        self.num_of_parameters = int(loop_settings.split(",")[1])
        self.num_of_directives = int(loop_settings.split(",")[2])
        # lines are actual templates to wrtie to tcl files
        self.directives = []
        # factors/parameters for the tcl files in strings
        self.parameter_lines = []

    def get_flattened(self):
        output = []
        for line in self.parameter_lines:
            loop_name = line.split(",")[1]
            loop_pipeline = line.split(",")[2].strip() == "pipeline"
            loop_unroll = line.split(",")[3].strip() == "unroll"
            unroll_factor_lists = line.split(",")[4].strip("[]").split()
            unroll_factor_lists = [x.strip("[]") for x in unroll_factor_lists]
            for unroll_factor in unroll_factor_lists:
                output.append([loop_name, loop_pipeline, loop_unroll, unroll_factor])
        return output

    def append_parameters(self, line):
        self.parameter_lines.append(line)

    def append_directives(self, line):
        self.directives.append(line)

    def get_directives(self):
        return self.directives

    def get_num_of_parameters(self):
        return self.num_of_parameters

    def get_num_of_directives(self):
        return self.num_of_directives


#################################################
def parse_template(
    src_template: Path,
) -> tuple[list[ArrayPartition], list[LoopOpt], str]:
    array_partition_object_lists: list[ArrayPartition] = []
    loop_opt_object_lists: list[LoopOpt] = []
    static_lines = ""

    lines = src_template.read_text().splitlines()
    i = 0
    while i < len(lines):
        line = lines[i]
        if line.strip().startswith("#") or line.rstrip() == "":
            i += 1
            continue

        if line.strip().startswith("array_partition"):
            array_partition_object = ArrayPartition(line.strip())
            for _x in range(array_partition_object.get_num_of_directives()):
                i += 1
                array_partition_object.append_directives(lines[i])
            array_partition_object_lists.append(array_partition_object)
            i += 1
            continue

        if line.strip().startswith("loop_opt"):
            loop_opt_object = LoopOpt(line.strip())
            for _x in range(loop_opt_object.get_num_of_parameters()):
                i += 1
                loop_opt_object.append_parameters(lines[i])

            for _x in range(loop_opt_object.get_num_of_directives()):
                i += 1
                loop_opt_object.append_directives(lines[i])

            loop_opt_object_lists.append(loop_opt_object)

            i += 1
            continue

        static_lines += line + "\n"
        i += 1

    return array_partition_object_lists, loop_opt_object_lists, static_lines


###################################################
def gen_opt(array_partition_object_lists, loop_opt_object_lists):
    array_partition_lines = []
    for array_partition_object in array_partition_object_lists:
        directive_lines = array_partition_object.get_directives()
        array_block_lines = []
        for array_settings in array_partition_object.get_flattened():
            # factor is 1, no need to set directives
            if array_settings[0].strip() == "1":
                array_block_lines.append("")
                continue

            output_line = ""
            for line in directive_lines:
                temp_line = line.replace("[factor]", array_settings[0])
                temp_line = temp_line.replace("[type]", array_settings[1])
                output_line += temp_line + "\n"

            array_block_lines.append(output_line)

        array_partition_lines.append(array_block_lines)
    array_partition_lines = list(itertools.product(*array_partition_lines))

    loop_opt_lines = []
    for loop_opt_object in loop_opt_object_lists:
        lines = loop_opt_object.get_directives()
        directive_pipeline_lines = []
        directive_unroll_lines = []
        for i in range(len(lines)):
            if lines[i].find("set_directive_pipeline") >= 0:
                directive_pipeline_lines.append(lines[i])
            else:
                directive_unroll_lines.append(lines[i])
        # by default, nothing to apply to the unroll and pipeline
        loop_opt_block = []
        for loop_opt_settings in loop_opt_object.get_flattened():
            output_line = ""
            # need to pipeline
            if loop_opt_settings[1] is True:
                for line in directive_pipeline_lines:
                    temp_line = line.replace("[name]", loop_opt_settings[0])
                    output_line += temp_line + "\n"

            # need to unroll
            if loop_opt_settings[2] is True:
                for line in directive_unroll_lines:
                    temp_line = line.replace("[factor]", loop_opt_settings[3])
                    temp_line = temp_line.replace("[name]", loop_opt_settings[0])
                    output_line += temp_line + "\n"

            loop_opt_block.append(output_line)

        loop_opt_lines.append(loop_opt_block)
    loop_opt_lines = list(itertools.product(*loop_opt_lines))
    return array_partition_lines, loop_opt_lines


### return dict[array_name, factor] ###
def get_array_partition_dic(lines: str) -> dict[str, str]:
    line_lists = lines.split("\n")
    array_dict = {}
    for line in line_lists:
        if "set_directive_array_partition" in line:
            array_name = line.split(" ")[-1]

            words = line.split()
            index_factor = words.index("-factor")
            array_factor = words[index_factor + 1]

            array_dict[array_name] = array_factor

    return array_dict


### return dict[loop_name, factor] ###
def get_loop_unroll_dic(lines: str) -> dict[str, str]:
    line_lists = lines.split("\n")
    loop_dict = {}
    for line in line_lists:
        if "set_directive_unroll" in line:
            loop_name = line.split(" ")[-1]
            loop_name = loop_name.replace('"', "")
            loop_name = loop_name.split("/")[-1]

            words = line.split()
            index_factor = words.index("-factor")
            loop_factor = words[index_factor + 1]

            loop_dict[loop_name] = loop_factor
    return loop_dict


### return list[loop_name] ###
def get_pipeline_list(lines: str) -> list[str]:
    line_lists = lines.split("\n")
    pipeline_list = []
    for line in line_lists:
        if "set_directive_pipeline" in line:
            pipeline_name = line.split(" ")[-1]
            pipeline_name = pipeline_name.replace('"', "")
            pipeline_name = pipeline_name.split("/")[-1]
            pipeline_list.append(pipeline_name)

    return pipeline_list


def get_kernel(hls_template: Path) -> (str, str):
    lines = hls_template.read_text().splitlines()
    kernel_name = str
    kernel_c = str
    for line in lines:
        if "set_top" in line:
            kernel_name = line.split(" ")[-1]
            # print ("kernel name is", kernel_name)
        #########################################################################################
        # Warning: It assumes that only one .c file exsits and that is the source of the kernel #
        #########################################################################################
        if ".c" in line and "add_file" in line and "-tb" not in line:
            kernel_c = line.split(" ")[-1].split("/")[-1]

    return kernel_name, kernel_c


polybench_header_text = [
    "#include <unistd.h> \n",
    "#include <string.h> \n",
    "#include <math.h> \n",
    "#include <HLS/stdio.h> \n",
    "#include <HLS/hls.h> \n",
    "\n",
]


def polybench_copy(source_dir: Path, target_dir: Path, kernel_name: str) -> None:
    # modify the header file and copy all the other files to the target folder
    for file_name in os.listdir(source_dir):
        if file_name == f"{kernel_name}.h":
            with (source_dir / file_name).open("r") as f:
                lines = f.readlines()
                new_lines = []
                for line in lines:
                    if "#include" not in line.strip():
                        new_lines.append(line)

            new_lines = polybench_header_text + new_lines
            with (target_dir / file_name).open("w") as target_f:
                target_f.writelines(new_lines)
            continue

        shutil.copy(source_dir / file_name, target_dir / file_name)


#### return the name list of annotated C codes  #######
def generate_annotate_c(
    design_dir: Path,
    array_partition_lines,
    loop_opt_lines,
    static_lines,
    work_dir: Path,
    kernel_name: str,
    kernel_file: str,
    random_sample=False,
    random_sample_num=10,
) -> list[Design]:
    line_combos_all = list(itertools.product(array_partition_lines, loop_opt_lines))
    if random_sample:
        line_combos_all = random.sample(line_combos_all, random_sample_num)
    # else:
    #     line_combos_all = line_combos_all

    #####################################################################################################
    # @Note: I do not use any hash here to generate the annoated C, simply put a counter at the end     #
    #####################################################################################################
    ct = 0
    kernel_f = open(kernel_file)
    design_list = []
    for a_line, l_line in line_combos_all:
        ct += 1

        a_l = ""
        l_l = ""
        for x in a_line:
            a_l += x

        for x in l_line:
            l_l += x

        dir = work_dir / str(design_dir).split("/")[-2] / (kernel_name + "_" + str(ct))
        if dir.exists():
            shutil.rmtree(dir)
        dir.mkdir(parents=True)
        # print ("design dir is",str(design_dir).split('/')[-2])
        # copy and modify the files to the working folder
        polybench_copy(design_dir, dir, kernel_name)

        array_partition_dic = get_array_partition_dic(a_l + l_l + static_lines)
        loop_unroll_dic = get_loop_unroll_dic(a_l + l_l + static_lines)
        pipeline_list = get_pipeline_list(a_l + l_l + static_lines)

        kernel_f.seek(0, 0)
        new_filename = dir / (kernel_name + "_" + str(ct) + ".c")
        new_f = open(new_filename, "w+")

        if "polybench" in str(design_dir):
            for line in kernel_f:
                new_line = line

                ### This is not safe since patterns matches only with void type function ####
                if "void " + kernel_name in line:
                    new_line = "component " + new_line

                # insert array partition
                if kernel_name not in line and "DATA_TYPE" in line:
                    array_name = line.split(" ")[-1]
                    array_name = array_name.split("[")[0]
                    if array_name in array_partition_dic:
                        new_f.write(
                            "hls_numbanks(" + array_partition_dic[array_name] + ")\n",
                        )

                ##############################################################################################
                # @Note: To detect a for loop label, simply detecting ':', it is unsafe but usable for now   #
                ##############################################################################################
                elif ":" in line:
                    match = re.search(r"(\w+):", line)
                    if match:
                        loop_name = match.group(1)
                        if loop_name in loop_unroll_dic:
                            new_f.write(
                                "#pragma unroll " + loop_unroll_dic[loop_name] + "\n",
                            )
                            if loop_name not in pipeline_list:
                                new_f.write("#pragma disable_loop_pipelining\n")

                        new_line = new_line.replace(loop_name, "")
                        new_line = new_line.replace(":", "")

                new_line = new_line.replace("register", "")
                new_f.write(new_line)

            new_design = Design(new_filename, dir)
            design_list.append(new_design)

            ############Machsuite###############
            # numbanks( ) and array partition for machsuite
        elif "machsuite" in str(design_dir):
            #         component_names= ("void " + kernel_name, "int " +kernel_name)
            for num, line in enumerate(kernel_f, 1):
                # iterate for functions only#
                oldline = line
                new_line = line
                is_function = re.search(r"\bint\b.*\b\(\b", line) or re.search(
                    r"\bvoid\b.*\b\(\b",
                    line,
                )
                #      if any(s in line for s in component_names):
                if is_function:
                    params = re.findall(r"\((.*?)\)", line)
                    stringc = "".join(map(str, params))
                    split_words = stringc.split(",")
                    # print ("inside component function", kernel_name)
                    to_replace = []
                    array_index = []
                    data_type = []
                    array_name = []
                    for word in split_words:
                        word = word.lstrip()
                        for i in range(len(word)):
                            if word[i] == "[":
                                to_replace1 = word
                                array_index1 = word[i + 1]  # the letter next to [
                                data_type1 = word.split()[0]
                                array_name_is = re.findall(
                                    r"\ (.*?)\[",
                                    word,
                                )  # this is a list
                                array_name1 = "".join(
                                    map(str, array_name_is),
                                )  # convert to string
                                # print ("word... is",to_replace1,array_index1,data_type1,array_name_is,array_name1)

                                to_replace.append(to_replace1)
                                array_index.append(array_index1)
                                data_type.append(data_type1)
                                array_name.append(array_name1)
                    newline = line

                    #         if array_name1 in array_partition_dic:
                    for j in range(len(to_replace)):
                        newline = newline.replace(
                            to_replace[j],
                            " hls_avalon_slave_memory_argument("
                            + array_index[j]
                            + ") hls_numbanks("
                            + "8"
                            + ")"
                            + " hls_bankwidth(sizeof("
                            + data_type[j]
                            + "))"
                            + " "
                            + data_type[j]
                            + "  *"
                            + array_name[j],
                        )

                    # print ("kernel name is ", kernel_name)
                    # print ("Old line was", oldline)
                    # print ("New line is", newline)
                    new_f.write(newline)

                # loop unrolls for machsuite
                if ":" in line:
                    match = re.search(r"(\w+):", line) or re.search(r"(\w+) :", line)
                    if match:
                        loop_name = match.group(1)
                        if loop_name in loop_unroll_dic:
                            new_f.write(
                                "#pragma unroll " + loop_unroll_dic[loop_name] + "\n",
                            )
                            if loop_name not in pipeline_list:
                                new_f.write("#pragma disable_loop_pipelining\n")

                        new_line = new_line.replace(loop_name, "")
                        new_line = new_line.replace(":", "")

                new_line = new_line.replace("register", "")
                if not is_function:
                    new_f.write(new_line)
            new_design = Design(new_filename, dir)
            design_list.append(new_design)

        new_f.close()

    kernel_f.close()

    return design_list


class OptDSLFrontendIntel(Frontend):
    name = "OptDSLFrontendIntel"

    def __init__(self, work_dir: Path, random_sample=False, random_sample_num=10):
        self.work_dir = work_dir
        self.random_sample = random_sample
        self.random_sample_num = random_sample_num

    def execute(self, design: Design, timeout: float | None) -> list[Design]:
        opt_template_fp = design.dir / "opt_template.tcl"

        #####################################################################################################
        # @Note: It takes hls_template.tcl as the input to seek for the top function to synthesize          #
        #####################################################################################################
        hls_template = design.dir / "hls_template.tcl"

        ###  set_top [kernel_name] from hls_template.tcl  ###
        kernel_name, kernel_c = get_kernel(hls_template)

        kernel_file = design.dir / "src" / kernel_c
        design_dir = design.dir / "src"
        (
            array_partition_object_lists,
            loop_opt_object_lists,
            static_lines,
        ) = parse_template(opt_template_fp)
        array_partition_lines, loop_opt_lines = gen_opt(
            array_partition_object_lists,
            loop_opt_object_lists,
        )

        design_list = generate_annotate_c(
            design_dir,
            array_partition_lines,
            loop_opt_lines,
            static_lines,
            self.work_dir,
            kernel_name,
            kernel_file,
            self.random_sample,
            self.random_sample_num,
        )

        return design_list
