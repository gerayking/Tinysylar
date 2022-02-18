cmake_minimum_required(VERSION 3.16)
project(sylar)
set(CMAKE_VERBOSE_MAKEFILE  ON)
set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
#set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

set(LIB_SRC sylar/log.cc
         sylar/util.cc)
add_library(sylar SHARED ${LIB_SRC}) #生成so文件
add_executable(test tests/test.cpp )
add_dependencies(test sylar)
target_link_libraries(test sylar)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

