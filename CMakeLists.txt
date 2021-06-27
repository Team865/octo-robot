cmake_minimum_required(VERSION 3.0)
project (firware_sim CXX C)

OPTION (BEEFOCUS_CLANG, "Build using Clang" OFF)

# Make sure we're building with C++ 17.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if ( BEEFOCUS_CLANG )
    MESSAGE (STATUS  "Building using CLANG")
    SET (CMAKE_C_COMPILER             "/usr/bin/clang")
    SET (CMAKE_C_FLAGS                "-Wall -std=c99")
    SET (CMAKE_C_FLAGS_DEBUG          "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
    SET (CMAKE_CXX_FLAGS                "-Wall")
    SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_AR      "/usr/bin/llvm-ar")
    SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
    SET (CMAKE_NM      "/usr/bin/llvm-nm")
    SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
    SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")
ENDIF ( BEEFOCUS_CLANG )

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

set (FIRMWARE_V1_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v1/hardware_interface.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v1/command_datasend.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v1/command_encoder.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v1/command_motor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v1/command_parser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v1/command_process_input.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v1/command_scheduler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v1/command_sr04.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v1/time_manager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v1/util_profile.cpp
)

set (FIRMWARE_V2_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v2/hardware_interface.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v2/command_datasend.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v2/command_encoder.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v2/command_motor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v2/command_parser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v2/command_process_input.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v2/command_scheduler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v2/command_sr04.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v2/time_manager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v2/util_profile.cpp
)

add_library( firmware_v1_lib STATIC ${FIRMWARE_V1_SOURCES} )
add_library( firmware_v2_lib STATIC ${FIRMWARE_V2_SOURCES} )

LIST(APPEND FIRMWARE_V1_SIM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v1_sim/main.cpp)
LIST(APPEND FIRMWARE_V2_SIM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/firmware/firmware_v2_sim/main.cpp)

# Testing
find_package (GTest)
find_package (GMock)

IF (GTEST_FOUND)
  add_definitions( -DGTEST_FOUND )
  MESSAGE (STATUS  "GTEST found, running unit tests")
  ADD_SUBDIRECTORY(firmware/unit_tests)
ELSE()
  MESSAGE (STATUS  "GTEST not found, skipping unit tests")
ENDIF (GTEST_FOUND)
  
add_definitions( -DPC_BUILD )

add_executable(firmware_v1_sim ${FIRMWARE_V1_SIM_SOURCES})
target_link_libraries(firmware_v1_sim firmware_v1_lib )

add_executable(firmware_v2_sim ${FIRMWARE_V2_SIM_SOURCES})
target_link_libraries(firmware_v2_sim firmware_v2_lib )


