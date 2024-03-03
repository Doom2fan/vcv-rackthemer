cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(vcv-rackthemer VERSION 1.0 LANGUAGES C CXX)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

set(LIB_TARGET_NAME ${PROJECT_NAME})

add_library(${LIB_TARGET_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${LIB_TARGET_NAME} PUBLIC include)
target_link_libraries(${LIB_TARGET_NAME} PRIVATE RackSDK)