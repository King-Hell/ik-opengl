cmake_minimum_required(VERSION 3.13)
project(ik-opengl)
set(CMAKE_CXX_STANDARD 17)
include_directories(../include)
aux_source_directory(ik-opengl SRCS)
add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} glfw3 GL m Xrandr Xi X11 Xxf86vm pthread dl Xinerama Xcursor assimp Leap)
#[[#将着色器文件拷贝到生成文件
file(GLOB SHADER_FILES RELATIVE ${PROJECT_SOURCE_DIR} shaders/*)
foreach (i ${SHADER_FILES})
    message(${i})
    configure_file(${i} ${i})
endforeach ()]]
add_definitions(-Wall)
