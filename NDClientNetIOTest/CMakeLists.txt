cmake_minimum_required(VERSION 2.8)
PROJECT(NDClientNetIOTest)

#包含外部头文件;
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../NDShareBase/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../NDProtocol/include)

AUX_SOURCE_DIRECTORY(src SRCS)

MESSAGE(STATUS "this is BINARY dir: " ${NDClientNetIOTest_BINARY_DIR})
MESSAGE(STATUS "this is SOURCE dir: " ${NDClientNetIOTest_SOURCE_DIR})
MESSAGE(STATUS "this is CURRENT dir: " ${CMAKE_CURRENT_SOURCE_DIR})


IF(UNIX)
ADD_DEFINITIONS(-DLINUX -g -O0)
SET(CMAKE_CXX_COMPILER g++)
SET(CMAKE_CXX_FLAGS -Wall)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../DebugBin/testnetio)

LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../DebugBin/lib)

ADD_EXECUTABLE(NDClientNetIOTest ${SRCS})

TARGET_LINK_LIBRARIES(NDClientNetIOTest pthread NDShareBase GMemPool mysqlclient dl)

ENDIF(UNIX)


