cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)


# Uebersetzung des Scanners aus Aufgabe 1
project(lexmain)

add_custom_target(lexmain COMMAND make lexmain
        -C ${Scanner_SOURCE_DIR}
        CLION_EXE_DIR=${PROJECT_BINARY_DIR})



# Uebersetzung des Parsers aus Aufgabe 1
project(parsemain)

add_custom_target(parsemain COMMAND make parsemain
        -C ${Parser_SOURCE_DIR}
        CLION_EXE_DIR=${PROJECT_BINARY_DIR})
