cmake_minimum_required(VERSION 3.10)

set(This client)

set(CMAKE_CXX_FLAGS "-g -Wall -Werror -std=c++14")

project(${This})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

add_subdirectory(proto)

add_custom_target(proto_gen 
    COMMAND protoc -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/proto/message.proto
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/proto/message.proto)


find_package(Boost REQUIRED COMPONENTS date_time chrono system thread)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/proto)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(${This} Client.cpp)

target_link_libraries(${This} proto ${PROTOBUF_LIBRARY} ${Boost_LIBRARIES})