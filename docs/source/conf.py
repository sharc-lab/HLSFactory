import os
import sys
from pathlib import Path

CURRENT_DIR = Path(__file__).resolve().parent


sys.path.insert(0, CURRENT_DIR.parent.as_posix())
sys.path.insert(0, CURRENT_DIR.parent.parent.as_posix())

project = "HLSFactory"
# copyright = "2024, TBD..."
# author = "TBD..."


extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.autosummary",
    "sphinx.ext.viewcode",
    "sphinx.ext.githubpages",
    "myst_parser",
]

templates_path = ["_templates"]
exclude_patterns = []


# html_theme = "furo"
html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]

html_css_files = [
    "custom.css",
]

html_title = "HLSFactory"
html_short_title = "HLSFactory"
html_favicon = "favicon.ico"

html_logo = "_static/hlsfactory_logo.png"

github_url = "https://github.com/sharc-lab/gnn-builder"

html_show_sphinx = False

# autosummary_generate = True
