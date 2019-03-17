cmake_minimum_required(VERSION 3.6)

set(TARGET_NAME server)
set(CMAKE_CXX_STANDARD 11)

project(${TARGET_NAME})

#Configure common
set(COMMON_ROOT ${CMAKE_SOURCE_DIR}/lib)
include_directories(${COMMON_ROOT})

#Configure rapidjson
set(RAPID_ROOT ${COMMON_ROOT})
include_directories(${RAPID_ROOT}/rapidjson)

#Configure ENet
set(ENET_ROOT ${COMMON_ROOT}/enet-1.3.14)
include_directories(${ENET_ROOT}/include)
add_subdirectory(${ENET_ROOT})

#Configure delegates
set(FAST_DELEGATE_ROOT ${COMMON_ROOT}/FastDelegate)
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

#Createa IDE filters

file(GLOB SERVER_FILTER src/*.hpp src/*.cpp)
source_group("" FILES ${SERVER_FILTER})

file(GLOB_RECURSE CONFIG_FILTER src/config/*.hpp src/config/*.cpp)
source_group("config" FILES ${CONFIG_FILTER})

file(GLOB_RECURSE SIMULATOR_FILTER src/simulator/*.hpp src/simulator/*.cpp)
source_group("simulator" FILES ${SIMULATOR_FILTER})
