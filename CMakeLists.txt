cmake_minimum_required(VERSION 3.16)

project(zexjson LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(SOURCE_CODE_DIR ${PROJECT_SOURCE_DIR}/src)

list(APPEND PROJECT_SOURCES main.cpp)

file(GLOB_RECURSE files
    ${SOURCE_INCLUDE_DIR}/*.hpp
    ${SOURCE_CODE_DIR}/*.cpp
)

foreach(file ${files})
    list(APPEND PROJECT_SOURCES ${file})
endforeach(file ${files})

add_executable(
    ${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${SOURCE_INCLUDE_DIR}
)

target_link_directories(${PROJECT_NAME}
    PUBLIC
        ${SOURCE_CODE_DIR}
)
