#set(CMAKE_VERBOSE_MAKEFILE ON)
#for debug
#cmake -DCMAKE_BUILD_TYPE=Debug ..

cmake_minimum_required(VERSION 3.0)
project(oakbrew-hw-control)
add_definitions(-std=c++11)

include_directories(inc)

#include_directories(/home/alarm/dev/i2c-tools/include)
#link_directories(/home/alarm/dev/i2c-tools/lib)

add_executable(oakbrew-hw-control
    src/main.cpp
    src/module.cpp
    src/util.cpp
    src/mpc_control.cpp
    src/hw_interface/SPI_Device.cpp
    src/hw_interface/RPi_GPIO.cpp
    src/hw_interface/Button.cpp
    src/hw_interface/PushButton.cpp
    src/hw_interface/SwitchButton.cpp
    src/hw_interface/LedControl.cpp
    src/hw_interface/MCP3008.cpp
    src/hw_interface/I2C_Device.cpp
    src/hw_interface/TDA7468.cpp)

# Your-external "mylib"
##add_library( i2c SHARED IMPORTED )
# You can define two import-locations: one for debug and one for release.
##set_target_properties( i2c PROPERTIES IMPORTED_LOCATION (/home/alarm/dev/i2c-tools/lib/ ))

# if no link to pthread, runtime error when starting thread
find_package(Threads REQUIRED)
TARGET_LINK_LIBRARIES(oakbrew-hw-control ${CMAKE_THREAD_LIBS_INIT} config++)

