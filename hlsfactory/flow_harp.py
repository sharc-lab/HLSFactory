# Copied from https://github.com/UCLA-VAST/HARP/ to generate HARP graphs for HLS programs
# depedns on programl


import os
import networkx as nx
import json
import shutil
from os.path import join, abspath, basename, exists, dirname, isfile
from subprocess import Popen, PIPE
from collections import OrderedDict
from copy import deepcopy
import ast
from pprint import pprint
from shutil import copy
from glob import glob, iglob
import csv
import re
import programl

from utils import create_dir_if_not_exists, get_root_path, natural_keys


PRAGMA_POSITION = {"PIPELINE": 0, "TILE": 2, "PARALLEL": 1}
BENCHMARK = "machsuite"
BENCHMARK = "poly"
type_graph = "harp"
processed_gexf_folder = join(get_root_path(), f"{type_graph}/{BENCHMARK}/processed")
auxiliary_node_gexf_folder = join(
    get_root_path(), f"{type_graph}/{BENCHMARK}/processed/extended-pseudo-block-base/"
)
MACHSUITE_KERNEL = [
    "aes",
    "gemm-blocked",
    "gemm-ncubed",
    "spmv-crs",
    "spmv-ellpack",
    "stencil_stencil2d",
    "nw",
    "md",
    "stencil-3d",
]

poly_KERNEL = [
    "2mm",
    "3mm",
    "adi",
    "atax",
    "bicg",
    "bicg-large",
    "covariance",
    "doitgen",
    "doitgen-red",
    "fdtd-2d",
    "fdtd-2d-large",
    "gemm-p",
    "gemm-p-large",
    "gemver",
    "gesummv",
    "heat-3d",
    "jacobi-1d",
    "jacobi-2d",
    "mvt",
    "seidel-2d",
    "symm",
    "symm-opt",
    "syrk",
    "syr2k",
    "trmm",
    "trmm-opt",
    "mvt-medium",
    "correlation",
    "atax-medium",
    "bicg-medium",
    "gesummv-medium",
    "symm-opt-medium",
    "gemver-medium",
]
ALL_KERNEL = {"machsuite": MACHSUITE_KERNEL, "poly": poly_KERNEL}


class Node:
    def __init__(self, block, function, text, type_n, features=None):
        self.block: int = block
        self.function: int = function
        self.text: str = text
        self.type_n: int = type_n  ## 0: instruction, 1: variable, 2: immediate, 3: pragma, 4: pseudo node for block
        self.features: str = features  ## contains full text

    def get_attr(self, after_process=True):
        """
        args:
            after_process : True if nodes are added to existing GNN-DSE graphs
                            False for initial graph generation in GNN-DSE
        """
        n_dict = {}
        n_dict["block"] = self.block
        n_dict["function"] = self.function
        n_dict["text"] = self.text
        n_dict["type"] = self.type_n
        if after_process:
            n_dict["full_text"] = self.features
        else:
            n_dict["features"] = {"full_text": [self.features]}

        return n_dict


class Edge:
    def __init__(self, src, dst, flow, position):
        self.src: int = src
        self.dst: int = dst
        self.flow: int = flow  ## 0: control, 1: data, 2: call, 3: pragma, 4: pseudo node for block, 5: connections between pseudo nodes, 6: for loop hierarchyhierarchy
        self.position: int = position

    def get_attr(self):
        e_dict = {}
        e_dict["flow"] = self.flow
        e_dict["position"] = self.position

        return e_dict


def create_pseudo_node_block(block, function):
    return Node(
        block,
        function,
        text="pseudo_block",
        type_n=4,
        features="auxiliary node for each block",
    )


def add_to_graph(g_nx, nodes, edges):
    if len(nodes) > 0:
        g_nx.add_nodes_from(nodes)
    if len(edges) > 0:
        g_nx.add_edges_from(edges)


def copy_files(name, src, dest):
    """
    copy the generated files to the project directory

    args:
        name: the kernel name
        src: the path to the files
        dest: where you want to copy the files
    """
    gen_files = [f for f in sorted(glob(join(src, f"{name}.*")))]
    gen_files.append(join(src, f"{name}_pretty.json"))
    gen_files.append(join(src, f"ds_info.json"))
    for f in gen_files:
        if "ds_info.json" in f:
            source_dest = join(
                os.getcwd(), BENCHMARK, "config", f"{name}_ds_config.json"
            )
            copy(f, source_dest)
            continue
        if f.endswith(".c") or f.endswith(".cpp"):
            new_f_name = basename(f).replace(f"{name}", f"{name}_kernel")
            source_dest = join(os.getcwd(), BENCHMARK, "sources", new_f_name)
            copy(f, source_dest)
        copy(f, dest)


def read_json_graph(name, readable=True):
    """
    reads a graph in json format as a netwrokx graph

    args:
        name: name of the json file/ kernel's name
        reaable: whether to store a readable format of the json file

    returns:
        g_nx: graph in networkx format
    """
    filename = name + ".json"
    with open(filename) as f:
        js_graph = json.load(f)
    g_nx = nx.readwrite.json_graph.node_link_graph(js_graph)
    if readable:
        make_json_readable(name, js_graph)

    return g_nx


def llvm_to_nx(name):
    """
    reads a LLVM IR and converts it to a netwrokx graph

    args:
        name: name of the LLVM file/ kernel's name

    returns:
        g_nx: graph in networkx format
    """
    filename = name + ".ll"
    with open(filename) as f:
        ll_file = f.read()
        G = programl.from_llvm_ir(ll_file)
        g_nx = programl.to_networkx(G)

    return g_nx


def make_json_readable(name, js_graph):
    """
    gets a json file and beautifies it to make it readable

    args:
        name: kernel name
        js_graph: the graph in networkx format read from the json file

    writes:
        a readable json file with name {name}_pretty.json
    """
    filename = name + "_pretty.json"
    f_json = open((filename), "w+")
    json.dump(js_graph, f_json, indent=4, sort_keys=True)
    f_json.close()


def extract_function_names(c_code):
    """
    extract the names of the function in c code along with their line number

    args:
        c_code: the c_code read with code.read()

    return:
        a list of tuples of (function name, line number)
    """
    pattern = r"\b\w+\s+\w+\s*\([^)]*\)\s*{"
    function_matches = re.finditer(pattern, c_code)
    function_names = []
    for match in function_matches:
        function_name = match.group().split()[1]
        line_number = c_code.count("\n", 0, match.start()) + 1
        function_names.append((function_name.split("(")[0], line_number))
    return function_names


def get_tc_for_loop(for_loop_text):
    """
    get trip count of the for loop
    """
    comp = for_loop_text.split(";")[1].strip()
    delims = ["<=", ">=", "<", ">", "--"]  ## FIXME: support for other condition types
    delim = None
    for d in delims:
        if d in comp:
            delim = d
            break
    if delim:
        if delim == "--":
            TC = 0
        else:
            TC = int(eval(comp.replace(" ", "").split(delim)[-1].strip()))
        return TC
    else:
        print(f"no comparison sign found in {for_loop_text}")
        raise RuntimeError()


def get_icmp(path, name, log=False):
    """
    gets an llvm file and returns the icmp instructions of each for loop

    args:
        path: parent directory of the llvm file
        name: kernel name

    returns:
        a dictionary corresponding to the icmp instructions:
            {for loop id: [icmp instruction, for.cond line number, icmp line number]}
        number of for loops
    """
    for_dict_llvm = OrderedDict()  ## {function inst: {for loop id: [icmp instruction, for.cond line number, icmp line number]}} ## function inst is the LLVM-equivalent of function defintion starting with "define"
    f_llvm = open(join(path, f"{name}.ll"), "r")
    lines_llvm = f_llvm.readlines()
    for_count_llvm, local_for_count_llvm = 0, 0
    func_inst = None
    for idx, line in enumerate(lines_llvm):
        if line.strip().startswith("define"):
            for_dict_llvm[line.strip()] = OrderedDict()
            func_inst = line.strip()
            local_for_count_llvm = 0
        elif line.strip().startswith("for.cond"):
            for_count_llvm += 1
            local_for_count_llvm += 1
            for idx2, line2 in enumerate(lines_llvm[idx + 1 :]):
                if line2.strip().startswith("for.body"):
                    print(
                        f"Do you have the right LLVM code? no icmp instruction found for loop at line {idx}."
                    )
                    raise RuntimeError()
                elif "icmp" in line2.strip():
                    assert func_inst != None, "no function scope found"
                    for_dict_llvm[func_inst][local_for_count_llvm] = [
                        line2.strip(),
                        idx,
                        idx2 + idx + 1,
                    ]
                    break
    if log:
        print(json.dumps(for_dict_llvm, indent=4))
    return for_dict_llvm, for_count_llvm


def get_pragmas_loops(path, name, EXT="c", log=False):
    """
    gets a c kernel and returns the pragmas of each for loop

    args:
        path: parent directory of the kernel file
        name: kernel name

    returns:
        a dictionary with each entry showing the for loop and its pragmas
            {for loop id: [for loop source code, [list of pragmas]]}
        number of for loops
    """

    for_dict_source = (
        OrderedDict()
    )  ## {function name: {for loop id: [for loop source code, [list of pragmas]]}}
    f_source = open(join(path, f"{name}.{EXT}"), "r")
    lines_source = f_source.readlines()
    f_source.close()
    with open(join(path, f"{name}.{EXT}"), "r") as f_source:
        function_names_list = extract_function_names(f_source.read())
    for_count_source, local_for_count_source = 0, 0
    pragma_zone = False
    for f_id, (f_name, idx_start) in enumerate(function_names_list):
        for_dict_source[f_name] = OrderedDict()
        local_for_count_source = 0
        last_line = -1
        if f_id + 1 < len(function_names_list):
            last_line = function_names_list[f_id + 1][1]
        for idx_, line in enumerate(lines_source[idx_start:last_line]):
            idx = idx_ + idx_start
            line = line.strip()
            if (
                not line or "scop" in line
            ):  ## if it's a blank line or #pragma scop in it, skip it
                continue
            if line.startswith("for(") or line.startswith("for "):
                for_count_source += 1
                local_for_count_source += 1
            if pragma_zone:
                if ":" in line:
                    continue  ## if it is a loop label, skip it
                if line.startswith("#pragma"):
                    pragma_list.append(line)
                elif line.startswith("for"):
                    for_dict_source[f_name][local_for_count_source] = [
                        line.strip("{"),
                        pragma_list,
                    ]
                    pragma_zone = False
                else:
                    print(
                        f"Do you have the right source code? expected either for loop or pragma at line {idx} but got {line}."
                    )
                    raise RuntimeError()
            else:
                if line.startswith("#pragma") and not "KERNEL" in line.upper():
                    pragma_list = [line]
                    pragma_zone = True

    if log:
        print(json.dumps(for_dict_source, indent=4))

    return for_dict_source, for_count_source


def create_pragma_nodes(g_nx, g_nx_nodes, for_dict_source, for_dict_llvm, log=True):
    """
    creates nodes for each pragma to be added to the graph

    args:
        g_nx: the graph object
        g_nx_nodes: number of nodes of the graph object
        for_dict_source: the for loops along with their pragmas
        for_dict_llvm: the for loops along with their icmp instruction in llvm

    returns:
        a list of nodes and a list of edges to be added to the graph
    """
    new_nodes, new_edges = [], []
    new_node_id = g_nx_nodes
    for f_name, f_content in for_dict_source.items():
        if len(f_content) == 0:  ## no pragma exists in this function
            continue
        llvm_content = [f for f in for_dict_llvm if f_name in f]
        assert len(llvm_content) == 1
        llvm_content = for_dict_llvm[llvm_content[0]]
        for for_loop_id, [for_loop_text, pragmas] in f_content.items():
            icmp_inst = llvm_content[for_loop_id][0]
            icmp_inst_ = icmp_inst.split("!dbg")[0]  ## if it has line number, strip it
            split_icmp_inst = icmp_inst_.split(",")
            TC_id = -1
            for sp in split_icmp_inst[::-1]:
                if sp.strip() == "":  ## sp is white space ''
                    TC_id -= 1
                else:
                    break
            TC_icmp = int(eval(split_icmp_inst[TC_id].strip()))
            TC_for = get_tc_for_loop(for_loop_text)
            assert (
                TC_for == TC_icmp
            ), f"trip count of loop {for_loop_text} did not match {icmp_inst}."

            node_id, block_id, function_id = None, None, None
            for node, ndata in g_nx.nodes(data=True):
                if "features" in ndata:
                    feat = ast.literal_eval(str(ndata["features"]))
                    if icmp_inst == feat["full_text"][0]:
                        print(f"found {icmp_inst} with id {node}")
                        node_id = int(node)
                        block_id = int(ndata["block"])
                        function_id = int(ndata["function"])
                        break
            if not node_id:
                print(f"icmp instruction {icmp_inst} not found.")
                raise RuntimeError()

            for pragma in pragmas:
                p_dict = {}
                p_dict["type"] = 3
                p_dict["block"] = block_id
                p_dict["function"] = function_id
                p_dict["features"] = {"full_text": [pragma]}
                p_dict["text"] = pragma.split(" ")[2]
                new_nodes.append((new_node_id, p_dict))

                e_dict = {
                    "flow": 3,
                    "position": PRAGMA_POSITION[p_dict["text"].upper()],
                }
                new_edges.append((node_id, new_node_id, e_dict))
                new_edges.append((new_node_id, node_id, e_dict))

                new_node_id += 1
    if log:
        pprint(new_nodes)
        pprint(new_edges)

    return new_nodes, new_edges


def prune_redundant_nodes(g_new):
    while True:
        remove_nodes = set()
        for node in g_new.nodes():
            if len(list(g_new.neighbors(node))) == 0 or node is None:
                print(node)
                remove_nodes.add(node)
                remove_nodes.add(node)
        for node in remove_nodes:
            g_new.remove_node(node)
        if not remove_nodes:
            break


def process_graph(name, g, csv_dict=None):
    """
    adjusts the node/edge attributes, removes redundant nodes,
        and writes the final graph to be used by GNN-DSE

    args:
        name: kernel name
        dest: where to store the graph
    """
    g_new = nx.MultiDiGraph()
    for node, ndata in g.nodes(data=True):
        attrs = deepcopy(ndata)
        if "features" in ndata:
            feat = ndata["features"]
            attrs["full_text"] = feat["full_text"][0]
            del attrs["features"]

        g_new.add_node(node)
        nx.set_node_attributes(g_new, {node: attrs})

    edge_list = []
    id = 0
    for nid1, nid2, edata in g.edges(data=True):
        edata["id"] = id
        edge_list.append((nid1, nid2, edata))
        id += 1
    g_new.add_edges_from(edge_list)

    prune_redundant_nodes(g_new)

    new_gexf_file = join(processed_gexf_folder, f"{name}_processed_result.gexf")
    if len(g_new.nodes) != len(g.nodes):
        print("#nodes:", len(g_new.nodes), "before processing was:", len(g.nodes))
    if len(g_new.edges) != len(g.edges):
        print("#edges:", len(g_new.edges), "before processing was:", len(g.edges))
    nx.write_gexf(g_new, new_gexf_file)
    current_g_value = {}
    current_g_value["num_node"] = len(g_new.nodes)
    current_g_value["num_edge"] = len(g_new.edges)
    current_g_value["name"] = name
    if csv_dict:
        csv_dict[name] = current_g_value


def graph_generator(name, path, benchmark, generate_programl=False, csv_dict=None):
    """
    runs ProGraML [ICML'21] to generate the graph, adds the pragma nodes,
        processes the final graph to be accepted by GNN-DSE

    args:
        name: kernel name
        path: path to parent directory of the kernel file
        benchmark: [machsuite|poly] None: simple program
    """
    ## generate PrograML graph
    if generate_programl:
        p = Popen(
            f"{get_root_path()}/clang_script.sh {name} {path} {type_graph}",
            shell=True,
            stdout=PIPE,
        )
        p.wait()

    ## convert it to networkx format
    # g_nx = read_json_graph(join(path, name))
    g_nx = llvm_to_nx(join(path, name))
    g_nx_nodes, g_nx_edges = g_nx.number_of_nodes(), len(g_nx.edges)

    ## find for loops and icmp instructions in llvm code
    for_dict_llvm, for_count_llvm = get_icmp(path, name)

    ## find for loops and their pragmas in the C/C++ code
    for_dict_source, for_count_source = get_pragmas_loops(path, name)
    assert for_count_llvm == for_count_source, (
        f"the number of for loops from the LLVM code and source code do not match "
        f"{for_count_llvm} in llvm vs {for_count_source} in the code"
    )

    print(f"number of nodes: {g_nx_nodes} and number of edges: {g_nx_edges}")
    graph_path = join(path, name + ".gexf")
    nx.write_gexf(g_nx, graph_path)

    augment_graph = True
    if augment_graph:
        ## create pragma nodes and their edges
        new_nodes, new_edges = create_pragma_nodes(
            g_nx, g_nx_nodes, for_dict_source, for_dict_llvm
        )

        add_to_graph(g_nx, new_nodes, new_edges)
        print(
            f"number of new nodes: {g_nx.number_of_nodes()} and number of new edges: {len(g_nx.edges)}"
        )
        process = True
        if process:
            process_graph(name, g_nx, csv_dict)

    copy_files_ = True
    if generate_programl:
        copy_files = True
    local = True  # True: programl is running in the directories inside this project
    if copy_files_:
        if not local:
            dest = join(os.getcwd(), f"{type_graph}", benchmark, name)
            create_dir_if_not_exists(dest)
            copy_files(name, path, dest)
        else:
            dest = path


def get_for_blocks_info(name, path):
    with open(join(path, name, f"{name}.ll"), "r") as f_llvm:
        lines_llvm = f_llvm.readlines()

    for_blocks_info = OrderedDict()  # label: {ind: loop number, preds:, next_instr:, line_num:, end: [(for.end line num, for.end label)], possible_children: children:}
    # possible_children are all children, children is only first level children
    # check up to 3 next instr to make sure the block is correct
    for_stack = []  # push for.cond pop for.end
    for_start = []
    for_end = []
    for_label = []
    i = 0
    for idx, line in enumerate(lines_llvm):
        if line.startswith("for."):
            content = line.strip().split(";")
            line = content[0].strip()
            if "for.cond" in line:
                key = f"{line}{idx}"
                assert key not in for_blocks_info
                for_blocks_info[key] = {
                    "ind": i,
                    "preds": content[1],
                    "next_instr": [
                        lines_llvm[idx + 1].strip(),
                        lines_llvm[idx + 2].strip(),
                        lines_llvm[idx + 3].strip(),
                    ],
                    "line_num": idx,
                }
                for_stack.append(key)
                i += 1
            elif "for.end" in line:
                res_cond = for_stack.pop()
                assert res_cond in for_blocks_info
                for_blocks_info[res_cond]["end"] = (idx, line)

    for for_l, for_l_value in for_blocks_info.items():
        if "cond" in for_l:
            for_start.append(for_l_value["line_num"])
            for_end.append(for_l_value["end"][0])
            for_label.append(for_l)
    for idx, start_num in enumerate(for_start):
        child_idx = idx + 1
        possible_children = []
        for s, e in zip(for_start[idx + 1 :], for_end[idx + 1 :]):
            if s > start_num and e < for_end[idx]:
                possible_children.append(for_label[child_idx])
                child_idx += 1
            else:
                break
        for_blocks_info[for_label[idx]]["possible_children"] = possible_children

    for for_l, for_l_value in for_blocks_info.items():
        possible_children = for_l_value["possible_children"]
        children = []
        i = 0
        while i < len(possible_children):
            children.append(possible_children[i])
            i += len(for_blocks_info[possible_children[i]]["possible_children"]) + 1
        for_l_value["children"] = children

    return for_blocks_info


def augment_graph_hierarchy(
    name, for_blocks_info, src_path, dst_path, csv_dict=None, node_type="block"
):
    if node_type == "block":
        gexf_file = join(src_path, f"{name}_processed_result.gexf")
        new_gexf_file = join(dst_path, f"{name}_processed_result.gexf")
        print(f"processing {gexf_file}")
        g = nx.readwrite.gexf.read_gexf(gexf_file)
        g_nx_nodes, g_nx_edges = g.number_of_nodes(), len(g.edges)
        print(f"started with {g_nx_nodes} nodes and {g_nx_edges} edges")
        new_edges = [(nid1, nid2, edata) for nid1, nid2, edata in g.edges(data=True)]
        new_nodes = [(node, ndata) for node, ndata in g.nodes(data=True)]
        g_new = nx.MultiDiGraph()
        id = g_nx_edges
        current_g_value = {}
        current_g_value["name"] = name
        current_g_value["prev_node"] = g_nx_nodes
        current_g_value["prev_edge"] = g_nx_edges

        block_ids = {}
        for for_l, for_l_value in for_blocks_info.items():
            ## get block_id of each for loop
            found = False
            print(for_l_value)
            for node, ndata in g.nodes(data=True):
                if "full_text" not in ndata:
                    continue
                if for_l_value["next_instr"][0] in ndata["full_text"]:
                    block, function = (ndata["block"], ndata["function"])
                    correct_node = 1
                    for neighbor in g.neighbors(node):
                        if (
                            for_l_value["next_instr"][1]
                            in g.nodes[neighbor]["full_text"]
                        ):
                            correct_node += 1
                        if correct_node == 2:
                            for nn in g.neighbors(neighbor):
                                if (
                                    for_l_value["next_instr"][2]
                                    in g.nodes[nn]["full_text"]
                                ):
                                    correct_node += 1
                                    break
                        if correct_node == 3:
                            break
                    if (
                        correct_node == 3
                    ):  ### FIXME: should match the number of next instrs that were saved:
                        found = True
                        block_ids[for_l] = (block, function)
                        break
            if not found:
                print(f"could not find the respective block for label {for_l}")
                raise RuntimeError()

        node_ids_block = {}
        for for_l in for_blocks_info:
            ## find pseudo node of each for loop
            for node, ndata in g.nodes(data=True):
                if "pseudo_block" not in ndata["text"]:
                    continue
                if (
                    ndata["block"] == block_ids[for_l][0]
                    and ndata["function"] == block_ids[for_l][1]
                ):
                    node_ids_block[for_l] = node
                    break

        for for_l, for_l_value in for_blocks_info.items():
            ## connect hierarchy edges
            if len(for_l_value["children"]) == 0:
                continue
            id1 = node_ids_block[for_l]
            position = 0
            for child in for_l_value["children"]:
                id2 = node_ids_block[child]
                e_dict = {
                    "id": id,
                    "flow": 6,
                    "position": position,
                }  ## assign a new flow to it
                new_edges.append((id1, id2, e_dict))
                id += 1
                e_dict = {"id": id, "flow": 6, "position": position}
                new_edges.append((id2, id1, e_dict))
                id += 1
                position += 1
                print(f"edge between {for_l} with id {id1} and {child} and id {id2}")

        add_to_graph(g_new, nodes=new_nodes, edges=new_edges)
        prune_redundant_nodes(g_new)
        g_nx_nodes, g_nx_edges = g_new.number_of_nodes(), len(g_new.edges)
        print(f"ending with {g_nx_nodes} nodes and {g_nx_edges} edges")
        current_g_value["new_node"] = g_nx_nodes
        current_g_value["new_edge"] = g_nx_edges
        if csv_dict:
            csv_dict[name] = current_g_value
        nx.write_gexf(g_new, new_gexf_file)

    else:
        raise NotImplementedError()


def add_auxiliary_nodes(
    name, path, processed_path, csv_dict, node_type="block", connected=False
):
    if node_type == "block":
        gexf_file = join(path, f"{name}_processed_result.gexf")
        new_gexf_file = join(processed_path, f"{name}_processed_result.gexf")
        if not isfile(gexf_file):
            return None
        print(f"processing {gexf_file}")
        g = nx.readwrite.gexf.read_gexf(gexf_file)
        g_nx_nodes, g_nx_edges = g.number_of_nodes(), len(g.edges)
        print(f"started with {g_nx_nodes} nodes and {g_nx_edges} edges")
        current_g_value = {}
        current_g_value["name"] = name
        current_g_value["prev_node"] = g_nx_nodes
        current_g_value["prev_edge"] = g_nx_edges
        orig_nodes = g_nx_nodes
        block_nodes = {}
        new_edges = [(nid1, nid2, edata) for nid1, nid2, edata in g.edges(data=True)]
        new_nodes = [(node, ndata) for node, ndata in g.nodes(data=True)]
        block_func = {}
        block_func = {}
        max_block = 0
        g_new = nx.MultiDiGraph()
        id = g_nx_edges

        for node, ndata in g.nodes(data=True):
            if (
                f"function-{ndata['function']}-block-{ndata['block']}"
                not in block_nodes
            ):
                new_node = create_pseudo_node_block(ndata["block"], ndata["function"])
                block_nodes[f"function-{ndata['function']}-block-{ndata['block']}"] = {
                    "id": g_nx_nodes,
                    "node": new_node,
                    "last_position": 0,
                }
                new_nodes.append((g_nx_nodes, new_node.get_attr(after_process=True)))
                g_nx_nodes += 1

            if ndata["function"] not in block_func:
                block_func[ndata["function"]] = {}
                block_func[ndata["function"]]["count"] = 1
                block_func[ndata["function"]]["blocks"] = [ndata["block"]]
            else:
                if ndata["block"] not in block_func[ndata["function"]]["blocks"]:
                    block_func[ndata["function"]]["count"] += 1
                    block_func[ndata["function"]]["blocks"].append(ndata["block"])

            key = f"function-{ndata['function']}-block-{ndata['block']}"
            pseudo_node = block_nodes[key]["node"]
            pseudo_id = block_nodes[key]["id"]
            pseudo_position = block_nodes[key]["last_position"]
            assert pseudo_node.function == ndata["function"]
            e_dict = {"id": id, "flow": 4, "position": pseudo_position}
            new_edges.append((node, pseudo_id, e_dict))
            id += 1
            e_dict = {"id": id, "flow": 4, "position": pseudo_position}
            new_edges.append((pseudo_id, node, e_dict))
            id += 1
            block_nodes[key]["last_position"] = pseudo_position + 1

        if connected:
            ## add edge between the new nodes
            sorted_nodes = sorted(block_nodes.keys(), key=natural_keys)
            for idx, node in enumerate(sorted_nodes[:-1]):
                id1 = block_nodes[node]["id"]
                id2 = block_nodes[sorted_nodes[idx + 1]]["id"]
                e_dict = {
                    "id": id,
                    "flow": 5,
                    "position": 0,
                }  ## assign a new flow to it
                new_edges.append((id1, id2, e_dict))
                id += 1
                e_dict = {"id": id, "flow": 5, "position": 0}
                new_edges.append((id2, id1, e_dict))
                id += 1

        add_to_graph(g_new, nodes=new_nodes, edges=new_edges)
        prune_redundant_nodes(g_new)
        g_nx_nodes, g_nx_edges = g_new.number_of_nodes(), len(g_new.edges)
        for f, b in block_func.items():
            # print(f, b)
            max_block += b["count"]
        assert g_nx_nodes == orig_nodes + max_block
        print(
            f"ending with {g_nx_nodes} nodes and {g_nx_edges} edges, max block: {max_block}"
        )
        current_g_value["new_node"] = g_nx_nodes
        current_g_value["new_edge"] = g_nx_edges
        current_g_value["block"] = max_block
        if csv_dict:
            csv_dict[name] = current_g_value
        nx.write_gexf(g_new, new_gexf_file)

    else:
        raise NotImplementedError()


def remove_extra_header(src_dir, kernel_name):
    from tempfile import mkstemp

    orig_file = join(src_dir, f"{kernel_name}.c")
    fnew, abs_path = mkstemp()
    with open(fnew, "w") as fpnew:
        with open(orig_file) as fp:
            for line in fp:
                if line.startswith("#include") and "merlin_type_define" in line:
                    continue
                fpnew.write(line)

    shutil.copymode(orig_file, abs_path)
    shutil.copy(abs_path, orig_file)


def write_csv_file(csv_dict, csv_header, file_path):
    with open(join(get_root_path(), file_path), mode="w") as f:
        f_writer = csv.DictWriter(f, fieldnames=csv_header)
        f_writer.writeheader()
        for d, value in csv_dict.items():
            if d == "header":
                continue
            f_writer.writerow(value)


def run_graph_gen(
    mode="initial", connected=True, target=["machsuite", "poly"], ALL_KERNEL=ALL_KERNEL
):
    test = "original"
    global processed_gexf_folder
    if mode == "initial":
        csv_header = ["name", "num_node", "num_edge"]
    else:
        csv_header = ["name", "prev_node", "prev_edge", "new_node", "new_edge"]
    if mode == "auxiliary":
        csv_header.append("block")
    csv_dict = {"header": csv_header}
    if mode == "initial":
        for BENCHMARK in target:
            processed_gexf_folder = join(
                get_root_path(), f"{type_graph}/{BENCHMARK}/processed/"
            )
            create_dir_if_not_exists(processed_gexf_folder)
            for kernel in ALL_KERNEL[BENCHMARK]:
                print("####################")
                print("now processing", kernel)
                ## modify the path below:
                path = join(get_root_path(), f"{type_graph}/{BENCHMARK}/", f"{kernel}")
                if not exists(path):
                    create_dir_if_not_exists(path)
                    ## this file needs to be run in programl mode first for this mode to work
                    kernel_dir = join(get_root_path(), f"{BENCHMARK}/sources/**/*")
                    kernel_path = [
                        f
                        for f in iglob(kernel_dir, recursive=True)
                        if f"{kernel}_" in f
                    ]
                    if len(kernel_path) != 1:
                        if len(kernel_path) == 0:
                            print("Error: no source file found")
                        else:
                            print(
                                f"Error: multiple source files found at {kernel_path}"
                            )
                        raise RuntimeError()
                    kernel_path = kernel_path[0]
                    ext = kernel_path.split(".")[-1]
                    new_file_path = join(path, f"{kernel}.{ext}")
                    print(kernel_path)
                    print(new_file_path)
                    shutil.copyfile(kernel_path, new_file_path)
                graph_generator(
                    kernel, path, BENCHMARK, generate_programl=True, csv_dict=csv_dict
                )
                print()
        write_csv_file(csv_dict, csv_header, f"{type_graph}/{mode}.csv")
    elif mode == "auxiliary":
        for BENCHMARK in target:
            processed_gexf_folder = join(
                get_root_path(), f"{type_graph}/{BENCHMARK}/processed"
            )
            if connected:
                auxiliary_node_gexf_folder = join(
                    get_root_path(),
                    f"{type_graph}/{BENCHMARK}/processed/extended-pseudo-block-connected/",
                )
            else:
                auxiliary_node_gexf_folder = join(
                    get_root_path(),
                    f"{type_graph}/{BENCHMARK}/processed/extended-pseudo-block-base/",
                )
            create_dir_if_not_exists(auxiliary_node_gexf_folder)
            all_kernel = ALL_KERNEL[BENCHMARK]
            for kernel in all_kernel:
                print("####################")
                print("now processing", kernel)
                add_auxiliary_nodes(
                    kernel,
                    processed_gexf_folder,
                    auxiliary_node_gexf_folder,
                    csv_dict=csv_dict,
                    node_type="block",
                    connected=connected,
                )
                print()
        write_csv_file(csv_dict, csv_header, f"{type_graph}/{mode}_{connected}.csv")
    elif mode == "hierarchy":
        for BENCHMARK in target:
            llvm_base_folder = join(get_root_path(), f"{type_graph}/{BENCHMARK}/")
            auxiliary_node_gexf_folder = join(
                get_root_path(),
                f"{type_graph}/{BENCHMARK}/processed/extended-pseudo-block-connected/",
            )
            dest_path = join(
                get_root_path(),
                f"{type_graph}/{BENCHMARK}/processed/extended-pseudo-block-connected-hierarchy/",
            )
            create_dir_if_not_exists(dest_path)
            assert exists(auxiliary_node_gexf_folder)
            for kernel in ALL_KERNEL[BENCHMARK]:
                print("####################")
                print("now processing", kernel)
                for_blocks_info = get_for_blocks_info(kernel, llvm_base_folder)
                augment_graph_hierarchy(
                    kernel,
                    for_blocks_info,
                    src_path=auxiliary_node_gexf_folder,
                    dst_path=dest_path,
                    csv_dict=csv_dict,
                )
                print()
        write_csv_file(csv_dict, csv_header, f"{type_graph}/{mode}.csv")
    else:
        raise NotImplementedError()


if __name__ == "__main__":
    run_graph_gen(mode="initial", connected=True)
    run_graph_gen(mode="auxiliary", connected=False)
    run_graph_gen(mode="auxiliary", connected=True)
    run_graph_gen(mode="hierarchy", connected=True)
