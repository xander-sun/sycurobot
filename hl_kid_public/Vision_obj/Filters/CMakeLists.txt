cmake_minimum_required(VERSION 3.7)
project(tf_detect)

#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -g")
#add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
if(CONAN_LIBCXX STREQUAL "libstdc++11")
    add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
elseif(CONAN_LIBCXX STREQUAL "libstdc++")
    add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
endif()
#set(CMAKE_VERBOSE_MAKEFILE on)

set(SOURCE_FILES out.cpp)
add_executable(tf_detect ${SOURCE_FILES})

# protobuf
#find_package(Protobuf REQUIRED)

# OpenCV libs
#set(OpenCV_DIR /home/ys/opencv-2.4.9/build)
#set(OpenCV_DIR /usr/local/opencv3.4/share/OpenCV)
#find_package(OpenCV 3.4 REQUIRED)
#include_directories(${OpenCV_INCLUDE_DIRS})
#target_link_libraries(tf_detector_example ${OpenCV_LIBS})


# ==================== PATHS TO SPECIFY! ==================== #

# Eigen lib headers
include_directories("/home/ys/tensorflow_test/include/eigen3")
include_directories("/usr/include/absl")

# TensorFlow headers
include_directories("/home/ys/tensorflow_test/include")
include_directories("/home/ys/tensorflow_test/include/tensorflow")
include_directories("/home/ys/tensorflow_test/include/google")
include_directories("/home/ys/tensorflow_test/include/third-party")
include_directories("/home/ys/")
# Link TensorFlow libs

link_directories("/home/ys/tensorflow_test/lib")
target_link_libraries(tf_detect tensorflow_cc)
target_link_libraries(tf_detect tensorflow_framework)
target_link_libraries(tf_detect "/usr/lib/libabsl.a")

