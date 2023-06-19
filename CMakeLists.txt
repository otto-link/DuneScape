cmake_minimum_required(VERSION 3.10)
project(dunescape)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Find required packages
find_package(glfw3 REQUIRED)

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)


# --- Dear ImGui
set(IMGUI_DIR external/imgui)

set(IMGUI_SRC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

set(IMGUI_INCLUDE
	external/imgui
	external/imgui/backends)

add_executable(${PROJECT_NAME}
    src/main.cpp
    ${IMGUI_SRC}
)

target_include_directories(${PROJECT_NAME}
                           PUBLIC
                             ${PROJECT_SOURCE_DIR}/include
			     ${IMGUI_INCLUDE}
			    )
			     
# Link libraries
target_link_libraries(${PROJECT_NAME}
    glfw
    OpenGL::GL
)

# Set C++ version
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
