cmake_minimum_required(VERSION 3.8)

project("SatoriCompiler")

file(GLOB_RECURSE SRC_FILES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/test/*.cpp"
    "${PROJECT_SOURCE_DIR}/test/*.h"
    "${PROJECT_SOURCE_DIR}/assets/sem/*.sem"
)

if(MSVC)
    # 设置 C 语言编译 flags,  输入代码编码格式为 utf-8
    set(CMAKE_C_FLAGS /source-charset:utf-8)

    # 设置 C++ 语言编译 flags,  输入代码编码格式为 utf-8
    set(CMAKE_CXX_FLAGS /source-charset:utf-8)

    # 4003:宏定义参数不足 4005:宏重定义 4819:文件名中有非ASCII字符
    add_compile_options(/wd4003 /wd4005 /wd4819)
    add_compile_options(/EHsc) # 异常处理，启用展开语义
endif()

find_package(Threads)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/test)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/assets/sem)

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

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