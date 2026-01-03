# Reusable CMake utilities for the project

include_guard(GLOBAL)

# Collect all C source files under a directory (recursively)
# Usage:
#   infra_collect_c_files(<out_var> [<directory>])
# - <out_var>: variable name to receive the list (PARENT_SCOPE)
# - <directory>: base directory to search; defaults to CMAKE_CURRENT_SOURCE_DIR
function(infra_collect_c_files out_var)
    if (ARGC GREATER 1)
        set(_dir "${ARGV1}")
    else()
        set(_dir "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    file(GLOB_RECURSE _c_files CONFIGURE_DEPENDS "${_dir}/*.c")
    set(${out_var} "${_c_files}" PARENT_SCOPE)
endfunction()