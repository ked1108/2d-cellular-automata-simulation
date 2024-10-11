find_package(OpenMP REQUIRED)
add_library(CA cellular_automata.cpp cell.cpp node.cpp)
target_include_directories(CA PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} OpenMP::OpenMP_CXX})
