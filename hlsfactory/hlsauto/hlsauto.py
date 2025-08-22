import re
from copy import deepcopy
from dataclasses import dataclass, field
from itertools import product
from pathlib import Path
from typing import Self

RE_PARAM_SET = re.compile(
    r"^auto\{\s*?([a-zA-Z0-9_]+?(?:\s*?,\s*?[a-zA-Z0-9_]+?)*?)\s*?\}(?:\[(\w+?)\])?$"
)


@dataclass
class HLSAutoParamSet:
    # auto{a, b, ...} or auto{a, b, ...}[my_group]
    values: list[str] = field(default_factory=list)
    group: str | None = None

    @classmethod
    def parse(cls, txt: str) -> Self:
        txt = txt.strip()

        match = RE_PARAM_SET.match(txt.strip())
        if not match:
            raise ValueError(f"Invalid param set format: {txt}")

        values_str, group = match.groups()
        values = [v.strip() for v in values_str.split(",") if v.strip()]
        if len(values) == 0:
            raise ValueError(
                f"No valid values found in param set: {txt}\n"
                f"If you don't want to parameterize, just don't use the `auto{...}` syntax."
            )
        return cls(values=values, group=group)


@dataclass
class HLSAutoDefinitionPragma:
    pragma_name: str
    args_const: dict[int, str] = field(
        default_factory=dict
    )  # the key is just the "index" from enumarating the args
    args_paramaterized: dict[int, HLSAutoParamSet] = field(
        default_factory=dict
    )  # the key is just the "index" from enumarating the args
    kwargs_const: dict[str, str] = field(default_factory=dict)
    kwargs_paramaterized: dict[str, HLSAutoParamSet] = field(default_factory=dict)

    @classmethod
    def parse(cls, txt: str) -> Self:
        txt = txt.strip()

        # assert that the prefix is #pragma HLS_AUTO or #PRAGMA HLS_AUTO
        if not txt.startswith("#pragma HLS_AUTO") and not txt.startswith(
            "#PRAGMA HLS_AUTO"
        ):
            raise ValueError(
                f"Text does not start with a valid HLS_AUTO pragma: {txt}\n"
                f"Expected to start with '#pragma HLS_AUTO' or '#PRAGMA HLS_AUTO'."
            )

        # remove the prefix
        if txt.startswith("#pragma HLS_AUTO"):
            txt = txt.removeprefix("#pragma HLS_AUTO").strip()
        elif txt.startswith("#PRAGMA HLS_AUTO"):
            txt = txt.removeprefix("#PRAGMA HLS_AUTO").strip()

        if not txt:
            raise ValueError(
                f"Pragma definition is empty: {txt}\n"
                f"Expected format: '#pragma HLS_AUTO <pragma_name> [<args>]'"
            )

        parts = txt.split(maxsplit=1)
        pragma_name = parts[0].strip()
        if re.match(r"^[a-zA-Z0-9_]+$", pragma_name) is None:
            raise ValueError(
                f"Invalid pragma name: '{pragma_name}'\n"
                f"Pragma names must consist of alphanumeric characters and underscores."
            )
        remaining = parts[1] if len(parts) > 1 else ""

        # parse args and kwargs
        args_const = []
        args_paramaterized = []
        kwargs_const = {}
        kwargs_paramaterized = {}

        if remaining:
            # Parse using a token-based approach with proper handling of auto{...} blocks
            tokens = []
            i = 0
            start = 0
            in_brace = False
            brace_level = 0

            # Tokenize the string
            while i < len(remaining):
                char = remaining[i]

                if char == "{":
                    in_brace = True
                    brace_level += 1
                elif char == "}":
                    brace_level -= 1
                    if brace_level == 0:
                        in_brace = False

                # If we're at whitespace and not inside braces, end the current token
                if char.isspace() and not in_brace and brace_level == 0:
                    if i > start:
                        tokens.append(remaining[start:i])
                    start = i + 1

                i += 1

            # Add the last token if there is one
            if start < len(remaining):
                tokens.append(remaining[start:])

            # Process tokens
            for token in tokens:
                if "=" in token:
                    # It's a key=value pair
                    key, value = token.split("=", 1)

                    if not key or not value:
                        raise ValueError(f"Invalid key-value pair: {token}")

                    if value.startswith("auto{"):
                        try:
                            param_set = HLSAutoParamSet.parse(value)
                            kwargs_paramaterized[key] = param_set
                        except ValueError as e:
                            # Re-raise the error from HLSAutoParamSet.parse
                            raise ValueError(
                                f"In pragma argument '{key}': {e!s}"
                            ) from e
                    else:
                        kwargs_const[key] = value
                # It's a positional argument
                elif token.startswith("auto{"):
                    try:
                        param_set = HLSAutoParamSet.parse(token)
                        args_paramaterized.append(param_set)
                    except ValueError as e:
                        # Re-raise the error from HLSAutoParamSet.parse
                        raise ValueError(f"In positional argument: {e!s}") from e
                else:
                    args_const.append(token)

        args_const_dict = dict(enumerate(args_const))
        args_paramaterized_dict = dict(enumerate(args_paramaterized))

        return cls(
            pragma_name=pragma_name,
            args_const=args_const_dict,
            args_paramaterized=args_paramaterized_dict,
            kwargs_const=kwargs_const,
            kwargs_paramaterized=kwargs_paramaterized,
        )


@dataclass
class HLSAutoConcretePragma:
    pragma_name: str
    args: list[str] = field(default_factory=list)
    kwargs: dict[str, str] = field(default_factory=dict)

    def emit(self) -> str:
        txt = f"#pragma HLS_AUTO {self.pragma_name}"
        if self.args:
            txt += " " + " ".join(self.args)
        if self.kwargs:
            txt += " " + " ".join(f"{k}={v}" for k, v in self.kwargs.items())
        return txt


@dataclass
class HLSAutoPoint:
    pragmas: list[HLSAutoConcretePragma] = field(default_factory=list)
    # TODO: defines
    # TODO: constants


def hlsauto_parse(txt: str) -> dict[int, HLSAutoDefinitionPragma]:
    definitions: dict[int, HLSAutoDefinitionPragma] = {}

    for line_idx, line in enumerate(txt.splitlines()):
        if line.strip().startswith("#pragma HLS_AUTO") or line.strip().startswith(
            "#PRAGMA HLS_AUTO"
        ):
            try:
                pragma = HLSAutoDefinitionPragma.parse(line)
                definitions[line_idx] = pragma
            except ValueError as e:
                raise ValueError(f"Error parsing line:\n'{line}'") from e

    return definitions


def hlsauto_parse_file(fp: Path) -> dict[int, HLSAutoDefinitionPragma]:
    return hlsauto_parse(fp.read_text())


T_source_text = str
T_source_id = str
T_source_line_number = int
T_source_collection = dict[str, T_source_text]
T_source_definitions = dict[str, dict[int, HLSAutoDefinitionPragma]]
T_source_definitions_unified = dict[tuple[str, int], HLSAutoDefinitionPragma]

T_source_concrete_pragmas = dict[tuple[str, int], HLSAutoConcretePragma]

T_space_key = tuple[
    str, int, str, str
]  # (source_id, line_number, "arg" | "kwarg", key)


def space_cartesian_product[K, V](
    space: dict[K, list[V]],
) -> list[dict[K, V]]:
    keys = list(space.keys())
    values_combinations = product(*(space[key] for key in keys))
    return [dict(zip(keys, values, strict=True)) for values in values_combinations]


def space_cartesian_product_grouped[K, V](
    space: dict[K, list[V]],
    group_info: dict[
        K, str | None
    ],  # Maps each key to its group name (or None if ungrouped)
) -> list[dict[K, V]]:
    # Organize keys by group
    grouped_keys: dict[str, list[K]] = {}  # Group name -> list of keys
    ungrouped_keys: list[K] = []

    # Categorize keys by their groups
    for key, group in group_info.items():
        if key in space:  # Only consider keys that are in the space
            if group:
                if group not in grouped_keys:
                    grouped_keys[group] = []
                grouped_keys[group].append(key)
            else:
                ungrouped_keys.append(key)

    # Add any keys from space that aren't in group_info (treat as ungrouped)
    for key in space:
        if key not in group_info and key not in ungrouped_keys:
            ungrouped_keys.append(key)

    # Validate that all keys in each group have the same number of values
    for group, keys in grouped_keys.items():
        value_counts = {len(space[key]) for key in keys}
        if len(value_counts) > 1:
            raise ValueError(
                f"Group '{group}' contains parameters with different numbers of values: {value_counts}"
            )

    # Generate all combinations for the ungrouped keys
    ungrouped_combinations = []
    if ungrouped_keys:
        ungrouped_values = [space[key] for key in ungrouped_keys]
        ungrouped_combinations = [
            dict(zip(ungrouped_keys, values, strict=True))
            for values in product(*ungrouped_values)
        ]
    else:
        # If there are no ungrouped keys, we need an empty dict to combine with groups
        ungrouped_combinations = [{}]

    # If there are no groups, just return the ungrouped combinations
    if not grouped_keys:
        return ungrouped_combinations

    # Generate combinations for each group (zipped)
    group_combinations: dict[str, list[dict[K, V]]] = {}
    for group, keys in grouped_keys.items():
        num_values = len(space[keys[0]])  # All keys in a group have the same length
        group_combs = []

        # Zip values within the group
        for i in range(num_values):
            comb = {}
            for key in keys:
                comb[key] = space[key][i]
            group_combs.append(comb)

        group_combinations[group] = group_combs

    # Generate all combinations between groups
    groups = list(grouped_keys.keys())
    group_indices = {group: range(len(group_combinations[group])) for group in groups}

    # Generate all combinations of group indices
    result = []

    # For each ungrouped combination
    for ungrouped_comb in ungrouped_combinations:
        # Generate all possible combinations of group indices
        group_indices_keys = list(group_indices.keys())
        group_indices_values = [group_indices[group] for group in group_indices_keys]

        # For each combination of group indices
        for indices_comb in product(*group_indices_values):
            combined = ungrouped_comb.copy()

            # Add the values from each group
            for i, group in enumerate(group_indices_keys):
                combined.update(group_combinations[group][indices_comb[i]])

            result.append(combined)

    return result


def hlsauto_paramaterize_source(
    txts: T_source_collection,
) -> list[(T_source_collection)]:
    output_txts: list[T_source_collection] = []

    definitions: T_source_definitions = {}
    for source_id, txt in txts.items():
        definitions[source_id] = hlsauto_parse(txt)

    definitions_unifnied: T_source_definitions_unified = {}
    for source_id, line_definitions in definitions.items():
        for line_number, definition in line_definitions.items():
            definitions_unifnied[(source_id, line_number)] = definition

    # we need to then create a combination of all psosobile values for diecifions
    # and then create a new source text for each combination

    # the key to the space is going to be (source_id, line_number, "arg" | "kwarg", key)
    # the values are just the parameterized values, [1,2,4,5]
    space: dict[T_space_key, list[str]] = {}
    group_info: dict[T_space_key, str | None] = {}
    for (source_id, line_number), definition in definitions_unifnied.items():
        for arg_idx, param_set in definition.args_paramaterized.items():
            key = (source_id, line_number, "arg", str(arg_idx))
            space[key] = param_set.values
            group_info[key] = param_set.group

        for kwarg_key, param_set in definition.kwargs_paramaterized.items():
            key = (source_id, line_number, "kwarg", kwarg_key)
            space[key] = param_set.values
            group_info[key] = param_set.group

        for arg_idx, value in definition.args_const.items():
            key = (source_id, line_number, "arg", str(arg_idx))
            space[key] = [value]
            group_info[key] = None

        for kwarg_key, value in definition.kwargs_const.items():
            key = (source_id, line_number, "kwarg", kwarg_key)
            space[key] = [value]
            group_info[key] = None

    # now we need to create all combinations of the values in the space
    space_points: list[dict[T_space_key, str]] = space_cartesian_product_grouped(
        space, group_info
    )

    # now we want to create a new definitions_unifnied where each HLSAutoDefinitionPragma is replaced with a concrete one
    for point in space_points:
        pragmas: T_source_concrete_pragmas = {}

        for definition_key, definition in definitions_unifnied.items():
            source_id, line_number = definition_key
            concrete_pragma = HLSAutoConcretePragma(
                pragma_name=definition.pragma_name,
            )

            # Add args
            for arg_idx, param_set in definition.args_paramaterized.items():
                key = (source_id, line_number, "arg", str(arg_idx))
                if key in point:
                    concrete_pragma.args.append(point[key])

            # Add const args
            for arg_idx, value in definition.args_const.items():
                key = (source_id, line_number, "arg", str(arg_idx))
                if key in point:
                    concrete_pragma.args.append(value)

            # Add kwargs
            for kwarg_key, param_set in definition.kwargs_paramaterized.items():
                key = (source_id, line_number, "kwarg", kwarg_key)
                if key in point:
                    concrete_pragma.kwargs[kwarg_key] = point[key]

            # Add const kwargs
            for kwarg_key, value in definition.kwargs_const.items():
                key = (source_id, line_number, "kwarg", kwarg_key)
                if key in point:
                    concrete_pragma.kwargs[kwarg_key] = value

            pragmas[(source_id, line_number)] = concrete_pragma

        # Now we need to create a new source text for each point
        new_txts: T_source_collection = deepcopy(txts)
        for (source_id, line_number), concrete_pragma in pragmas.items():
            assert source_id in new_txts

            lines = new_txts[source_id].splitlines()
            if line_number < 0 or line_number >= len(lines):
                raise ValueError(
                    f"Line number {line_number} out of range for source '{source_id}'"
                )
            # find how much whitespace is before the pragma, then place the new pragmas with the same amout of whitespace in front of the line
            whitespace_match = re.match(r"^\s*", lines[line_number])
            if not whitespace_match:
                raise ValueError(
                    f"Could not find whitespace before line {line_number} in source '{source_id}'"
                )
            whitespace = whitespace_match.group(0)
            new_pragma = whitespace + concrete_pragma.emit()
            lines[line_number] = new_pragma
            new_txts[source_id] = "\n".join(lines)

        output_txts.append(new_txts)

    return output_txts
