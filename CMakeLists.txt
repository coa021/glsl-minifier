cmake_minimum_required(VERSION 3.28)
project(glsl_minifier)

set(CMAKE_CXX_STANDARD 17)

add_executable(glsl_minifier src/main.cpp
        src/Token.cpp
        include/Token.hpp
        src/Scanner.cpp
        include/Scanner.hpp
        src/Minifier.cpp
        include/Minifier.hpp
        src/MinificationStats.cpp
        include/MinificationStats.hpp
        src/ErrorReporter.cpp
        include/ErrorReporter.hpp
        src/SymbolTable.cpp
        include/SymbolTable.hpp
        src/ShaderVerifier.cpp
        include/ShaderVerifier.hpp
        src/Application.cpp
        include/Application.hpp)

target_include_directories(glsl_minifier PRIVATE include)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SFML_FETCH_DEPENDENCIES TRUE)
include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
)

set(SFML_BUILD_TESTS OFF)
FetchContent_MakeAvailable(SFML)


target_link_libraries(glsl_minifier PRIVATE
        SFML::Graphics
)