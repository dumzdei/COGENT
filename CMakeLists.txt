cmake_minimum_required(VERSION 3.20)
project(COGENT VERSION 1.0 LANGUAGES CXX)

if(MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

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
find_package(Qt6 COMPONENTS Widgets Core REQUIRED)

add_executable(cogent-gui
    src/gui/main_gui.cpp
    src/gui/MainWindow.cpp
    src/gui/CogentWorker.cpp
    include/gui/MainWindow.h
    include/gui/CogentWorker.h
)

set_target_properties(cogent-gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

target_link_libraries(cogent-gui PRIVATE 
    Qt6::Widgets 
    Qt6::Core
    cogent_core
)

if(WIN32)
    set_target_properties(cogent-gui PROPERTIES WIN32_EXECUTABLE ON)
endif()