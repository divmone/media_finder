cmake_minimum_required(VERSION 3.28)

project(media_finder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)

FetchContent_Declare(
        httplib
        URL https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.18.1.tar.gz
)

FetchContent_MakeAvailable(json httplib)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/Scanner.cpp
        src/Scanner.hpp
        src/HttpServer.cpp
        src/HttpServer.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json httplib::httplib)
