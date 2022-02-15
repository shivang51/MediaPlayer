set(Lib_Name "libmedia")

message(STATUS "Building ${Lib_Name}")

include_directories(${Ven_Inc_Dirs})
include_directories(${Proj_Inc_Dirs})

link_directories(${Ven_Lib_Dirs})
link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

file(GLOB src_files
	"src/*.h"
	"src/*.cpp"
)

add_library(${Lib_Name}
    ${src_files}
)

target_link_libraries(${Lib_Name} debug ${ffmpeg_libs})