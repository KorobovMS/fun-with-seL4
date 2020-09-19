cmake_minimum_required(VERSION 3.7.2)
project(FunWithSel4 C)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/seL4" kernel)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/seL4/libsel4" libsel4)