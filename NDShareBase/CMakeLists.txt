cmake_minimum_required(VERSION 2.8)
PROJECT(NDShareBase)


set(HeadFileDirList "")  
#递归包含该项目下指定文件夹下的所有.h文件的文件路径;  
file(GLOB SRCH_LIST_1 "common/*.h")
file(GLOB SRCH_LIST_2 "commonImpl/*.h")
file(GLOB SRCH_LIST_3 "linux/*.h")
set(SRCH_LIST ${SRCH_LIST_1} ${SRCH_LIST_2} ${SRCH_LIST_3}) 
foreach(HeadFile ${SRCH_LIST})  
    #去掉文件名,截取路径;
    string(REGEX REPLACE "(.*)/.*\\.h$" "\\1" HeadFileDir ${HeadFile})
    #包含路径;
    include_directories(${HeadFileDir})
    #拼合路径,然后设置CMAKE_INCLUDE_PATH可以使用find_path搜索到该目录;
    set(HeadFileDirList "${HeadFileDirList}${HeadFileDir} ")
endforeach()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/commonImpl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/mysql)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/zlib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/lua)

#包含c文件;
set(zlib_c_path ${CMAKE_CURRENT_SOURCE_DIR}/commonImpl/zlib)
SET(SRC_C ${zlib_c_path}/adler32.c
					${zlib_c_path}/compress.c
					${zlib_c_path}/crc32.c
					${zlib_c_path}/deflate.c
					${zlib_c_path}/gzclose.c
					${zlib_c_path}/gzlib.c
					${zlib_c_path}/gzread.c
					${zlib_c_path}/gzwrite.c
					${zlib_c_path}/inflate.c
					${zlib_c_path}/infback.c
					${zlib_c_path}/inftrees.c
					${zlib_c_path}/inffast.c
					${zlib_c_path}/trees.c
					${zlib_c_path}/uncompr.c
					${zlib_c_path}/zutil.c)
					
set(lua_c_path ${CMAKE_CURRENT_SOURCE_DIR}/commonImpl/lua)
SET(SRC_LUA ${lua_c_path}/lapi.c
					${lua_c_path}/lcode.c
					${lua_c_path}/lctype.c
					${lua_c_path}/ldebug.c
					${lua_c_path}/ldo.c
					${lua_c_path}/ldump.c
					${lua_c_path}/lfunc.c
					${lua_c_path}/lgc.c
					${lua_c_path}/llex.c
					${lua_c_path}/lmem.c
					${lua_c_path}/lobject.c
					${lua_c_path}/lopcodes.c
					${lua_c_path}/lparser.c
					${lua_c_path}/lstate.c
					${lua_c_path}/lstring.c
					${lua_c_path}/ltable.c
					${lua_c_path}/ltm.c
					${lua_c_path}/lundump.c
					${lua_c_path}/lvm.c
					${lua_c_path}/lzio.c
					${lua_c_path}/lauxlib.c
					${lua_c_path}/lbaselib.c
					${lua_c_path}/lbitlib.c
					${lua_c_path}/lcorolib.c
					${lua_c_path}/ldblib.c
					${lua_c_path}/liolib.c
					${lua_c_path}/lmathlib.c
					${lua_c_path}/loslib.c
					${lua_c_path}/lstrlib.c
					${lua_c_path}/ltablib.c
					${lua_c_path}/lutf8lib.c
					${lua_c_path}/loadlib.c
					${lua_c_path}/linit.c)
										
#包含cpp文件;
SET(SRC_CPP ${CMAKE_CURRENT_SOURCE_DIR}/linux/NDShareBaseLinuxAll.cpp)


MESSAGE(STATUS "this is BINARY dir: " ${NDShareBase_BINARY_DIR})
MESSAGE(STATUS "this is SOURCE dir: " ${NDShareBase_SOURCE_DIR})
MESSAGE(STATUS "this is CURRENT dir: " ${CMAKE_CURRENT_SOURCE_DIR})


IF(UNIX)
ADD_DEFINITIONS(-DLINUX -DND_USE_EPOLL -DLUA_USE_LINUX -g -O0)
SET(CMAKE_CXX_COMPILER g++)
SET(CMAKE_CXX_FLAGS -Wall)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../Bin/linux/x64/UnityDebugBin/lib)

LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/dep/lib)

ADD_LIBRARY(NDShareBase STATIC ${SRC_C} ${SRC_LUA} ${SRC_CPP})

TARGET_LINK_LIBRARIES(NDShareBase GMemPool mysqlclient)

ENDIF(UNIX)


