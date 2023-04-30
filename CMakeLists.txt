# CMakeList.txt: SatoriCompiler 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required(VERSION 3.8)

project("SatoriCompiler")

file(GLOB_RECURSE SRC_FILES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.h"
)

if (MSVC)
    # 设置 C 语言编译 flags,  输入代码编码格式为 utf-8
    set(CMAKE_C_FLAGS /source-charset:utf-8)
endif()

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)

# 将源代码添加到此项目的可执行文件。
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-w)

if(CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET SatoriCompiler PROPERTY CXX_STANDARD 20)
endif()

add_custom_command(
    TARGET SatoriCompiler
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/assets"
)