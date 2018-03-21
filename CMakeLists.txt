cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)


# Uebersetzung des Scanners aus Aufgabe 1
project(lexmain)
add_executable(lexmain lexmain.cxx init.cxx error.cxx lexan.cxx symbol.cxx parser.cxx global.h)


# Uebersetzung des Parsers aus Aufgabe 1
project(parsemain)
add_executable(parsemain parsemain.cxx init.cxx error.cxx lexan.cxx symbol.cxx parser.cxx global.h)
