# CMakeList.txt : CMake project for Assertive, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (Assertive "Assertive.cpp" "Assertive.h")

set_property(TARGET Assertive PROPERTY CXX_STANDARD 20)
# TODO: Add tests and install targets if needed.
