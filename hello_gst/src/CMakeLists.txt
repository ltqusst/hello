cmake_minimum_required (VERSION 2.6)
project (gst)

# some files need directly copied into the build
configure_file(${PROJECT_SOURCE_DIR}/setenv.sh ${PROJECT_BINARY_DIR}/setenv.sh COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/test_adv.ogg ${PROJECT_BINARY_DIR}/test_adv.ogg COPYONLY)


# find 3rd party packages and add them into project(for all target)
find_package(PkgConfig)
pkg_check_modules(GST REQUIRED gstreamer-1.0>=1.4)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${GST_INCLUDE_DIRS}")
link_libraries(${GST_LIBRARIES})
add_compile_options(${GST_CFLAGS})

# add the executable
add_executable (init 			init.c)
add_executable (audio_player1 	audio_player1.c)
add_executable (bt_helloworld 	bt_helloworld.c)
