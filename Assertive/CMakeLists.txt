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

if(ASSERTIVE_STACKTRACE STREQUAL "STD")
    message(FATAL_ERROR
        "ASSERTIVE_STACKTRACE=STD requires the std backend, which is introduced in a follow-up PR. Use AUTO or OFF for now.")
endif()

# AUTO and OFF both ship only the no-op default in this PR; AUTO is upgraded to
# probe __cpp_lib_stacktrace in the follow-up.

# Add source to this project's executable.
add_executable (Assertive "Assertive.cpp" "Assertive.h")

set_property(TARGET Assertive PROPERTY CXX_STANDARD 20)
# TODO: Add tests and install targets if needed.
