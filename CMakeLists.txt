# 设置 CMake 最小版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(PhpyStand)

option(PHPYSTAND_CONSOLE "Build ${PROJECT_NAME} as a console application." OFF)

# sources
set(sources
	main.cpp
	resource.rc
)

if (PHPYSTAND_CONSOLE)
	add_executable(${PROJECT_NAME} ${sources})
	target_compile_definitions(${PROJECT_NAME} PRIVATE PHPYSTAND_CONSOLE)
else()
	add_executable(${PROJECT_NAME} ${sources})
endif()

# 设置包含目录
include_directories(
    "${CMAKE_SOURCE_DIR}/php/devel/include"
    "${CMAKE_SOURCE_DIR}/php/devel/include/main"
    "${CMAKE_SOURCE_DIR}/php/devel/include/sapi"
    "${CMAKE_SOURCE_DIR}/php/devel/include/Zend"
    "${CMAKE_SOURCE_DIR}/php/devel/include/TSRM"
)

# 查找所需库
find_library(PHP_LIB php8 REQUIRED PATHS "${CMAKE_SOURCE_DIR}/php/devel/lib")
# 链接库
target_link_options(${PROJECT_NAME} PUBLIC /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
target_link_libraries(${PROJECT_NAME} ${PHP_LIB})
