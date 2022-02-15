set(App_Name "amp")

message(STATUS "Building ${App_Name}")

include_directories(${Ven_Inc_Dirs})
include_directories(${Proj_Inc_Dirs})

link_directories(${Ven_Lib_Dirs})
link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

file(GLOB src_files
	"src/*.h"
	"src/*.cpp"

)

add_executable(${App_Name}
	${src_files}
	${ImGui_Src_Files}
)

target_link_libraries(${App_Name} gui.lib player.lib libmedia.lib audio.lib video.lib portaudio.lib glfw3.lib ${ffmpeg_libs})