cmake_minimum_required(VERSION 3.16)
project(stb)

add_library(stb)
target_sources(stb PRIVATE stb_image.c stb_include.c)
target_include_directories(stb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})