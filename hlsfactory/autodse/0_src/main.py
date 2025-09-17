# from design_space.container import DesignSpaceContainer
# from design_space.explorer import DesignSpaceExplorer
# from algorithm_factory.factory import get_algorithm
# from objective.objectives import OBJECTIVE_FUNCS

# # Setup design
# container = DesignSpaceContainer("my_code.cpp")
# algorithm = get_algorithm("exhaustive")  # or "random", etc.
# objective_fn = OBJECTIVE_FUNCS["area_latency_tradeoff"]

# # Run DSE
# explorer = DesignSpaceExplorer(
#     container=container,
#     algorithm=algorithm,
#     objective_fn=objective_fn
# )

# best_config = explorer.run()

# from concrete_design.concrete_design import ConcreteDesign
# design = ConcreteDesign(source="gen.cpp", build_dir="build/")
