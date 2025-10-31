cmake_minimum_required(VERSION 3.16)
project(OpenCVLab)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
	CACHE STRING "Vcpkg toolchain file")
set(OpenCV_DIR "C:/vcpkg/installed/x64-windows/share/opencv")
# Find packages
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)


# Include directories
include_directories(
    ${GLM_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    .
)
# Use experimental glm features
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

# Collect all libraries
set(ALL_LIBS
    ${OPENGL_LIBRARY}
    glfw
    ${OpenCV_LIBS}
)

add_definitions(
    -D_CRT_SECURE_NO_WARNINGS
)

add_executable(OpenCVLab
    main.cpp
    Camera.cpp
    Object.cpp
    Quad.cpp
    Shader.cpp
    Scene.cpp
    Texture.cpp           
    TextureShader.cpp
    glad/glad_impl.cpp     
	)
target_link_libraries(OpenCVLab PRIVATE ${ALL_LIBS})


