cmake_minimum_required(VERSION 3.10)
project(aceinetx.gzdoom)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

# Download and set up libmem
FetchContent_Declare(libmem-config URL "https://raw.githubusercontent.com/rdbo/libmem/config-v1/libmem-config.cmake" DOWNLOAD_NO_EXTRACT TRUE)
FetchContent_MakeAvailable(libmem-config)
set(CMAKE_PREFIX_PATH "${libmem-config_SOURCE_DIR}" "${CMAKE_PREFIX_PATH}")
set(LIBMEM_DOWNLOAD_VERSION "5.0.2")

# Find libmem package
find_package(libmem CONFIG REQUIRED)

include_directories("include")
set(SOURCES "dllmain.cpp" "include/imgui_hook.cpp" "include/kiero/kiero.cpp" "include/kiero/minhook/src/buffer.c" "include/kiero/minhook/src/hook.c" "include/kiero/minhook/src/trampoline.c" "include/kiero/minhook/src/hde/hde32.c" "include/kiero/minhook/src/hde/hde64.c" "style.cpp" "gzdoom.cpp" "interface.cpp")
set(IMGUI_SOURCES "include/imgui/imgui.cpp" "include/imgui/imgui_demo.cpp" "include/imgui/imgui_draw.cpp" "include/imgui/imgui_impl_opengl2.cpp" "include/imgui/imgui_impl_win32.cpp" "include/imgui/imgui_tables.cpp" "include/imgui/imgui_widgets.cpp")

include_directories("include" "include/imgui" "include/kiero" "include/kiero/minhook/src" "include/kiero/minhook/include" "include/kiero/minhook/src/hde")

link_directories("lib")

add_library("aceinetx.gzdoom" SHARED ${SOURCES} ${IMGUI_SOURCES})
target_link_libraries("aceinetx.gzdoom" PRIVATE opengl32 user32 psapi ntdll shell32 libmem::libmem)