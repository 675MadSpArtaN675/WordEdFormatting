cmake_minimum_required(VERSION 4.0)
project(tui_interface)

file(GLOB_RECURSE SOURCES "*.cpp")

find_package(ftxui REQUIRED)
find_package(Boost REQUIRED COMPONENTS headers filesystem)

add_executable(study_formatter ${SOURCES})
target_link_libraries(study_formatter PUBLIC word_formatter PRIVATE ftxui::ftxui Boost::headers Boost::filesystem)
