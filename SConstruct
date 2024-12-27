#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/EzFsm/"])
sources = Glob("src/EzFsm/*.cpp")

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("addons/EzFsm/doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

bin_folder = "addons/EzFsm/bin/"
lib_name = "ez_fsm"

if env["platform"] == "macos":
    library = env.SharedLibrary(
        bin_folder + "{}.{}.{}.framework/{}}.{}.{}".format(
            lib_name, env["platform"], env["target"], lib_name, env["platform"], env["target"]
        ),
        source=sources,
    )
else:
    library = env.SharedLibrary(
        bin_folder + "{}{}{}".format(lib_name, env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)