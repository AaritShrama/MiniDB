
cmake_minimum_required(VERSION 3.20)

project(MiniDBMS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(MiniDBMS
    src/main.cpp
    src/record.cpp
    src/schema.cpp
    src/table.cpp
    src/database.cpp
    src/storage.cpp
)

target_include_directories(MiniDBMS PRIVATE include)
