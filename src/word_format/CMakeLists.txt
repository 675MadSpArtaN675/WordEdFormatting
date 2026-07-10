cmake_minimum_required(VERSION 4.0)
project(DocxFormatter CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DUCKX_DIRECTORY "C:/Lib/duckx")
set(LIBRARIES_DIRS ${DUCKX_DIRECTORY} "C:/Lib/pugixml")

list(TRANSFORM LIBRARIES_DIRS APPEND "/include" OUTPUT_VARIABLE LIBRARIES_INCLUDES)
list(APPEND LIBRARIES_INCLUDES "${DUCKX_DIRECTORY}/include/duckx")
list(APPEND LIBRARIES_INCLUDES "${DUCKX_DIRECTORY}/include")

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${LIBRARIES_DIRS} "${DUCKX_DIRECTORY}/cmake/duckx" "/usr/local" )

find_package(Boost REQUIRED headers Log Regex)
find_package(duckx REQUIRED HINTS ${DUCKX_DIRECTORY})
find_package(pugixml CONFIG REQUIRED)

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost_VERSION: ${Boost_VERSION}")
add_definitions(-D_WIN32_WINNT=0x0602)

file(GLOB_RECURSE HEADERS "include/*.hpp")
file(GLOB SOURCES "*.cpp")

message("Header files: ${HEADERS};\nSource files: ${SOURCES}")
add_library(word_formatter SHARED ${SOURCES} ${HEADERS})

target_include_directories(word_formatter PUBLIC "include" ${LIBRARIES_INCLUDES})
target_link_libraries(word_formatter PUBLIC Boost::headers Log Regex duckx::duckx PRIVATE pugixml )
target_compile_definitions(word_formatter PRIVATE DEBUG_LOG)

target_compile_definitions(word_formatter PRIVATE BUILD_LIB)

message("Compiling tests...")
file(GLOB TEST_SOURCES "test/*.cpp")

message("Compiling: ${TEST_SOURCES}")
add_executable(word_formatter_tests ${TEST_SOURCES} ${HEADERS})

target_link_libraries(word_formatter_tests PRIVATE word_formatter)
target_include_directories(word_formatter_tests PRIVATE "include")
