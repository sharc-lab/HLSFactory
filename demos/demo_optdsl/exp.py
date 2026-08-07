from concurrent.futures import ThreadPoolExecutor, as_completed
import itertools
import json
import os
from pathlib import Path
import shutil

import joblib
import tqdm

from dotenv import load_dotenv

from hlsfactory.datasets_builtin import datasets_builder
from hlsfactory.opt_dsl_v2.opt_dsl import OptDSL
from hlsfactory.framework import Design

from models import build_model_remote_openrouter, normalize_model_name
from prompts import build_prompt_gen_optdsl_zero_shot, extract_code_xml_from_llm_output


load_dotenv() # important, needed for openrouter api key


def opt_dsl_check(txt_opt_dsl: str) -> bool:
    try:
        opt_dsl = OptDSL(txt_opt_dsl)
        if opt_dsl.opt_dsl_error:
            return False, opt_dsl.error_message
        return True, ""
    except Exception as e:
        return False, str(e)

def serialize_data(data: dict, fp: Path) -> None:
    fp.write_text(json.dumps(data, indent=4))

def eval_single(work_dir: Path, model: str, design: Design, k_index: int) -> bool:

    print(f"Evaluating: {design.name}, model: {model}, k_index: {k_index}")

    data_fp = work_dir / "eval_data_single.json"

    if work_dir.exists():
        if data_fp.exists():
            print(f"Skipping: {design.name}, model: {model}, k_index: {k_index} because it has already been evaluated")
            return
        shutil.rmtree(work_dir)
    work_dir.mkdir(parents=True)


    data = {}
    data["design"] = design.name
    data["model"] = model
    data["k_index"] = k_index


    design_dir = work_dir / design.name
    design_dir.mkdir(parents=True)
    shutil.copytree(design.dir, design_dir, dirs_exist_ok=True)


    source_files = sorted((design_dir / "src").glob("*"))

    # generate the opt_dsl
    prompt = build_prompt_gen_optdsl_zero_shot(source_files)
    data["prompt"] = prompt

    fp_raw_prompt = work_dir / "raw_prompt.txt"
    fp_raw_prompt.write_text(prompt)

    llm = build_model_remote_openrouter(model, api_key=os.getenv("OPENROUTER_KEY"))
    try:
        r = llm.llm.prompt(prompt)
        r._force()
        raw_output = r.text()
        assert raw_output is not None
    except Exception as e:
        data["can_call_llm"] = False
        serialize_data(data, data_fp)
        return
    data["can_call_llm"] = True

    fp_raw_output = work_dir / "raw_response.txt"
    fp_raw_output.write_text(raw_output)

    try:
        extracted_opt_dsl = extract_code_xml_from_llm_output(raw_output)
    except Exception as e:
        data["can_extract_opt_dsl"] = False
        serialize_data(data, data_fp)
        return

    # make sure there is one and only one key in the extracted_opt_dsl
    if len(extracted_opt_dsl) != 1:
        data["can_extract_opt_dsl"] = False
        serialize_data(data, data_fp)
        return
    opt_dsl_key = list(extracted_opt_dsl.keys())[0]
    extracted_opt_dsl = extracted_opt_dsl[opt_dsl_key]
    data["extracted_opt_dsl"] = extracted_opt_dsl
    data["can_extract_opt_dsl"] = True

    fp_extracted_opt_dsl = work_dir / "extracted_opt_dsl.txt"
    fp_extracted_opt_dsl.write_text(extracted_opt_dsl)

    # check the opt_dsl
    try:
        is_valid_opt_dsl, error_message = opt_dsl_check(extracted_opt_dsl)
    except Exception as e:
        data["can_check_opt_dsl"] = False
        serialize_data(data, data_fp)
        return
    data["can_check_opt_dsl"] = True
    data["is_valid_opt_dsl"] = is_valid_opt_dsl

    serialize_data(data, data_fp)


MODELS = [
    "openai/gpt-oss-20b",
    "openai/gpt-oss-120b",
]

N_JOBS = 64

K_SAMPLES = 10

if __name__ == "__main__":
    dir_current = Path(__file__).parent

    dir_eval_work_dir = dir_current / "eval_work_dir"
    dir_eval_work_dir.mkdir(parents=True, exist_ok=True)

    # load polybench_designs
    dir_designs = dir_current / "hls_dataset_sources"
    if dir_designs.exists():
        shutil.rmtree(dir_designs)
    dataset_polybench_designs = datasets_builder(dir_designs, ["polybench"], dataset_labels=["polybench"])
    dataset_polybench = dataset_polybench_designs["polybench"]
    designs_polybench = dataset_polybench.designs

    combos = sorted(list(itertools.product(MODELS, designs_polybench, range(K_SAMPLES))), key=lambda x: x[1].name)

    def eval_single_wrapper(combo: tuple[str, Design, int]) -> None:
        model, design, k_index = combo
        work_dir = dir_eval_work_dir / f"{normalize_model_name(model)}__{design.name}__{k_index}"
        eval_single(work_dir, model, design, k_index)

    # use threadpool with as_completed and tqdm to evaluate the combos
    with ThreadPoolExecutor(max_workers=N_JOBS) as executor:
        futures = [executor.submit(eval_single_wrapper, combo) for combo in combos]
        for future in tqdm.tqdm(as_completed(futures), total=len(futures)):
            future.result()