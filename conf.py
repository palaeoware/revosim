# -*- coding: utf-8 -*-
#
# Configuration file for the Sphinx documentation builder.
#
# This file is set up to allow the sphinx-multiproject to build the docs for envirogen and revosim from a single repository
extensions = [
   "multiproject",
]

# Define the projects that will share this configuration file.
# These have to match the project environmental variable defined in the readthedocs admin menu (readthedocs project --> admin --> environmental variables)
multiproject_projects = {
    "revosim/docs": {
         "config": {
            "project": "REvoSim",
            "html_title": "REvoSim",
            "path":  "revosim/docs",
         },
    },
    "utility/envirogen/docs": {
         "config": {
            "project": "EnviroGen",
            "html_title": "EnviroGen",
            "path":  "utility/envirogen/docs",
         },
    },
}

# Common options.
html_theme = "sphinx_rtd_theme"
