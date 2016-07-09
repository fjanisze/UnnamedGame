project(Game)
cmake_minimum_required(VERSION 2.8)

#########################################################
# C++1y
#########################################################

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#########################################################
# PTHREAD
#########################################################

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET my_app PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET my_app PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()

#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)


#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

if(NOT OPENGL_GLU_FOUND)
    message(ERROR " GLU not found!")
endif(NOT OPENGL_GLU_FOUND)


#########################################################
# Remaining shit
#########################################################

file(GLOB SRC_LIST
    "*.hpp"
    "*.cpp"
    "./chrono/*.hpp"
    "./chrono/*.cpp"
    "./events/*.cpp"
    "./events/*.hpp"
    "./logger/*.*"
    "./configuration/*.*"
    "./runner/*.*"
    "./graphics/*.*"
    "./maps/*.*"
    "./random/*.*"
)

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(
    ${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${GLUT_LIBRARIES}
    ${OPENGL_glu_LIBRARY}
    ${OPENGL_gl_LIBRARY} )



