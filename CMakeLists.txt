cmake_minimum_required(VERSION 3.28)
project(Duplicatin_in_CA)

set(CMAKE_CXX_STANDARD 23)

add_executable(Duplicatin_in_CA main.cpp
        cellular_automata.cpp
        cellular_automata.h
        cell.cpp
        cell.h)
