cmake_minimum_required(VERSION 3.5.2)
project(${CMAKE_PROJECT_NAME}_example CXX)

file(GLOB SOURCES *.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_PROJECT_NAME}_lib pthread)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
