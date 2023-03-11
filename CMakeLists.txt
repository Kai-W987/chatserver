#最小版本
cmake_minimum_required(VERSION 2.8)
#工程名
project(chatserver)

#设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

#头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/database)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/third)

#连接库路径
link_directories(/usr/local/mysql/lib/)

#加载子目录
add_subdirectory(src)