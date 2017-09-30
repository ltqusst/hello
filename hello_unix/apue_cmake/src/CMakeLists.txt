cmake_minimum_required (VERSION 2.6)
project (apue)
# The version number.
set (apue_VERSION_MAJOR 1)
set (apue_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/cmake_config.h.in"
  "${PROJECT_BINARY_DIR}/cmake_config.h"
  )

# add the binary tree to the search path for include files
# so that we will find cmake_config.h
include_directories("${PROJECT_BINARY_DIR}")


# add compile options here
#add_compile_options()

# add the executable
add_executable (apue_stat 			apue_stat.c)
add_executable (apue_helloworld 	apue_helloworld.c)
add_executable (apue_fileio 		apue_fileio.c)
add_executable (apue_fork0 			apue_fork0.c)
add_executable (apue_fork1 			apue_fork1.c)
add_executable (apue_interp 		apue_interp.c)

add_executable (apue_nice 			apue_nice.c)
target_link_libraries (apue_nice    pthread)

add_executable (apue_sig1 			apue_sig1.c)
target_link_libraries (apue_sig1    m)

add_executable (apue_sig2 			apue_sig2.c)

add_executable (apue_daemonize1 	apue_daemonize1.c apue_utils.c)

add_executable (apue_pthread1 		apue_pthread1.c)
target_link_libraries (apue_pthread1 	pthread)

add_executable (apue_noblock 		apue_noblock.c apue_utils.c)
add_executable (apue_filelock 		apue_filelock.c)

add_executable (apue_dup 		apue_dup.c)
add_executable (apue_pipe 		apue_pipe.c)

add_executable (apue_socket1 	apue_socket1.c apue_utils.c)

add_executable (apue_server1 	apue_server1.c apue_utils.c)
add_executable (apue_client1 	apue_client1.c apue_utils.c)

add_executable (apue_term1 	apue_term1.c apue_utils.c)
target_link_libraries (apue_term1 	pthread)

# add linker flags
#set_target_properties(apue_helloworld PROPERTIES LINK_FLAGS "-static")

#target_link_libraries (apue_stat  ${EXTRA_LIBS})

