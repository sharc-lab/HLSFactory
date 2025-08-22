import re
from pathlib import Path
from pprint import pp

import pytest

from hlsfactory.hlsauto.hlsauto import (
    HLSAutoDefinitionPragma,
    HLSAutoParamSet,
    hlsauto_paramaterize_source,
    hlsauto_parse,
    hlsauto_parse_file,
)

DIR_CURRENT = Path(__file__).parent


def test_param_set_parse_simple_no_group() -> None:
    param = HLSAutoParamSet.parse("auto{a,b,c}")
    assert param.values == ["a", "b", "c"]
    assert param.group is None


def test_param_set_parse_simple_with_group() -> None:
    param = HLSAutoParamSet.parse("auto{a,b,c}[foo]")
    assert param.values == ["a", "b", "c"]
    assert param.group == "foo"


def test_param_set_parse_single_value_no_group() -> None:
    param = HLSAutoParamSet.parse("auto{abc}")
    assert param.values == ["abc"]
    assert param.group is None


def test_param_set_parse_single_value_with_group() -> None:
    param = HLSAutoParamSet.parse("auto{abc}[group1]")
    assert param.values == ["abc"]
    assert param.group == "group1"


def test_param_set_parse_with_whitespace() -> None:
    param = HLSAutoParamSet.parse("auto{  a ,  b  ,c }[grp_2]")
    assert param.values == ["a", "b", "c"]
    assert param.group == "grp_2"


def test_param_set_parse_with_underscores_and_digits() -> None:
    param = HLSAutoParamSet.parse("auto{foo_1,bar2,baz_3_4}[g1]")
    assert param.values == ["foo_1", "bar2", "baz_3_4"]
    assert param.group == "g1"


@pytest.mark.parametrize(
    "invalid",
    [
        r"auto{}",  # empty
        r"auto{   }",  # only whitespace
        r"auto",  # missing braces
        r"auto{a,b",  # missing closing brace
        r"autoa,b}",  # missing opening brace
        r"foo{a,b}",  # invalid prefix
        r"auto{a,b}[",  # unclosed group
        r"auto{a,b}[group",  # unclosed group without closing bracket
        r"auto{a,b}[]",
        r"auto{a,b}[group1,group2]",  # multiple groups not allowed
        r"auto{a,b}[group1, group2]",  # multiple groups not allowed
        r"auto{a,b}[group1][group2]",  # multiple groups not allowed
        r"auto{a,b}[   ]",
    ],
)
def test_param_set_parse_invalid(invalid: str) -> None:
    with pytest.raises(ValueError):  # noqa: PT011
        HLSAutoParamSet.parse(invalid)


def test_definition_pragma_single_positional_param() -> None:
    p = HLSAutoDefinitionPragma.parse("#pragma HLS_AUTO inline auto{on, off}")
    assert p.pragma_name == "inline"
    assert p.args_const == {}
    assert len(p.args_paramaterized) == 1
    assert isinstance(p.args_paramaterized[0], HLSAutoParamSet)
    assert p.args_paramaterized[0].values == ["on", "off"]
    assert p.kwargs_const == {}
    assert p.kwargs_paramaterized == {}


def test_definition_pragma_single_keyword_param() -> None:
    p = HLSAutoDefinitionPragma.parse("#pragma HLS_AUTO pipeline II=auto{1,2,4}")
    assert p.pragma_name == "pipeline"
    assert p.args_const == {}
    assert p.args_paramaterized == {}
    assert p.kwargs_const == {}
    assert "II" in p.kwargs_paramaterized
    param = p.kwargs_paramaterized["II"]
    assert param.values == ["1", "2", "4"]
    assert param.group is None


def test_definition_pragma_keyword_param_with_group() -> None:
    p = HLSAutoDefinitionPragma.parse(
        "#pragma HLS_AUTO array_partition variable=a type=cyclic factor=auto{1,2,4,8}[unroll_group]"
    )
    assert p.pragma_name == "array_partition"
    assert p.args_const == {}
    assert p.args_paramaterized == {}
    assert p.kwargs_const["variable"] == "a"
    assert p.kwargs_const["type"] == "cyclic"
    param = p.kwargs_paramaterized["factor"]
    assert param.values == ["1", "2", "4", "8"]
    assert param.group == "unroll_group"


def test_definition_pragma_multiple_args_and_kwargs() -> None:
    p = HLSAutoDefinitionPragma.parse(
        "#pragma HLS_AUTO foo bar baz auto{x, y} alpha=auto{1,2,3}[g] beta=2"
    )
    assert p.pragma_name == "foo"
    assert p.args_const == {0: "bar", 1: "baz"}
    assert len(p.args_paramaterized) == 1
    assert p.args_paramaterized[0].values == ["x", "y"]
    assert "alpha" in p.kwargs_paramaterized
    assert p.kwargs_paramaterized["alpha"].values == ["1", "2", "3"]
    assert p.kwargs_paramaterized["alpha"].group == "g"
    assert p.kwargs_const["beta"] == "2"


def test_definition_pragma_multiple_positional_param() -> None:
    p = HLSAutoDefinitionPragma.parse("#pragma HLS_AUTO test auto{1,2,3} auto{a,b,c}")
    assert p.pragma_name == "test"
    assert p.args_const == {}
    assert len(p.args_paramaterized) == 2
    assert p.args_paramaterized[0].values == ["1", "2", "3"]
    assert p.args_paramaterized[1].values == ["a", "b", "c"]


def test_definition_pragma_args_only() -> None:
    p = HLSAutoDefinitionPragma.parse("#pragma HLS_AUTO reset sequential fast")
    assert p.pragma_name == "reset"
    assert p.args_const == {0: "sequential", 1: "fast"}
    assert p.args_paramaterized == {}
    assert p.kwargs_const == {}
    assert p.kwargs_paramaterized == {}


def test_definition_pragma_no_params() -> None:
    p = HLSAutoDefinitionPragma.parse("#pragma HLS_AUTO latency")
    assert p.pragma_name == "latency"
    assert p.args_const == {}
    assert p.args_paramaterized == {}
    assert p.kwargs_const == {}
    assert p.kwargs_paramaterized == {}


def test_definition_pragma_keyword_and_positional_param() -> None:
    p = HLSAutoDefinitionPragma.parse(
        "#pragma HLS_AUTO example auto{fast,slow} mode=auto{a,b}"
    )
    assert p.pragma_name == "example"
    assert p.args_const == {}
    assert len(p.args_paramaterized) == 1
    assert p.args_paramaterized[0].values == ["fast", "slow"]
    assert "mode" in p.kwargs_paramaterized
    assert p.kwargs_paramaterized["mode"].values == ["a", "b"]


@pytest.mark.parametrize(
    "txt",
    [
        # No pragma prefix
        "HLS_AUTO foo=bar",
        "foo bar",
        "inline autofoo{on,off}",  # Missing 'auto' syntax
        "#pragma HLS_AUTO",  # Nothing after pragma
        "#pragma HLS_AUTO   ",  # Nothing meaningful
        "#pragma HLS_AUTO auto{}",  # Empty auto set
        "#pragma HLS_AUTO =auto{on,off}",  # Starts with =
        "#pragma HLS_AUTO foo=",  # Missing value after =
        "#pragma HLS_AUTO foo=auto{}",  # Empty param set for keyword
        "#pragma HLS_AUTO foo=auto{    }",  # Only whitespace in param set
        "#pragma HLS_AUTO auto{,}",  # Comma with no values
        "#pragma HLS_AUTO foo=auto{a,,b}",  # Double comma
        "#pragma HLS_AUTO foo=auto{a b}",  # Missing comma between values
        "#pragma HLS_AUTO foo=auto{a,b}[",  # Unclosed group
        "#pragma HLS_AUTO foo=auto{a,b}[group",  # Unclosed group
        "#pragma HLS_AUTO foo=auto{",  # Unclosed auto block
        "#pragma HLS_AUTO foo=}",  # Closing brace with no opening
    ],
)
def test_definition_pragma_invalid(txt) -> None:
    with pytest.raises(ValueError):
        HLSAutoDefinitionPragma.parse(txt)


TEST_DESIGNS_FP = Path(__file__).parent / "hls_auto_test_designs.md"
# parse out all ```cpp blocks from the markdown file

RE_CODE_BLOCK: re.Pattern[str] = re.compile(r"```cpp\s*(.*?)\s*```", re.DOTALL)


def load_test_designs() -> list[str]:
    assert TEST_DESIGNS_FP.exists(), f"Test designs file not found: {TEST_DESIGNS_FP}"
    txt = TEST_DESIGNS_FP.read_text()

    matches = list(RE_CODE_BLOCK.finditer(txt))
    if not matches:
        return []
    test_designs = []
    for match in matches:
        code_block = match.group(1).strip()
        if code_block:
            test_designs.append(code_block)
    return test_designs


TEST_DESIGNS = load_test_designs()


@pytest.mark.parametrize(
    "test_design", TEST_DESIGNS, ids=[f"design_{i}" for i in range(len(TEST_DESIGNS))]
)
def test_hlsauto_parse(test_design: str) -> None:
    definitions = hlsauto_parse(test_design)
    assert definitions


@pytest.mark.parametrize(
    "test_design", TEST_DESIGNS, ids=[f"design_{i}" for i in range(len(TEST_DESIGNS))]
)
def test_hlsauto_paramaterize_source(test_design: str) -> None:
    source_collection = {"design": test_design}
    samples = hlsauto_paramaterize_source(source_collection)
    assert samples
