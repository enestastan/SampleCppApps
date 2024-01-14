.ONESHELL:
SHELL := /bin/bash

.DEFAULT_GOAL := info

.PHONY: info help purge config reconfig rebuild
.PHONY: clean build install package test run all

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(patsubst %/,%,$(dir $(mkfile_path)))

prj_name := app_test1

# toolchain := native
# toolchain := arm-linux-gnueabi

# CMAKE_TOOLCHAIN_FILE := ${mkfile_dir}/../toolchains/${toolchain}.cmake

info:
	@cat <<- EOF
	cmake helper makefile. some options:
	├── info     : display this info message.
	├── help     : display cmake makefile help.
	├── purge    : remove all cmake project artifacts.
	├── config   : configure cmake project.
	├── reconfig : force reconfigure cmake project.
	├── rebuild  : force rebuild cmake project.
	├── clean    : clean cmake project.
	├── build    : build cmake project.
	├── install  : install cmake project.
	├── package  : package cmake project.
	├── test     : run tests of cmake project (run build first).
	├── run      : run executable of cmake project (run build first).
	└── all      : [purge, config, clean, build, install, package, test, run].
	EOF

help: config
	cmake --build "$(mkfile_dir)/build" --target help

purge:
	rm -rf "$(mkfile_dir)/build" "$(mkfile_dir)/out"

config:
	@if [ ! -f "$(mkfile_dir)/build/CMakeCache.txt" ]; then\
		make --file="$(mkfile_path)" reconfig;\
	fi

reconfig: purge
	cmake -S "$(mkfile_dir)" -B "$(mkfile_dir)/build" \
		-DCMAKE_TOOLCHAIN_FILE="$(CMAKE_TOOLCHAIN_FILE)"

rebuild: reconfig build

clean: config
	cmake --build "$(mkfile_dir)/build" --target clean

build: config
	cmake --build "$(mkfile_dir)/build" --target all

install: config
	cmake --build "$(mkfile_dir)/build" --target install

package: config
	cmake --build "$(mkfile_dir)/build" --target package

test: config
	cmake --build "$(mkfile_dir)/build" --target test/fast

run: config
	cmake --build "$(mkfile_dir)/build" --target "run_$(prj_name)/fast"

all: purge config clean build install package test run