cmake_minimum_required(VERSION 3.0)
project(SIC-XE-Assembler)

include_directories(include)

file(GLOB SOURCES "src/assembler.cpp")

add_executable(assembler ${SOURCES})
