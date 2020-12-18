cmake_minimum_required(VERSION 2.8)
project(kid_size)


find_package(catkin REQUIRED COMPONENTS
  RhAL
  model
  robocup_referee
  rhoban_graphs
  rhoban_random
  rhoban_unsorted
  tiny-dnn
  aruco
  tclap
  rhoban_team_play
  rhoban_model_learning
  csa_mdp_experiments
  )

catkin_package(
  INCLUDE_DIRS Motion/
  #LIBRARIES #TODO: which libraries are published?
  CATKIN_DEPENDS tclap RhAL model robocup_referee rhoban_graphs rhoban_team_play rhoban_random rhoban_unsorted rhoban_model_learning
)

#Build option
option (BUILD_KID_SIZE_VISION "Build the Vision" ON)
option (BUILD_VISION_OPENCL "Build vision with OpenCL support" OFF)
option (BUILD_KID_SIZE_PROGRAM_VISION "Enable the build of Vision Programs" OFF)
option (BUILD_KID_SIZE_PROGRAM_STRATEGY "Enable the build of Strategy programs" ON)

#Define Vision enable Macro + specific includes
if (BUILD_KID_SIZE_VISION)
  enable_language(ASM)
  add_definitions(-DVISION_COMPONENT)
  add_definitions(-DENABLE_OPENCV)
  #set (OpenCV_DIR /home/gregwar/opencv/build/)
  #set(OpenCV_DIR /home/ys/opencv-2.4.9/)
  set(OpenCV_DIR /usr/local/opencv2.4/share/OpenCV)
  find_package(OpenCV 2.4 REQUIRED)
  #set(OpenCV_DIR /usr/local/opencv3.4/share/OpenCV)
  #find_package(OpenCV 3.4 REQUIRED)
  
  #include_directories(${OpenCV_DIR}/include)
  #include_directories(${OpenCV_DIR}/include/opencv2)
  #link_directories(${OpenCV_DIR}/lib ${OpenCV_DIR}/3rdparty/lib)

  # Use SSE2 and AVX if provided, otherwise, show a warning
  if ("${CMAKE_CXX_FLAGS}" MATCHES "-msse2" AND
      "${CMAKE_CXX_FLAGS}" MATCHES "-mavx")
    # Enable SSE2 optimizations
    add_definitions(-DCNN_USE_SSE)
    # Enable Advanced Vector Extensions
    add_definitions(-DCNN_USE_AVX)
  else()
    message(WARNING "missing flags for SSE2 and AVX! DNN will be really slow")
  endif()
endif ()

if (BUILD_VISION_OPENCL)
    add_definitions(-DENABLE_OPENCL)
endif ()



#Enable C++11
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Enable real warnings
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")

# Enable open mp support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")


#Enable Weird option linker
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -g")

#Included internal Directories
set (INCLUDED_DIRECTORIES
    Motion
    ${catkin_INCLUDE_DIRS}
)

if (BUILD_KID_SIZE_VISION)
  set (INCLUDED_DIRECTORIES ${INCLUDED_DIRECTORIES} Vision_obj)
endif()

include_directories(${INCLUDED_DIRECTORIES})


#Linked dependency external Libraries
set (LINKED_LIBRARIES
    pthread
    ${catkin_LIBRARIES}
)

#Add Vision libraries
if (BUILD_KID_SIZE_VISION)
    set (LINKED_LIBRARIES
      ${LINKED_LIBRARIES}
      ${OpenCV_LIBS}
#      v4l2 #v4l2 cameras are not used anymore
      flycapture
      )
endif ()


#List Sources sub source directories
set(SOURCES_DIRECTORIES
    Motion/moves
    Motion/scheduler
    Motion/services
    Motion/strategy
)

if (BUILD_KID_SIZE_VISION)
  set (SOURCES_DIRECTORIES
    ${SOURCES_DIRECTORIES}
    Vision_obj/Application
    Vision_obj/Binding
    Vision_obj/CameraState
    Vision_obj/FrameSource
    Vision_obj/Field
    Vision_obj/Filters
    Vision_obj/Filters/Source
    Vision_obj/Filters/Custom
    Vision_obj/Filters/ObjectDetect
    Vision_obj/Filters/Features
    Vision_obj/Features
    Vision_obj/FrameSource
    Vision_obj/Hough
    Vision_obj/Localisation
    Vision_obj/Localisation/Ball
    Vision_obj/Localisation/Robot
    Vision_obj/Localisation/Field
    Vision_obj/Tags
    Vision_obj/Utils
    #Vision_obj/detect_server
    )
endif(BUILD_KID_SIZE_VISION)


#Include Sources sub sources
foreach (DIRECTORY ${SOURCES_DIRECTORIES})
    message(STATUS "-- Sources: Adding ${DIRECTORY}/")
    include (${DIRECTORY}/Sources.cmake)
    set (PREFIXED_SOURCES)
    foreach (SOURCE ${SOURCES})
        set (PREFIXED_SOURCES ${PREFIXED_SOURCES} ${DIRECTORY}/${SOURCE})
    endforeach (SOURCE)
    set (ALL_SOURCES ${ALL_SOURCES} ${PREFIXED_SOURCES})
endforeach (DIRECTORY)

#Build the kid_size main library
add_library (kid_size SHARED ${ALL_SOURCES})
target_link_libraries (kid_size ${LINKED_LIBRARIES})
# build libcurl
find_package(curl REQUIRED)
include_directories(/usr/local/include)
target_link_libraries(kid_size -L/usr/local/lib -lcurl)
#ADD_COMPILE_OPTIONS(curl-config --cc --cflags --libs)

# pthread link
target_link_libraries(kid_size -lpthread -lm)

#==============  PATHS TO TENSORFLOW  =======================
# build tensorflow session 
set(tf_SOURCE_FILES Vision_obj/detect_server/detect.cpp Vision_obj/detect_server/main.cpp Vision_obj/detect_server/base64.cpp Vision_obj/detect_server/base64.h)
add_executable(detect_crow_server ${tf_SOURCE_FILES})

# OpenCV libs
#set(OpenCV_DIR /usr/local/opencv2.4/share/OpenCV)
#find_package(OpenCV 2.4 REQUIRED)
#set(OpenCV_DIR /usr/local/opencv3.4/share/OpenCV)
#find_package(OpenCV 3.4 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(detect_crow_server ${OpenCV_LIBS})

# boost lib
find_package(Boost REQUIRED COMPONENTS system thread)
include_directories( ${Boost_INCLUDE_DIRS} )
target_link_libraries(detect_crow_server ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY})

# pthread link
target_link_libraries(detect_crow_server -lpthread -lm)

# protobuf
find_package(Protobuf REQUIRED)

include_directories("/home/ys/tensorflow_test/include/eigen3")
#include_directories("/usr/include/absl")

# TensorFlow headers
include_directories("/home/ys/tensorflow_test/include")
include_directories("/home/ys/tensorflow_test/include/tensorflow")
include_directories("/home/ys/tensorflow_test/include/google")
include_directories("/home/ys/tensorflow_test/include/third-party")
#include_directories("/home/ys/")
# Link TensorFlow libs

link_directories("/home/ys/tensorflow_test/lib")
target_link_libraries(detect_crow_server tensorflow_cc)
target_link_libraries(detect_crow_server tensorflow_framework)
#target_link_libraries(kid_size "/usr/lib/libabsl.a")

#######################################################################=================================================================================

# Adding build infos in the main
exec_program(
    "git"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ARGS "rev-parse HEAD"
    OUTPUT_VARIABLE VERSION_NUM )
exec_program(
    "whoami"
    ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE VERSION_USER )
exec_program(
    "hostname"
    ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE VERSION_HOSTNAME )

set_source_files_properties (
    KidSize/buildinfos.c
    PROPERTIES COMPILE_FLAGS
     "-DVERSION_USER=\"${VERSION_USER}\" -DVERSION_NUM=\"${VERSION_NUM}\" -DVERSION_HOSTNAME=\"${VERSION_HOSTNAME}\"")

#Force main recompilation to have up to date build info
add_custom_target(date ALL COMMAND touch ${CMAKE_CURRENT_SOURCE_DIR}/KidSize/buildinfos.c)

#Build the main kid_size binary
add_executable (KidSize KidSize/main.cpp KidSize/buildinfos.c)
target_link_libraries (KidSize ${LINKED_LIBRARIES} kid_size)
add_dependencies(KidSize date)

if (BUILD_KID_SIZE_PROGRAM_STRATEGY)
    add_executable(KickStrategy Motion/strategy/main.cpp)
    target_link_libraries(KickStrategy ${LINKED_LIBRARIES} kid_size)
endif ()

#Build Vision Programs
if (BUILD_KID_SIZE_PROGRAM_VISION)
    # Simple player
    add_executable(Player Vision/Examples/Player.cpp)
    target_link_libraries(Player ${LINKED_LIBRARIES} kid_size)
endif ()

