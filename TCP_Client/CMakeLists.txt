cmake_minimum_required(VERSION "3.16")

###############################################################################
## config #####################################################################
###############################################################################

set(debug_level 3 CACHE STRING "debug level.")
set(optimization_level 0 CACHE STRING "optimization level.")

# set CMAKE_TOOLCHAIN_FILE to use specified toolchain.
# set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../toolchains/arm-linux-gnueabi.cmake")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../toolchains/native.cmake" CACHE STRING "Toolchain file.")

# set cpp standard
set(CMAKE_CXX_STANDARD 17)

# for single-config generators (makefile is), this variable should be set.
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type.")

# set project name and version.
project("TCP_Client" VERSION "0.1.0"
    DESCRIPTION "A sample tcp client project with cmake build system."
)

# export compile commands for source code indexers.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# display project-related information.
message(NOTICE "-- CMAKE_CROSSCOMPILING = ${CMAKE_CROSSCOMPILING}")

###############################################################################
## variables ##################################################################
###############################################################################

# set project target names.
set(main_target "${PROJECT_NAME}")
set(test_target "test_${PROJECT_NAME}")

# dir variable names made unique to project since having
# the same list name with append option accross different
# projects causes appending dirs to the same list.

# set project source directories.
list(APPEND src_dirs_${PROJECT_NAME}
    "src"
)

# set project include directories.
list(APPEND inc_dirs_${PROJECT_NAME}
    ""
)

# set project test source directories.
list(APPEND test_src_dirs_${PROJECT_NAME}
    "test"
)

###############################################################################
## file globbing ##############################################################
###############################################################################

# copy dir lists to append different file extensions.
set(src_cpp_files ${src_dirs_${PROJECT_NAME}})
list(TRANSFORM src_cpp_files APPEND "/*.cpp")

set(src_c_files ${src_dirs_${PROJECT_NAME}})
list(TRANSFORM src_c_files APPEND "/*.c")

set(test_cpp_files ${test_src_dirs_${PROJECT_NAME}})
list(TRANSFORM test_cpp_files APPEND "/*.cpp")

set(test_c_files ${test_src_dirs_${PROJECT_NAME}})
list(TRANSFORM test_c_files APPEND "/*.c")

# globbing is more generic, but files can also be listed explicitly.
file(GLOB_RECURSE srcs      CONFIGURE_DEPENDS ${src_cpp_files} ${src_c_files})
file(GLOB_RECURSE srcs_test CONFIGURE_DEPENDS ${test_cpp_files} ${test_c_files})

###############################################################################
## target definitions #########################################################
###############################################################################

# global options should be set before adding a target (executable or library).
# # optional global compiler options.
# add_compile_options(
#     -std=c++17 -Wall -Wextra -Wconversion -Wfloat-equal
# )

# enable optimizations and debugging.
add_compile_options(
    -O${optimization_level} -g${debug_level}
)

# select whether this project will build an executable or a library.
add_executable("${main_target}")
# add_library("${main_target}")

# specific configurations for executables and libraries.
get_target_property(main_target_type "${main_target}" TYPE)
if(main_target_type STREQUAL "EXECUTABLE")
    if(NOT CMAKE_CROSSCOMPILING)
        add_custom_target("run_${main_target}" COMMAND "${main_target}" VERBATIM)
    else()
        add_custom_target("run_${main_target}"
            COMMAND echo "[INFO] ${main_target} is cross-compiled!" VERBATIM
        )
    endif()

    # set executable output directories.
    set(build_dir "${PROJECT_BINARY_DIR}/outputs/${CMAKE_LIBRARY_ARCHITECTURE}/binary")
    set(install_dir "${PROJECT_BINARY_DIR}/outputs/install/${CMAKE_LIBRARY_ARCHITECTURE}")

    set_target_properties("${main_target}" PROPERTIES
        # OUTPUT_NAME "${main_target}" # custom name can be given.
        SUFFIX ".elf"
        RUNTIME_OUTPUT_DIRECTORY "${build_dir}"
    )
else()
    add_custom_target("run_${main_target}"
        COMMAND echo "[INFO] ${main_target} is not an executable!" VERBATIM
    )

    # set library output directories.
    set(build_dir "${PROJECT_BINARY_DIR}/outputs/${CMAKE_LIBRARY_ARCHITECTURE}/lib")
    set(install_dir "${PROJECT_BINARY_DIR}/outputs/install/lib/${CMAKE_LIBRARY_ARCHITECTURE}/${PROJECT_NAME}")

    set_target_properties("${main_target}" PROPERTIES
        OUTPUT_NAME "${main_target}_${CMAKE_LIBRARY_ARCHITECTURE}" # custom name can be given.
        PREFIX "lib"
        ARCHIVE_OUTPUT_DIRECTORY "${build_dir}"
        LIBRARY_OUTPUT_DIRECTORY "${build_dir}"
    )
endif()

# add sources to project target.
target_sources("${main_target}" PRIVATE ${srcs})

# build interface requires full include path.
set(inc_dirs_full ${inc_dirs_${PROJECT_NAME}})
list(TRANSFORM inc_dirs_full PREPEND "${PROJECT_SOURCE_DIR}/")

# allow file inclusion relative to root of the src directory with a <> pair.
target_include_directories("${main_target}"
    PUBLIC "$<BUILD_INTERFACE:${inc_dirs_full}>"
    PUBLIC "$<INSTALL_INTERFACE:${inc_dirs_${PROJECT_NAME}}>"
)

# compiler definitions.
target_compile_definitions("${main_target}"
    PRIVATE PRJ_NAME="${PROJECT_NAME}"
)

# target compiler options.
target_compile_options("${main_target}"
    PUBLIC -std=c++17 -Wall -Wextra -Wconversion -Wfloat-equal
)

# enable sanitizers.
if(("${debug_level}" GREATER_EQUAL 3) AND ("${optimization_level}" EQUAL 0) AND (NOT ${CMAKE_CROSSCOMPILING}))
    message(NOTICE "-- Enabling sanitizers...")
    target_link_libraries("${main_target}"
        PRIVATE -fsanitize=address
        PRIVATE -fsanitize=undefined
        # PRIVATE -fsanitize=leak
        PRIVATE -fsanitize=signed-integer-overflow
        PRIVATE -fsanitize=bounds
    )
endif()

###############################################################################
## dependencies ###############################################################
###############################################################################

include(FetchContent)


###############################################################################
## installation ###############################################################
###############################################################################

set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")

# all install commands use the same destination.
# this allows for using paths relative to the executable.
install(TARGETS "${main_target}"
    EXPORT "${main_target}Config"
    DESTINATION "${install_dir}"
)

# export is useful for including the package from other cmake projects.
install(EXPORT "${main_target}Config"
    DESTINATION "${install_dir}/cmake"
)

###############################################################################
## packaging ##################################################################
###############################################################################

# minimal specifications for package creation.
set(CPACK_PACKAGE_NAME "${main_target}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

# packaging configuration flags.
set(CPACK_MONOLITHIC_INSTALL 1)

# this must be last.
include(CPack)

###############################################################################
## custom targets #############################################################
###############################################################################

if(PROJECT_IS_TOP_LEVEL OR NOT has_parent_dir)
    add_custom_target(graphviz_${PROJECT_NAME} ALL
        COMMAND ${CMAKE_COMMAND} "--graphviz=${PROJECT_BINARY_DIR}/dep_graphs/dots/deps.dot" "${PROJECT_SOURCE_DIR}"
        COMMAND bash -c "dot -Tsvg -O $(find ${PROJECT_BINARY_DIR}/dep_graphs/dots/ -type f ! -name *.svg)" VERBATIM
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/dep_graphs/svgs"
        COMMAND bash -c "mv ${PROJECT_BINARY_DIR}/dep_graphs/dots/*.svg ${PROJECT_BINARY_DIR}/dep_graphs/svgs/" VERBATIM
        WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
endif()
