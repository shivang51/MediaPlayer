set(Lib_Name "gui")

message(STATUS "Building ${Lib_Name}")

include_directories(${Ven_Inc_Dirs})
include_directories(${Proj_Inc_Dirs})

link_directories(${Ven_Lib_Dirs})
link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set(ImGui_Src_Files
	${Vendors_Dir}/imgui/imgui_draw.cpp
	${Vendors_Dir}/imgui/imgui_tables.cpp
	${Vendors_Dir}/imgui/imgui_widgets.cpp
	${Vendors_Dir}/imgui/imgui.cpp
	${Vendors_Dir}/imgui/backends/imgui_impl_opengl3.cpp
	${Vendors_Dir}/imgui/backends/imgui_impl_glfw.cpp
)

file(GLOB src_files
	"src/*.h"
	"src/*.cpp"
	${ImGui_Src_Files}
)

add_library(${Lib_Name}
    ${src_files}
)