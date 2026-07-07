cmake_minimum_required(VERSION 4.0)
project(ReportWrapper CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SUBPROJECTS_DIR "src")

file(COPY "./TestFile_1.docx" DESTINATION "src/word_format")
add_subdirectory("${SUBPROJECTS_DIR}/word_format")