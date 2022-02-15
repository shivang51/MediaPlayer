set(Lib_Name "data_types")

message(STATUS "Building ${Lib_Name}")

file(GLOB src_files	
	"src/*.h"
	"src/*.cpp"
)

# include_directories(${Ven_Inc_Dirs})
add_library(${Lib_Name} STATIC
    ${src_files}
)

set_target_properties(${Lib_Name} PROPERTIES LINKER_LANGUAGE CXX)