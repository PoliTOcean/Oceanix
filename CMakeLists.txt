cmake_minimum_required(VERSION 3.0)
project(Oceanix VERSION 0.1)

# Print compiler information
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Suppress warnings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif()

# Gather all source files in the src directory
file(GLOB_RECURSE PROJECT_SOURCE_FILES 
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
# Gather all library source files in the lib directory
file(GLOB_RECURSE LIBRARY_SOURCE_BAR 
    "${PROJECT_SOURCE_DIR}/libs/BAR/src/*.c"
)
file(GLOB_RECURSE LIBRARY_SOURCE_IMU 
    "${PROJECT_SOURCE_DIR}/libs/IMU/src/*.c"
)
file(GLOB_RECURSE LIBRARY_SOURCE_CTRL 
    "${PROJECT_SOURCE_DIR}/libs/Controller/src/*.cpp"
)
file(GLOB_RECURSE LIBRARY_SOURCE_CHIM 
    "${PROJECT_SOURCE_DIR}/libs/Chimpanzee/src/*.cpp"
)

find_library(PIGPIO_LIB pigpio)

# Create a static library from the library source files
add_library(BAR_lib STATIC ${LIBRARY_SOURCE_BAR})
add_library(IMU_lib STATIC ${LIBRARY_SOURCE_IMU})
add_library(CTRL_lib STATIC ${LIBRARY_SOURCE_CTRL})
add_library(CHIM_lib STATIC ${LIBRARY_SOURCE_CHIM})
add_library(JSON_lib INTERFACE)

# Create an executable from the project source files
add_executable(Oceanix ${PROJECT_SOURCE_FILES})

target_include_directories(Oceanix PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(BAR_lib PUBLIC ${PROJECT_SOURCE_DIR}/libs/BAR/include)
target_include_directories(IMU_lib PUBLIC ${PROJECT_SOURCE_DIR}/libs/IMU/include)
target_include_directories(CTRL_lib PUBLIC ${PROJECT_SOURCE_DIR}/libs/Controller/include)
target_include_directories(JSON_lib SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/libs/json)
target_include_directories(CHIM_lib PUBLIC ${PROJECT_SOURCE_DIR}/libs/Chimpanzee/include)

# Find libuv using pkg-config (assumes libuv is installed and pkg-config is set up)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUV REQUIRED libuv)

# Include the libuv include directory
target_include_directories(Oceanix PRIVATE ${LIBUV_INCLUDE_DIRS})

# Link the libuv library
target_link_libraries(Oceanix PRIVATE BAR_lib IMU_lib CTRL_lib CHIM_lib JSON_lib ${LIBUV_LIBRARIES} paho-mqtt3a PahoMqttCpp::paho-mqttpp3 ${PIGPIO_LIB})

# Additional libraries you may need
find_package(OpenSSL REQUIRED)
find_package(eclipse-paho-mqtt-c REQUIRED)
find_package(PahoMqttCpp REQUIRED)
