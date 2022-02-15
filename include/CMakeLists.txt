# Location of vendors directory
set(Vendors_Dir ${CMAKE_SOURCE_DIR}/vendors)

# Setting project include dirs
set(Proj_Inc_Dirs
	${CMAKE_CURRENT_SOURCE_DIR}/Application
	${CMAKE_CURRENT_SOURCE_DIR}/Audio
	${CMAKE_CURRENT_SOURCE_DIR}/Datatypes
	${CMAKE_CURRENT_SOURCE_DIR}/Gui
	${CMAKE_CURRENT_SOURCE_DIR}/Media
	${CMAKE_CURRENT_SOURCE_DIR}/Player
	${CMAKE_CURRENT_SOURCE_DIR}/Video
)

# Setting vendors include paths
set(Ven_Inc_Dirs 
	${Vendors_Dir}/ffmpeg/include
	${Vendors_Dir}/glad/include 
	${Vendors_Dir}/glfw/include
	${Vendors_Dir}/glm/
	${Vendors_Dir}/imgui
	${Vendors_Dir}/portaudio/include
	${Vendors_Dir}/
)

# Setting vendors lib paths
set(Ven_Lib_Dirs 
	${Vendors_Dir}/ffmpeg/lib
	${Vendors_Dir}/glfw/x64/lib-vc2019
	${Vendors_Dir}/portaudio/lib/Static/x64
)

# ffmpeg lib dependencies
set(ffmpeg_libs
	avcodec.lib
	avdevice.lib
	avfilter.lib
	avformat.lib
	avutil.lib
	swresample.lib
	swscale.lib
)

add_subdirectory("Application")
add_subdirectory("Audio")
add_subdirectory("Datatypes")
add_subdirectory("Gui")
add_subdirectory("Media")
add_subdirectory("Player")
add_subdirectory("Video")