# -*- coding: utf-8 -*-
#
# Configuration file for the Sphinx documentation builder.
#
# This file is set up to allow the sphinx-multiproject to build the docs for envirogen and revosim at the same time

extensions = [
   "multiproject",
   "sphinx.ext.intersphinx",
]

#from multiproject.utils import get_project

# Define the projects that will share this configuration file.
multiproject_projects = {
    "revosim/docs": {
         #"use_config_file": False,
         "config": {
            "project": "REvoSim",
            "html_title": "REvoSim",
            "path":  "revosim/docs",
         },
    },
    "utility/envirogen/docss": {
         #"use_config_file": False,
         "config": {
            "project": "EnviroGen",
            "html_title": "EnviroGen",
            "path":  "utility/envirogen/docs",
         },
    },
}

#docset = get_project(multiproject_projects)

#locale_dirs = [
#    f"{docset}/locale/",
#]

# Common options.
html_theme = "sphinx_rtd_theme"