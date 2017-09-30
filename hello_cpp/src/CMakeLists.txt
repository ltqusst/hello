cmake_minimum_required (VERSION 2.6)
project (cpp11)

set(CMAKE_CXX_STANDARD 11)

# add the binary tree to the search path for include files
# so that we will find cmake_config.h
include_directories("${PROJECT_BINARY_DIR}")


# add compile options here
# add_compile_options(-std=c++11)

# add the executable
add_executable (cpp11_test1 			cpp11_test1.cpp)
add_executable (cpp11_RALL1 			cpp11_RALL1.cpp)
add_executable (cpp11_callable1 		cpp11_callable1.cpp)
add_executable (cpp11_operator1 		cpp11_operator1.cpp)

add_executable (cpp11_objmodel1 		cpp11_objmodel1.cpp)
add_executable (cpp11_objmodel2 		cpp11_objmodel2.cpp)


#target_link_libraries (apue_nice    pthread)

