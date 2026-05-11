cmake_minimum_required(VERSION 3.10)
project(caching-proxy)

set(CMAKE_CXX_STANDARD 17)

add_executable(caching-proxy src/main.cpp)
target_link_libraries(caching-proxy pthread)
