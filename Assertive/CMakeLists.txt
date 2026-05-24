# CMakeList.txt : CMake project for Assertive, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# --- Stack trace backend selection -----------------------------------------
# AUTO : use std::stacktrace if available, otherwise fall back to OFF.
# STD  : require <stacktrace>. (Wired in a follow-up PR.)
# OFF  : ship the no-op default. Behavior is byte-identical to pre-stacktrace
#        builds; users may install a custom capture via set_stack_capture().
set(ASSERTIVE_STACKTRACE "AUTO" CACHE STRING
    "Stack trace backend (AUTO | STD | OFF)")
set_property(CACHE ASSERTIVE_STACKTRACE PROPERTY STRINGS AUTO STD OFF)

if(NOT ASSERTIVE_STACKTRACE MATCHES "^(AUTO|STD|OFF)$")
    message(FATAL_ERROR
        "ASSERTIVE_STACKTRACE must be AUTO, STD, or OFF (got '${ASSERTIVE_STACKTRACE}')")
endif()

# Probe for std::stacktrace availability (skipped when OFF was explicitly chosen).
#
# libstdc++ keeps the stacktrace runtime in a separate helper library; the name
# changed between releases:
#   - GCC 13 / 14 : stdc++_libbacktrace
#   - GCC 15+     : stdc++exp (merged with other experimental features)
# We try both. MSVC links std_stacktrace.lib.
set(_assertive_have_std_stacktrace OFF)
set(_assertive_stacktrace_link "")
if(NOT ASSERTIVE_STACKTRACE STREQUAL "OFF")
    include(CheckCXXSourceCompiles)
    set(_assertive_probe_flags_saved "${CMAKE_REQUIRED_FLAGS}")
    set(_assertive_probe_libs_saved "${CMAKE_REQUIRED_LIBRARIES}")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} /std:c++latest")
        set(_assertive_candidate_libs "std_stacktrace")
    else()
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++23")
        # Order matters: newer libstdc++ names first.
        set(_assertive_candidate_libs "stdc++exp" "stdc++_libbacktrace" "")
    endif()

    set(_assertive_probe_src "
#include <stacktrace>
#if !defined(__cpp_lib_stacktrace) || __cpp_lib_stacktrace < 202011L
#  error std::stacktrace feature macro not advertised
#endif
int main() { auto s = std::stacktrace::current(); return static_cast<int>(s.size()); }
")

    foreach(_lib IN LISTS _assertive_candidate_libs)
        set(CMAKE_REQUIRED_LIBRARIES "${_lib}")
        # Use a distinct cache var per candidate so re-probes aren't short-circuited.
        string(REPLACE "+" "p" _slug "${_lib}")
        string(REPLACE "" "none" _slug "${_slug}")
        if(_lib STREQUAL "")
            set(_slug "none")
        endif()
        check_cxx_source_compiles("${_assertive_probe_src}" _assertive_probe_${_slug})
        if(_assertive_probe_${_slug})
            set(_assertive_have_std_stacktrace ON)
            set(_assertive_stacktrace_link "${_lib}")
            break()
        endif()
    endforeach()

    set(CMAKE_REQUIRED_FLAGS "${_assertive_probe_flags_saved}")
    set(CMAKE_REQUIRED_LIBRARIES "${_assertive_probe_libs_saved}")
endif()

# Resolve the active backend.
if(ASSERTIVE_STACKTRACE STREQUAL "STD")
    if(NOT _assertive_have_std_stacktrace)
        message(FATAL_ERROR
            "ASSERTIVE_STACKTRACE=STD requires a toolchain with working <stacktrace>, but the probe failed. Use AUTO or OFF, or upgrade the toolchain.")
    endif()
    set(_assertive_backend "std")
elseif(ASSERTIVE_STACKTRACE STREQUAL "AUTO")
    if(_assertive_have_std_stacktrace)
        set(_assertive_backend "std")
    else()
        set(_assertive_backend "off")
    endif()
else()
    set(_assertive_backend "off")
endif()

message(STATUS "Assertive stack trace backend: ${_assertive_backend}")

# Assemble the source list.
set(_assertive_sources "Assertive.cpp" "Assertive.h")
if(_assertive_backend STREQUAL "std")
    list(APPEND _assertive_sources "stack_std.cpp")
endif()

add_executable(Assertive ${_assertive_sources})

if(_assertive_backend STREQUAL "std")
    set_property(TARGET Assertive PROPERTY CXX_STANDARD 23)
    # Link whichever helper library the probe selected (may be empty if the
    # toolchain auto-links it, e.g. some libc++ configurations).
    if(NOT _assertive_stacktrace_link STREQUAL "")
        target_link_libraries(Assertive PRIVATE ${_assertive_stacktrace_link})
    endif()
else()
    set_property(TARGET Assertive PROPERTY CXX_STANDARD 20)
endif()
# TODO: Add tests and install targets if needed.
