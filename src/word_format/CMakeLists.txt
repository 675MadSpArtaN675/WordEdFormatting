cmake_minimum_required(VERSION 4.0)
project(DocxFormatter CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBRARIES_DIRS ${DUCKX_DIRECTORY} ${PUGIXML_DIRECTORY})

list(TRANSFORM LIBRARIES_DIRS APPEND "/include" OUTPUT_VARIABLE LIBRARIES_INCLUDES)
list(APPEND LIBRARIES_INCLUDES "${DUCKX_DIRECTORY}/include/duckx")
list(APPEND LIBRARIES_INCLUDES "${DUCKX_DIRECTORY}/include")
list(APPEND LIBRARIES_INCLUDES "C:\\Lib\\_include")

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${LIBRARIES_DIRS} "${DUCKX_DIRECTORY}/cmake/duckx" "/usr/local" )

find_package(Boost REQUIRED headers log regex)
find_package(duckx REQUIRED HINTS ${DUCKX_DIRECTORY})
find_package(pugixml CONFIG REQUIRED)

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost_VERSION: ${Boost_VERSION}")

if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0602)
endif()

file(GLOB_RECURSE HEADERS "include/*.hpp")
file(GLOB SOURCES "*.cpp")

message("Header files: ${HEADERS};\nSource files: ${SOURCES}")
add_library(word_formatter SHARED ${SOURCES} ${HEADERS})

target_include_directories(word_formatter PUBLIC "include" ${LIBRARIES_INCLUDES})
target_link_libraries(word_formatter PUBLIC Boost::headers Boost::log Boost::regex duckx::duckx PRIVATE pugixml)

if(DEBUG_LIB)
    message("Debugging on!")
    target_compile_definitions(word_formatter PRIVATE DEBUG_LOG)
endif()

target_compile_definitions(word_formatter PRIVATE BUILD_LIB)

message("Compiling tests...")
file(GLOB TEST_SOURCES "test/*.cpp")

message("Compiling: ${TEST_SOURCES}")
add_executable(word_formatter_tests ${TEST_SOURCES} ${HEADERS})

target_link_libraries(word_formatter_tests PRIVATE word_formatter)
target_include_directories(word_formatter_tests PRIVATE "include")
