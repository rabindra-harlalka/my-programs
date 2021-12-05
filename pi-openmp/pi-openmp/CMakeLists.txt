# CMakeList.txt : CMake project for pi-openmp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(OpenMP)

# Add source to this project's executable.
add_executable (pi-openmp "pi-openmp.cpp" "pi-openmp.h")

# TODO: Add tests and install targets if needed.
if(OpenMP_CXX_FOUND)
    target_link_libraries(pi-openmp PUBLIC OpenMP::OpenMP_CXX)
endif()