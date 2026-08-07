from dataclasses import dataclass, field
from typing import Any

from llm import Model
from llm_openrouter import OpenRouterChat


@dataclass
class Model:
    name: str
    llm: Model | OpenRouterChat
    settings: dict[str, bool | int | str | float] = field(default_factory=dict)
    other: dict[str, Any] = field(default_factory=dict)



def build_model_remote_openrouter(
    model_name: str,
    api_key: str | None = None,
    provider: dict[str, str] | None = None,
    **kwargs,
) -> Model:
    model = OpenRouterChat(
        model_id=f"openrouter/{model_name}",
        key=api_key,
        model_name=model_name,
        api_base="https://openrouter.ai/api/v1",
        headers={"HTTP-Referer": "https://llm.datasette.io/", "X-Title": "LLM"},
    )
    settings = {}
    if "settings" in kwargs:
        settings.update(kwargs["settings"])
    if provider is not None:
        settings["provider"] = provider
    return Model(name=model_name, llm=model, settings=settings)


def normalize_model_name(model_name: str) -> str:
    return model_name.replace("/", "_").replace("-", "_").replace(" ", "_").lower()