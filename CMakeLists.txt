﻿# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
set(CMAKE_CXX_STANDARD 17)
cmake_minimum_required (VERSION 3.8)

project ("Vulkan")

# 包含子项目。
add_subdirectory ("Vulkan")
