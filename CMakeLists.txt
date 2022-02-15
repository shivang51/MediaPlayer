cmake_minimum_required(VERSION 3.10)

project(MediaPlayer)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc /std:c++17")

# Static Library output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/lib/static")

# Library output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/lib/dynamic")

# Binary or Dll output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/bin")

add_subdirectory("include")

add_dependencies(audio data_types)
add_dependencies(video data_types)
add_dependencies(libmedia audio video data_types)
add_dependencies(player libmedia)
add_dependencies(gui player)
add_dependencies(amp gui player)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
	PROPERTY VS_STARTUP_PROJECT amp)

set_property(TARGET amp PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
