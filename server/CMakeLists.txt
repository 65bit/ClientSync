cmake_minimum_required(VERSION 3.13)

set(TARGET_NAME server)

project(${TARGET_NAME})

#Configure ENet
set(ENET_ROOT ${CMAKE_SOURCE_DIR}/lib/enet-1.3.14)
include_directories(${ENET_ROOT}/include)
add_subdirectory(${ENET_ROOT})

#Configure delegates
set(FAST_DELEGATE_ROOT ${CMAKE_SOURCE_DIR}/lib/FastDelegate)
include_directories(${FAST_DELEGATE_ROOT})

#Build
include_directories(${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE PROJECT_INCLUDE src/*.hpp)
file(GLOB_RECURSE PROJECT_SOURCE src/*.cpp)

add_executable(${TARGET_NAME} ${PROJECT_INCLUDE} ${PROJECT_SOURCE})

#Link externals
target_link_libraries(${TARGET_NAME} enet)

if(MSVC)
    target_link_libraries(${TARGET_NAME} wsock32 ws2_32 Winmm)
endif(MSVC)