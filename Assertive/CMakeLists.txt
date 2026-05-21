# CMakeList.txt : CMake project for Assertive, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# --- <format> requirement ---------------------------------------------------
# The library formats assertion operands via std::format, which is the C++20
# standard customization point (users specialize std::formatter<T> for their
# types). <format> requires GCC 13+ / libstdc++ 13+ / libc++ 14+ / MSVC 17.x+;
# older toolchains fail configuration here with a clear upgrade message.
include(CheckCXXSourceCompiles)
set(_assertive_probe_flags_saved "${CMAKE_REQUIRED_FLAGS}")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} /std:c++20")
else()
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++20")
endif()
check_cxx_source_compiles("
#include <format>
#if !defined(__cpp_lib_format) || __cpp_lib_format < 201907L
#  error std::format feature macro not advertised
#endif
int main() { (void)std::format(\"{}\", 42); return 0; }
" _assertive_have_std_format)
set(CMAKE_REQUIRED_FLAGS "${_assertive_probe_flags_saved}")

if(NOT _assertive_have_std_format)
    message(FATAL_ERROR
        "Assertive requires std::format support. "
        "Upgrade to GCC 13+ / libstdc++ 13+ / libc++ 14+ / MSVC 17.x+, "
        "or use an older release of this library that predates the std::format-based operand formatting.")
endif()

# Add source to this project's executable.
add_executable (Assertive "Assertive.cpp" "Assertive.h")

set_property(TARGET Assertive PROPERTY CXX_STANDARD 20)
# TODO: Add tests and install targets if needed.
