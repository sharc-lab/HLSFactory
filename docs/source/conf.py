import os
import sys
from pathlib import Path

CURRENT_DIR = Path(__file__).resolve().parent

sys.path.insert(0, CURRENT_DIR.parent.as_posix())
sys.path.insert(0, CURRENT_DIR.parent.parent.as_posix())

project = "HLSFactory"
author = "HLSFactory Authors"

html_theme = "furo"
html_theme_options = {
    "sidebar_hide_name": True,
    "light_logo": "hlsfactory_logo.png",
    "dark_logo": "hlsfactory_logo_dark.png",
    "source_repository": "https://github.com/sharc-lab/HLSFactory",
    "source_branch": "main",
    "source_directory": "docs/source",
}


html_static_path = ["_static"]
templates_path = ["_templates"]
html_css_files = [
    "custom.css",
]
html_title = "HLSFactory"
html_short_title = "HLSFactory"
html_favicon = "_static/favicon.ico"
# html_logo = "_static/hlsfactory_logo.png"

html_show_sphinx = False

myst_enable_extensions = ["colon_fence", "deflist"]

extensions = [
    "sphinx.ext.autosummary",
    "sphinx.ext.viewcode",
    "sphinx.ext.githubpages",
    "myst_parser",
    "sphinxext.opengraph",
    "sphinx_copybutton",
    "sphinx.ext.todo",
    "autodoc2",
]


ogp_site_url = "https://sharc-lab.github.io/HLSFactory/docs/"
ogp_image = "https://sharc-lab.github.io/HLSFactory/docs/_static/hlsfactory_logo.png"

# autosummary_generate = True

todo_include_todos = True

autodoc2_packages = [
    # "../../hlsfactory",
    {
        "path": "../../hlsfactory",
        "exclude_dirs": ["__pycache__", "hls_dataset_sources", "autodse"],
    },
]
