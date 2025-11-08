cmake_minimum_required(VERSION 3.16)
project(MTStockSystem LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)
include_directories(include)
add_library(mtstock
    src/Order.cpp
    src/OrderFactory.cpp
    src/ThreadSafeQueue.cpp
    src/ThreadPool.cpp
    src/MatchingEngine.cpp
)
add_executable(mt_stock src/main.cpp)
target_link_libraries(mt_stock PRIVATE mtstock)
