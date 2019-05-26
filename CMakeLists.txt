cmake_minimum_required(VERSION 3.13)
project(ik-opengl)
set(CMAKE_CXX_STANDARD 17)
include_directories(../include)
aux_source_directory(src SRCS)
add_executable(${PROJECT_NAME} ${SRCS})
FILE(COPY res DESTINATION .)
target_link_libraries(${PROJECT_NAME} glfw3 GL m Xrandr Xi X11 Xxf86vm pthread dl Xinerama Xcursor assimp)
#将着色器文件拷贝到生成文件

add_definitions(-Wall)
