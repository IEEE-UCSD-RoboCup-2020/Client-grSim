cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++14")

project(client)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

add_subdirectory(proto)

add_custom_target(proto_gen 
    COMMAND protoc -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto)


find_package(Boost REQUIRED COMPONENTS date_time chrono system thread)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/proto)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(client Client.cpp)
add_executable(client_udp Client_Udp.cpp)

target_link_libraries(client proto ${PROTOBUF_LIBRARY} ${Boost_LIBRARIES})
target_link_libraries(client_udp proto ${PROTOBUF_LIBRARY} ${Boost_LIBRARIES})