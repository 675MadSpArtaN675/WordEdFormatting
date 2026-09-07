cmake_minimum_required(VERSION 4.0)
project(tui_interface)

file(GLOB_RECURSE SOURCES "*.cpp")

add_executable(study_formatter ${SOURCES})
target_link_libraries(study_formatter PUBLIC word_formatter)
