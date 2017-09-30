cmake_minimum_required (VERSION 2.6)
project (gtk)

# some files need directly copied into the build
# configure_file(${PROJECT_SOURCE_DIR}/setenv.sh ${PROJECT_BINARY_DIR}/setenv.sh COPYONLY)

# find 3rd party packages and add them into project(for all target)
find_package(PkgConfig)
pkg_check_modules(GTK REQUIRED gtk+-2.0)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${GTK_INCLUDE_DIRS}")
link_libraries(${GTK_LIBRARIES})
add_compile_options(${GTK_CFLAGS})

# add the executable
add_executable (hello_world 			hello_world.c)

