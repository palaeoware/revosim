# -*- coding: utf-8 -*-
#
# Configuration file for the Sphinx documentation builder.
#
# This file is set up to allow the sphinx-multiproject to build the docs for envirogen and revosim at the same time

extensions = [
   "multiproject",
]

# Define the projects that will share this configuration file.
multiproject_projects = {
    "revosim/docs": {},
    "utility/envirogen/docs": {},
}

# Common options.
html_theme = "sphinx_rtd_theme"