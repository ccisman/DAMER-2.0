cmake_minimum_required(VERSION 3.15)
project(PDNet2_0)

set(CMAKE_CXX_STANDARD 14)

add_executable(PDNet2_0 main.cpp   v_table.cpp v_table.h y.tab.cpp y.tab.h lex.y.cpp
         expression_tree.cpp expression_tree.h AST.cpp AST.h pre_process.cpp cpn.h cpn.cpp base.cpp base.h MultiSet.h MultiSet.cpp Sort.h )

target_link_libraries(PDNet2_0 tcmalloc)