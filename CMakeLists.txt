cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)
project(TV_ambient_lighting_rasppi)
include(CPack)

# -- PROJECT FILES -- 
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

# -- OPENCV --
set(OPENCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Librarys/opencv-4.x")
set(OPENCV_BUILD_DIR "${OPENCV_DIR}/build")

#if(NOT IS_DIRECTORY OPENCV_DIR) 
#    message(FATAL_ERROR "Can't find OpenCV directory Have you ran the setup script yet? Looking for it at: ${OPENCV_DIR}")
#endif()
#if(NOT EXISTS OPENCV_BUILD_DIR) 
#    message(FATAL_ERROR "Can't find OpenCV build directory Have you build the library yet? Looking for it at: ${OPENCV_BUILD_DIR}")
#endif()

find_package(OpenCV REQUIRED) # Manualy set path to look for OpenCV, otherwise it cant find it...  PATHS OPENCV_DIR NO_DEFAULT_PATH
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(TV_ambient_lighting_rasppi ${OpenCV_LIBS})

# -- RPI_WS281X --
set(RPI_WS281X_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Librarys/rpi_ws281x")
set(RPI_WS281X_LIB "${RPI_WS281X_DIR}/build/libws2811.a")

#if(NOT IS_DIRECTORY RPI_WS281X_DIR) 
#    message(FATAL_ERROR "Can't find rpi_ws281x folder. Have you ran the setup script yet? Looking for it at: ${RPI_WS281X_DIR}")
#endif()
target_include_directories(TV_ambient_lighting_rasppi PRIVATE "${RPI_WS281X_DIR}")

if(UNIX)
    if(NOT EXISTS RPI_WS281X_LIB) 
        message(FATAL_ERROR "Can't find libws2811.a! Have you build the library yet? Looking for it at: ${RPI_WS281X_LIB}")
    endif()

    target_link_libraries(TV_ambient_lighting_rasppi ${RPI_WS281X_LIB})
else()
    message("Non-unix platform detected: Not linking rpi_ws281x library! (Headers are included)")
endif()

