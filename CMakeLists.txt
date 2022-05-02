cmake_minimum_required(VERSION 3.17)
project(cough_cough C)

set(CMAKE_C_STANDARD 99)
include_directories(data libs occupation parser io socket)
add_executable(cough_cough main.c libs/occupation/occupation.c libs/occupation/occupation.h libs/parser/parser.c
        libs/parser/parser.h libs/socket/socket.c libs/socket/socket.h libs/io/io.c libs/io/io.h
        from_N_processes_to_file/from_N_processes_to_file.c from_N_processes_to_file/from_N_processes_to_file.h from_N_processes_to_parent_to_file/from_N_processes_to_parent_to_file.c from_N_processes_to_parent_to_file/from_N_processes_to_parent_to_file.h from_N_processes_to_parent_to_M_processes/from_N_processes_to_parent_to_M_processes.c from_N_processes_to_parent_to_M_processes/from_N_processes_to_parent_to_M_processes.h from_socket_to_file/from_socket_to_file.c from_socket_to_file/from_socket_to_file.h)