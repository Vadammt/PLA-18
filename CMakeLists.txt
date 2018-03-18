cmake_minimum_required(VERSION 3.8)
project(Solution)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        error.cxx
        global.h
        init.cxx
        lexan.cxx
        lexmain.cxx
        Makefile
        parsemain.cxx
        parser.cxx
        symbol.cxx)

add_executable(Solution ${SOURCE_FILES})