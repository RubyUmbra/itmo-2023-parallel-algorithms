cmake_minimum_required(VERSION 3.27)

project(bfs
        VERSION 1.0.0
        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-O3)

add_library(cube_graph_generator STATIC cube_graph_generator.cpp cube_graph_generator.h types.h)

add_library(bfs_seq STATIC bfs_seq.cpp bfs_seq.h types.h)

add_library(bfs_par STATIC bfs_par.cpp bfs_par.h types.h)
target_link_libraries(bfs_par tbb)

add_executable(bfs main.cpp types.h)
target_link_libraries(bfs cube_graph_generator bfs_seq bfs_par)
