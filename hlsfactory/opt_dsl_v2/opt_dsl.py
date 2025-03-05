class OptDSL:
    def __init__(self) -> None:
        raise NotImplementedError

    def sample_random(self, seed: int = 7) -> None:
        raise NotImplementedError

    def add_space_array_partition(self, array_name: str) -> None:
        raise NotImplementedError

    def add_space_loop_unroll(self, loop_name: str) -> None:
        raise NotImplementedError

    def add_space_loop_pipeline(self, loop_name: str) -> None:
        raise NotImplementedError

    def add_static_optimization(self, opt_name: str) -> None:
        raise NotImplementedError


def parse_opt_dsl_lang(opt_dsl_lang_source: str) -> OptDSL:
    raise NotImplementedError
