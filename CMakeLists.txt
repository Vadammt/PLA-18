cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)


# Uebersetzung des Scanners aus Aufgabe 1
project(lexmain)
add_executable(lexmain src/lexmain.cxx src/init.cxx src/error.cxx src/lexan.cxx src/symbol.cxx src/parser.cxx src/global.h)


# Uebersetzung des Parsers aus Aufgabe 1
project(parsemain)
add_executable(parsemain src/parsemain.cxx src/init.cxx src/error.cxx src/lexan.cxx src/symbol.cxx src/parser.cxx src/global.h)
