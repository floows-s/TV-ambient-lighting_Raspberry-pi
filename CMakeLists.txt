cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)
project(TV_ambient_lighting_rasppi)

# Project files
set(SOURCE_DIR ./Source)
list(
    APPEND SOURCE_FILES
    ${SOURCE_DIR}/main.cpp 
    ${SOURCE_DIR}/const_config.h   
    ${SOURCE_DIR}/Dimensions.h 
    ${SOURCE_DIR}/LEDCounts.h 
    ${SOURCE_DIR}/Zone.h 
    ${SOURCE_DIR}/Zone.cpp 
    ${SOURCE_DIR}/ZoneManager.h 
    ${SOURCE_DIR}/ZoneManager.cpp
)

add_executable(TV_ambient_lighting_rasppi ${SOURCE_FILES})

# OpenCV
# if(WIN32)
#     set(OpenCVIncludeDirs, "C:/opencv/build/include")
#     set(OpenCVLibs, "C:/opencv/build/x64/vc16/lib")
# endif(WIN32)

# if(UNIX)
#     set(OpenCVIncludeDirs, "./Librarys/opencv/build/include")
#     set(OpenCVLibs, "./Librarys/opencv/x64/vc16/lib")
# endif(UNIX)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(TV_ambient_lighting_rasppi ${OpenCV_LIBS})

message(${OpenCV_LIBS})
#Getting error when make: prob using the .lib which is MSCV and not a .dll, Maybe just make windows cmake do visual studio project...

#TODO: rpi_2847wx

