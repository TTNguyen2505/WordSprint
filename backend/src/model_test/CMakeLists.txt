cmake_minimum_required(VERSION 3.22)
project(ModelTest LANGUAGES CXX)

add_executable(ModelTest main.cpp)
# Include the model headers
target_include_directories(ModelTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../model)
