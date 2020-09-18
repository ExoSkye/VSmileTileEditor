# CMakeList.txt : CMake project for VSmileTileDitor, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(SDL2_DIR "../libs/SDL2")

set(CMAKE_CXX_STANDARD 20)

# Add source to this project's executable.
add_executable (VTileEdit "main.cpp" "main.h" "ImGUI/imgui.cpp" "ImGUI/imgui_draw.cpp" "ImGUI/imgui_widgets.cpp" "ImGUI/imgui_sdl.cpp" "ImGUI/imgui_demo.cpp" "ImGUI/imgui_impl_sdl.cpp")
target_include_directories(VTileEdit PUBLIC ${SDL2_DIR}/include)
target_link_libraries(VTileEdit SDL2-static SDL2main)
# TODO: Add tests and install targets if needed.
