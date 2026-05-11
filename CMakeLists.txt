cmake_minimum_required(VERSION 3.20)
project(COGENT VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==== library core ====
add_library(cogent_core STATIC
    src/Parser.cpp
    src/Parser_SystemVerilog.cpp
    src/Parser_VHDL.cpp
    src/Exporter.cpp
    src/Exporter_HTML.cpp
    src/Exporter_MD.cpp
    src/Exporter_ADOC.cpp
    src/CmdLine.cpp
)

target_include_directories(cogent_core PUBLIC include/)

# ==== CLI ====
add_executable(cogent-cli src/main.cpp)
target_link_libraries(cogent-cli PRIVATE cogent_core)

# ==== GUI ====
#

if(WIN32)
    target_compile_definitions(cogent_core PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()