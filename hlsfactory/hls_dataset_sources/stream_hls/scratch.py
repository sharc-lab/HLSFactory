import json
import shutil
from pathlib import Path
from pprint import pp

DIR_CURRENT = Path(__file__).parent

DIR_PROCESSED_DATASET = DIR_CURRENT / "____processed_dataset"


if DIR_PROCESSED_DATASET.exists():
    shutil.rmtree(DIR_PROCESSED_DATASET)
DIR_PROCESSED_DATASET.mkdir()


# find all metadata.json files

metadata_files = sorted(DIR_CURRENT.rglob("metadata.json"))
design_dirs = [f.parent for f in metadata_files]

for design_dir in design_dirs:
    shutil.copytree(
        design_dir,
        DIR_PROCESSED_DATASET / design_dir.name,
    )

design_dirs_new = sorted(DIR_PROCESSED_DATASET.glob("*"))

# for each design_dir, find the src/*_tb.cpp file
# then find and comment out the following lines
#   char* prj_path_c_str = getenv("PRJ_PATH");
#   if (prj_path_c_str == NULL) {
#     std::cerr << "Environment variable PRJ_PATH not set" << std::endl;
#     return 1;
#   }
#   std::string prj_path_str(prj_path_c_str);
#   std::cout << "PRJ_PATH: " << prj_path_str << std::endl;


for design_dir in design_dirs_new:
    src_dir = design_dir / "src"
    tb_files = sorted(src_dir.glob("*_tb.cpp"))
    if len(tb_files) != 1:
        raise ValueError(
            f"Expected 1 testbench file in {src_dir}, found {len(tb_files)}",
        )
    tb_file = tb_files[0]

    tb_file_txt = tb_file.read_text()

    print(f"Processing {tb_file}")

    lines = tb_file_txt.splitlines()
    # find the first line that contains "char* prj_path_c_str = getenv("PRJ_PATH");" or "char *prj_path_c_str = getenv("PRJ_PATH");"
    first_line_idx = None
    for i, line in enumerate(lines):
        if (
            'char* prj_path_c_str = getenv("PRJ_PATH");' in line
            or 'char *prj_path_c_str = getenv("PRJ_PATH");' in line
        ):
            first_line_idx = i
            break
    if first_line_idx is None:
        raise ValueError(
            f"Could not find line 'char* prj_path_c_str = getenv(\"PRJ_PATH\");' in {tb_file}",
        )

    # find the first line that contains "std::cout << "PRJ_PATH: " << prj_path_str << std::endl;"
    second_line_idx = None
    for i, line in enumerate(lines):
        if 'std::cout << "PRJ_PATH: " << prj_path_str << std::endl;' in line:
            second_line_idx = i
            break
    if second_line_idx is None:
        raise ValueError(
            f"Could not find line 'std::cout << \"PRJ_PATH: \" << prj_path_str << std::endl;' in {tb_file}",
        )

    distance = second_line_idx - first_line_idx
    print(f"Distance between lines: {distance}")

    # prefix all lines between with // as a c++ comment
    for i in range(first_line_idx, second_line_idx + 1):
        lines[i] = "// " + lines[i]

    # add a line right after the second line that sets prj_path_c_str = "."
    lines.insert(
        second_line_idx + 1,
        '  std::string prj_path_str = ".";',
    )

    # write the lines back to the file
    tb_file.write_text("\n".join(lines))

    # find the tcl file in the design_dir
    tcl_file = design_dir / "hls.tcl"

    # find the follwing line
    # add_files -tb src/[lindex $argv 0]_tb.cpp

    lines = tcl_file.read_text().splitlines()
    line_index = None
    for i, line in enumerate(lines):
        if "add_files -tb src/[lindex $argv 0]_tb.cpp" in line:
            line_index = i
            break
    if line_index is None:
        raise ValueError(
            f"Could not find line 'add_files -tb src/[lindex $argv 0]_tb.cpp' in {tcl_file}",
        )

    # add the followng lines under it
    # add_files -tb data/
    # add_files -tb data/*

    lines = (
        lines[: line_index + 1]
        + [
            "add_files -tb data/",
            # "add_files -tb data/*",
        ]
        + lines[line_index + 1 :]
    )

    # write the lines back to the file
    tcl_file.write_text("\n".join(lines))

    # create a new dataset_hls.tcl file in the design_dir
    # it contains the following lines

    # set project prj
    # set phase syn

    # set argv [list $project $phase]
    # set argc [llength $argv]

    # source hls.tcl

    # get the design name, from metadata.json
    meta = json.loads(
        (design_dir / "metadata.json").read_text(),
    )
    design_name = meta["kernel"]
    print(f"Design name: {design_name}")

    assert (src_dir / f"{design_name}_tb.cpp").exists()
    assert (src_dir / f"{design_name}.cpp").exists()

    txt = [
        # "set project prj",
        f"set project {design_name}",
        "set phase syn",
        "",
        "set argv [list $project $phase]",
        "set argc [llength $argv]",
        "",
        "source hls.tcl",
    ]
    dataset_hls_tcl_file = design_dir / "dataset_hls.tcl"
    dataset_hls_tcl_file.write_text("\n".join(txt))
    print(f"Created {dataset_hls_tcl_file}")

    # create a dataset_hls_csim.tcl file in the design_dir
    txt = [
        # "set project prj",
        f"set project {design_name}",
        "set phase csim",
        "",
        "set argv [list $project $phase]",
        "set argc [llength $argv]",
        "",
        "source hls.tcl",
    ]
    dataset_hls_csim_tcl_file = design_dir / "dataset_hls_csim.tcl"
    dataset_hls_csim_tcl_file.write_text("\n".join(txt))
    print(f"Created {dataset_hls_csim_tcl_file}")
