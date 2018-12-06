cmake_minimum_required(VERSION 2.8)
PROJECT(NDGatewayServer)


set(HeadFileDirList "")  
#递归包含该项目下指定文件夹下的所有.h文件的文件路径;  
file(GLOB SRCH_LIST_1 "include/*.h")
set(SRCH_LIST ${SRCH_LIST_1}) 
foreach(HeadFile ${SRCH_LIST})  
    #去掉文件名,截取路径;
    string(REGEX REPLACE "(.*)/.*\\.h$" "\\1" HeadFileDir ${HeadFile})
    #包含路径;
    include_directories(${HeadFileDir})
    #拼合路径,然后设置CMAKE_INCLUDE_PATH可以使用find_path搜索到该目录;
    set(HeadFileDirList "${HeadFileDirList}${HeadFileDir} ")
endforeach()

#包含外部头文件;
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../NDShareBase/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../NDProtocol/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../NDServerShare/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


#包含cpp文件;
SET(SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/NDGatewayServer_all.cpp)


MESSAGE(STATUS "this is BINARY dir: " ${NDGatewayServer_BINARY_DIR})
MESSAGE(STATUS "this is SOURCE dir: " ${NDGatewayServer_SOURCE_DIR})
MESSAGE(STATUS "this is CURRENT dir: " ${CMAKE_CURRENT_SOURCE_DIR})


IF(UNIX)
ADD_DEFINITIONS(-DLINUX -g -O0)
SET(CMAKE_CXX_COMPILER g++)
SET(CMAKE_CXX_FLAGS -Wall)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../Bin/linux/x64/UnityDebugBin)

LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../Bin/linux/x64/UnityDebugBin/lib)

ADD_EXECUTABLE(NDGatewayServer ${SRCS})

TARGET_LINK_LIBRARIES(NDGatewayServer pthread NDServerShare NDShareBase GMemPool mysqlclient dl)

ENDIF(UNIX)


