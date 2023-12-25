cmake_minimum_required(VERSION 3.27)

#set(CMAKE_CXX_COMPILER /opt/opencilk/bin/clang++)

project(bfs
        VERSION 1.0.0
        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(lib/pctl/include)
include_directories(lib/cmdline/include)
include_directories(lib/chunkedseq/include)
include_directories(lib/cilkrts/include)

link_directories(lib/cilkrts/lib)

add_compile_options(-fcilkplus)
add_compile_options(-DUSE_CILK_PLUS_RUNTIME)
add_compile_options(-O3)

add_library(cube_graph_generator STATIC cube_graph_generator.cpp cube_graph_generator.h types.h)

add_library(bfs_seq STATIC bfs_seq.cpp bfs_seq.h types.h)

add_library(bfs_par STATIC bfs_par.cpp bfs_par.h types.h)
target_link_libraries(bfs_par cilkrts)

add_executable(bfs main.cpp types.h)
target_link_libraries(bfs cube_graph_generator bfs_seq bfs_par)
