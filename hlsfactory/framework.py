import enum
import multiprocessing
import shutil
from abc import ABC, abstractmethod
from collections import defaultdict
from collections.abc import Callable
from enum import Enum
from functools import partial
from pathlib import Path

import psutil
import tqdm

EXTENSIONS_CPP = [".cpp", ".cc", ".hpp", ".h"]
EXTENTIONS_TCL = [".tcl"]
EXTENSIONS_PYTHON = [".py"]

NAMES_MAKEFILE = ["Makefile", "makefile"]
EXTENTIONS_MAKEFILE = [".mk"]

# EXTENSIONS_JINJA = [".jinja", ".jinja2"]


def all_files_in_dir(dir_path: Path) -> list[Path]:
    return [file for file in dir_path.rglob("*") if file.is_file()]


def filter_files_by_ext(files: list[Path], ext: str) -> list[Path]:
    return [file for file in files if file.suffix == ext]


class DesignStage(Enum):
    ABSTRACT = enum.auto()
    CONCRETE = enum.auto()


class Design:
    def __init__(self, name: str, dir_path: Path) -> None:
        self.name = name
        self.dir = dir_path

    def __repr__(self) -> str:
        return f"Design(name={self.name}, dir={self.dir})"

    @property
    def all_files(self) -> list[Path]:
        return all_files_in_dir(self.dir)

    @property
    def tcl_files(self) -> list[Path]:
        return filter_files_by_ext(self.all_files, ".tcl")

    @property
    def cpp_source_files(self) -> list[Path]:
        source_files = []
        for ext in EXTENSIONS_CPP:
            source_files.extend(filter_files_by_ext(self.all_files, ext))
        return source_files

    def rename(self, new_name: str) -> "Design":
        self.dir.rename(self.dir.parent / new_name)
        self.dir = self.dir.parent / new_name
        self.name = new_name
        return self

    def move_to_new_parent_dir(self, new_parent_dir: Path) -> "Design":
        shutil.move(self.dir, new_parent_dir / self.name)
        self.dir = new_parent_dir / self.name
        return self

    def copy_to_new_parent_dir(self, new_parent_dir: Path) -> "Design":
        shutil.copytree(self.dir, new_parent_dir / self.name)
        self.dir = new_parent_dir / self.name
        return self

    def copy_and_rename_to_new_parent_dir(
        self,
        new_name: str,
        new_parent_dir: Path,
    ) -> "Design":
        new_dir = new_parent_dir / new_name
        if new_dir.exists():
            shutil.rmtree(new_dir)
        shutil.copytree(self.dir, new_dir)
        return Design(new_name, new_dir)


class DesignDataset:
    def __init__(self, name: str, dataset_dir: Path, designs: list[Design]) -> None:
        self.name = name
        self.dataset_dir = dataset_dir
        self.designs = designs

    def __repr__(self) -> str:
        return f"DesignDataset(name={self.name}, dataset_dir={self.dataset_dir}, len(designs)={len(self.designs)})"

    @classmethod
    def from_dir(
        cls,
        name: str,
        dir_path: Path,
        exclude_dir_filter: None | Callable[[Path], bool] = None,
    ) -> "DesignDataset":
        designs = []
        for sub_dir in dir_path.iterdir():
            if sub_dir.is_dir():
                if exclude_dir_filter is not None and exclude_dir_filter(sub_dir):
                    continue
                designs.append(Design(sub_dir.name, sub_dir))
        designs = sorted(designs, key=lambda design: design.name)
        return DesignDataset(name, dir_path, designs)

    @classmethod
    def from_empty_dir(cls, name: str, work_dir: Path) -> "DesignDataset":
        dataset_dir = work_dir / name
        if dataset_dir.exists():
            shutil.rmtree(dataset_dir)
        dataset_dir.mkdir()
        return DesignDataset(name, dataset_dir, [])

    def add_design(self, design: Design) -> "DesignDataset":
        self.designs.append(design)
        return self

    def add_designs(self, designs: list[Design]) -> "DesignDataset":
        self.designs.extend(designs)
        return self

    def add_design_copy(self, design: Design) -> "DesignDataset":
        new_design = design.copy_and_rename_to_new_parent_dir(
            design.name,
            self.dataset_dir,
        )
        self.add_design(new_design)
        return self

    def add_designs_copy(self, designs: list[Design]) -> "DesignDataset":
        for design in designs:
            self.add_design_copy(design)
        return self

    def copy_dataset(self, work_dir: Path) -> "DesignDataset":
        new_dataset = DesignDataset.from_empty_dir(self.name, work_dir)
        for design in self.designs:
            new_design = design.copy_and_rename_to_new_parent_dir(
                design.name,
                new_dataset.dataset_dir,
            )
            new_dataset.add_design(new_design)
        return new_dataset

    def copy_and_rename(self, new_name: str, work_dir: Path) -> "DesignDataset":
        new_dataset = DesignDataset.from_empty_dir(new_name, work_dir)
        for design in self.designs:
            new_design = design.copy_and_rename_to_new_parent_dir(
                design.name,
                new_dataset.dataset_dir,
            )
            new_dataset.add_design(new_design)
        return new_dataset


def check_n_jobs_cpu_affinity(n_jobs: int, cpu_affinity: list[int] | None) -> None:
    if cpu_affinity is not None and len(cpu_affinity) != n_jobs:
        msg = f"Ideally cpu_affinity must have the same length as n_jobs if cpu_affinity is specified, len(cpu_affinity)={len(cpu_affinity)} != n_jobs={n_jobs}"
        raise ValueError(msg)


DesignDatasetCollection = dict[str, DesignDataset]


def count_designs_in_dataset_collection(
    design_datasets: DesignDatasetCollection,
) -> dict[str, int]:
    return {name: len(dataset.designs) for name, dataset in design_datasets.items()}


def count_total_designs_in_dataset_collection(
    design_datasets: DesignDatasetCollection,
) -> int:
    return sum(count_designs_in_dataset_collection(design_datasets).values())


def worker_init(core_queue: multiprocessing.Queue) -> None:
    worker_core = core_queue.get()
    current_process = psutil.Process()
    current_process.cpu_affinity([worker_core])


class Flow(ABC):
    name: str

    def __init__(self, work_dir: Path) -> None:
        self.work_dir = work_dir

    @abstractmethod
    def execute(self, design: Design, timeout: float | None = None) -> list[Design]: ...

    def execute_multiple_designs(
        self,
        designs: list[Design],
        n_jobs: int = 1,
        cpu_affinity: list[int] | None = None,
        timeout: float | None = None,
    ) -> list[Design]:
        check_n_jobs_cpu_affinity(n_jobs, cpu_affinity)

        if cpu_affinity is None:
            pool = multiprocessing.Pool(n_jobs)
        else:
            cores_to_use = multiprocessing.Queue()
            for core in cpu_affinity:
                cores_to_use.put(core)

            pool = multiprocessing.Pool(
                n_jobs,
                initializer=worker_init,
                initargs=(cores_to_use,),
            )

        new_designs_lists = pool.map(
            partial(self.execute, timeout=timeout),
            tqdm.tqdm(designs),
            chunksize=1,
        )
        pool.close()
        pool.join()
        new_designs = [design for sublist in new_designs_lists for design in sublist]
        return new_designs

    def default_new_dataset_name_fn(self) -> Callable[[str], str]:
        return lambda x: f"{x}_post_{self.name}"

    def execute_multiple_design_datasets_naive_parallel(
        self,
        design_datasets: DesignDatasetCollection,
        copy_dataset: bool,
        new_dataset_name_fn: Callable[[str], str] | None = None,
        n_jobs: int = 1,
        cpu_affinity: list[int] | None = None,
        timeout: float | None = None,
    ) -> DesignDatasetCollection:
        check_n_jobs_cpu_affinity(n_jobs, cpu_affinity)

        new_design_datasets: dict[str, list[Design]] = defaultdict(list)
        for dataset_name, design_dataset in design_datasets.items():
            new_designs = self.execute_multiple_designs(
                design_dataset.designs,
                n_jobs=n_jobs,
                cpu_affinity=cpu_affinity,
                timeout=timeout,
            )
            new_design_datasets[dataset_name].extend(new_designs)

        if not copy_dataset:
            return {
                dataset_name: DesignDataset(
                    dataset_name,
                    design_dataset.dataset_dir,
                    designs,
                )
                for dataset_name, designs in new_design_datasets.items()
            }
        if new_dataset_name_fn is None:
            new_dataset_name_fn = self.default_new_dataset_name_fn()

        new_design_datasets_copied = {}
        old_dataset_names = list(set(design_datasets.keys()))
        new_dataset_names = list(map(new_dataset_name_fn, old_dataset_names))

        for old_name, new_name in zip(
            old_dataset_names,
            new_dataset_names,
            strict=False,
        ):
            if old_name == new_name:
                raise ValueError(
                    f"Old and new dataset names are the same, {old_name}=={new_name}, the new_dataset_name_fn must provide a unique renaming",
                )

        for old_name, new_name in zip(
            old_dataset_names,
            new_dataset_names,
            strict=False,
        ):
            d_new = DesignDataset.from_empty_dir(new_name, self.work_dir)
            for design in new_design_datasets[old_name]:
                design.move_to_new_parent_dir(d_new.dataset_dir)
                d_new.add_design(design)
            new_design_datasets_copied[new_name] = d_new

        return new_design_datasets_copied

    def execute_multiple_design_datasets_fine_grained_parallel(
        self,
        design_datasets: DesignDatasetCollection,
        copy_dataset: bool,
        new_dataset_name_fn: Callable[[str], str] | None = None,
        n_jobs: int = 1,
        cpu_affinity: list[int] | None = None,
        par_chunksize: int | None = 1,
        timeout: float | None = None,
    ) -> DesignDatasetCollection:
        check_n_jobs_cpu_affinity(n_jobs, cpu_affinity)

        designs = []
        dataset_names = []
        for design_dataset_name, design_dataset in design_datasets.items():
            for design in design_dataset.designs:
                designs.append(design)
                dataset_names.append(design_dataset_name)

        if cpu_affinity is None:
            pool = multiprocessing.Pool(n_jobs)
        else:
            cores_to_use = multiprocessing.Queue()
            for core in cpu_affinity:
                cores_to_use.put(core)

            pool = multiprocessing.Pool(
                n_jobs,
                initializer=worker_init,
                initargs=(cores_to_use,),
            )

        new_designs_lists = pool.map(
            partial(self.execute, timeout=timeout),
            tqdm.tqdm(designs),
            chunksize=par_chunksize,
        )
        pool.close()
        pool.join()

        if new_designs_lists is None:
            raise ValueError("new_designs_lists is None")

        designs_collected: dict[str, list[Design]] = defaultdict(list)
        for designs, dataset_name in zip(
            new_designs_lists,
            dataset_names,
            strict=False,
        ):
            designs_collected[dataset_name].extend(designs)

        if not copy_dataset:
            return {
                dataset_name: DesignDataset(
                    dataset_name,
                    design_dataset.dataset_dir,
                    designs,
                )
                for dataset_name, designs in designs_collected.items()
            }
        if new_dataset_name_fn is None:
            new_dataset_name_fn = self.default_new_dataset_name_fn()

        new_design_datasets_copied: dict[str, DesignDataset] = {}
        old_dataset_names = list(set(designs_collected.keys()))
        new_dataset_names = list(map(new_dataset_name_fn, old_dataset_names))

        for old_name, new_name in zip(
            old_dataset_names,
            new_dataset_names,
            strict=False,
        ):
            if old_name == new_name:
                raise ValueError(
                    f"Old and new dataset names are the same, "
                    f"{old_name}=={new_name}, "
                    f"the new_dataset_name_fn must provide a unique renaming",
                )

        for old_name, new_name in zip(
            old_dataset_names,
            new_dataset_names,
            strict=False,
        ):
            d_new = DesignDataset.from_empty_dir(new_name, self.work_dir)
            for design in designs_collected[old_name]:
                design.move_to_new_parent_dir(d_new.dataset_dir)
                d_new.add_design(design)
            new_design_datasets_copied[new_name] = d_new

        return new_design_datasets_copied


class Frontend(Flow): ...


class ToolFlow(Flow): ...
