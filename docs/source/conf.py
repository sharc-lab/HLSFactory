import os
import sys
from pathlib import Path

CURRENT_DIR = Path(__file__).resolve().parent

sys.path.insert(0, CURRENT_DIR.parent.as_posix())
sys.path.insert(0, CURRENT_DIR.parent.parent.as_posix())

project = "HLSFactory"
author = "HLSFactory Authors"

main_doc = "index"

# html_theme = "sphinx_rtd_theme"
# html_theme_options = {
#     "collapse_navigation": False,
# }

html_theme = "furo"
html_theme_options = {
    "sidebar_hide_name": True,
    "light_logo": "hlsfactory_logo.png",
    "dark_logo": "hlsfactory_logo_dark.png",
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

github_url = "https://github.com/sharc-lab/hlsfactory"

html_show_sphinx = False

extensions = [
    "sphinx.ext.autodoc",
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
