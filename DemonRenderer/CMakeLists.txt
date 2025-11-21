cmake_minimum_required(VERSION 3.10...3.29)

set(RENDERER_NAME "DemonRenderer")

# Library Files
set (RENDERER_HEADER_FILES
	"include/DemonRenderer.hpp"
	"include/core/entryPoint.hpp"
	"include/core/application.hpp"
	"include/core/log.hpp"
	"include/core/timer.hpp"
	"include/core/layer.hpp"
	"include/core/layerStack.hpp"
	"include/core/physics.hpp"
	"include/core/randomiser.hpp"
    "include/events/event.hpp"
    "include/events/eventHandler.hpp"
    "include/events/events.hpp"
    "include/events/keyEvents.hpp"
    "include/events/mouseEvents.hpp"
    "include/events/windowEvents.hpp"
	"include/windows/windowsSystem.hpp"
	"include/windows/GLFWSystem.hpp"
	"include/windows/window.hpp"
	"include/windows/GLFWWindowImpl.hpp"
	"include/windows/graphicsContext.hpp"
	"include/windows/GLFW_GL_GC.hpp"
	"include/buffers/VBO.hpp"
	"include/buffers/IBO.hpp"
	"include/buffers/VAO.hpp"
	"include/buffers/VBOLayout.hpp"
	"include/buffers/UBO.hpp"
	"include/buffers/UBOLayout.hpp"
	"include/buffers/UBOmanager.hpp"
	"include/buffers/FBO.hpp"
	"include/buffers/FBOlayout.hpp"
	"include/buffers/RBO.hpp"
	"include/buffers/SSBO.hpp"
	"include/assets/shader.hpp"
	"include/assets/textureBase.hpp"
	"include/assets/cubeMap.hpp"
	"include/assets/bindlessCubeMap.hpp"
	"include/assets/bindlessTexture.hpp"
	"include/assets/texture.hpp"
	"include/assets/managedTexture.hpp"
	"include/assets/textureUnitManager.hpp"
	"include/assets/mesh.hpp"
	"include/assets/image.hpp"
	"include/rendering/material.hpp"
	"include/rendering/camera.hpp"
	"include/rendering/lights.hpp"
	"include/rendering/renderPass.hpp"
	"include/rendering/depthOnlyPass.hpp"
	"include/rendering/computePass.hpp"
	"include/rendering/UBObasePass.hpp"
	"include/rendering/scene.hpp"
	"include/rendering/renderer.hpp"
	"include/rendering/uniformDataTypes.hpp"
	"include/gameObjects/actor.hpp"
	"include/gameObjects/collidable.hpp"
	"include/gameObjects/ordering.hpp"
	"include/gameObjects/renderable.hpp"
	"include/gameObjects/scriptable.hpp"
	"include/gameObjects/transformable.hpp"
)
set (RENDERER_SOURCE_FILES
	"src/core/application.cpp"
	"src/core/log.cpp"
	"src/core/layerStack.cpp"
	"src/core/timer.cpp"
	"src/core/randomiser.cpp"
	"src/core/physics.cpp"
	"src/windows/GLFWWindowImpl.cpp"
	"src/windows/GLFW_GL_GC.cpp"
	"src/buffers/VBO.cpp"
	"src/buffers/IBO.cpp"
	"src/buffers/VAO.cpp"
	"src/buffers/VBOLayout.cpp"
	"src/buffers/UBO.cpp"
	"src/buffers/UBOLayout.cpp"
	"src/buffers/UBOmanager.cpp"
	"src/buffers/FBO.cpp"
	"src/buffers/RBO.cpp"
    "src/buffers/SSBO.cpp"
	"src/assets/shader.cpp"
	"src/assets/textureBase.cpp"
	"src/assets/managedTexture.cpp"
	"src/assets/cubeMap.cpp"
	"src/assets/bindlessCubeMap.cpp"
	"src/assets/bindlessTexture.cpp"
	"src/assets/textureUnitManager.cpp"
	"src/assets/mesh.cpp"
	"src/assets/image.cpp"
	"src/rendering/material.cpp"
	"src/rendering/renderer.cpp"
	"src/rendering/renderPass.cpp"
	"src/rendering/depthOnlyPass.cpp"
	"src/rendering/UBObasePass.cpp"
)

# Add library target (renderer) and include directory
add_library(${RENDERER_NAME} ${RENDERER_SOURCE_FILES} ${RENDERER_HEADER_FILES})
target_include_directories(${RENDERER_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
# Visual Studio filters
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "src" FILES ${RENDERER_SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "include" FILES ${RENDERER_HEADER_FILES})

# Add external libraries
include(FetchContent)

find_package(OpenGL REQUIRED)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY  https://github.com/gabime/spdlog.git
    GIT_TAG         v1.x
)


set(GLM_ENABLE_CXX_20 ON CACHE BOOL "" FORCE)
set(GLM_BUILD_LIBRARY ON CACHE BOOL "" FORCE)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 		1.0.1
)

FetchContent_Declare(
	glfw 
	GIT_REPOSITORY	https://github.com/glfw/glfw.git 
	GIT_TAG			3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/SimonCouplandDMU/DMUGlad.git
  GIT_TAG        gl4.6_core_ARB_bindless_texure
 )

set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)

FetchContent_Declare(
	assimp
	GIT_REPOSITORY	https://github.com/assimp/assimp.git
	GIT_TAG			v5.4.0
)

FetchContent_Declare(GSL
    GIT_REPOSITORY https://github.com/microsoft/GSL
    GIT_TAG v4.0.0
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(spdlog glm glfw glad assimp GSL)

# IMGUI needs a bit more fiddling with...  

set(IMGUI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/imgui-")
set(IMGUI_SOURCE_DIR "${IMGUI_ROOT}src")
FetchContent_Populate(imgui
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.4-docking.zip
  SOURCE_DIR "${IMGUI_ROOT}src"
  SUBBUILD_DIR "${IMGUI_ROOT}subbuild"
  BINARY_DIR "${IMGUI_ROOT}build"
)

add_library(imgui_glfw STATIC
  ${IMGUI_SOURCE_DIR}/imgui.cpp
  ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
  ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
  ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
  ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
  ${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

set_target_properties(imgui_glfw PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${IMGUI_ROOT}build"
)

target_link_libraries(imgui_glfw PRIVATE glfw OpenGL::GL)

target_include_directories(imgui_glfw
PUBLIC
  ${IMGUI_SOURCE_DIR}
  ${IMGUI_SOURCE_DIR}/backends
)


# Add and create STB image files specifically
set(STB_IMAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/stb_image")
file(DOWNLOAD https://raw.githubusercontent.com/nothings/stb/master/stb_image.h  "${STB_IMAGE_DIR}/stbImage/stb_image.h" STATUS _stat TLS_VERIFY ON LOG _log)
#message("status  ${_stat} log ${_log}")

file(WRITE "${STB_IMAGE_DIR}/stbImage/stb_image.cpp" "#define STB_IMAGE_IMPLEMENTATION\n#include \"stb_image.h\"")

add_library(stb_image_impl STATIC "${STB_IMAGE_DIR}/stbImage/stb_image.cpp")

target_include_directories(stb_image_impl PUBLIC "${STB_IMAGE_DIR}")

# Use C++20 everywhere
set_target_properties(${APPLICATION_NAME} PROPERTIES CXX_STANDARD 20)
set_target_properties(${RENDERER_NAME} PROPERTIES CXX_STANDARD 20)
set_target_properties(spdlog PROPERTIES CXX_STANDARD 20)
set_target_properties(glm PROPERTIES CXX_STANDARD 20)
set_target_properties(glfw PROPERTIES CXX_STANDARD 20)
set_target_properties(glad PROPERTIES CXX_STANDARD 20)
set_target_properties(assimp PROPERTIES CXX_STANDARD 20)
set_target_properties(imgui_glfw PROPERTIES CXX_STANDARD 20)
set_target_properties(stb_image_impl PROPERTIES CXX_STANDARD 20)


target_link_libraries(${RENDERER_NAME} PUBLIC OpenGL::GL)
target_link_libraries(${RENDERER_NAME} PUBLIC spdlog)
target_link_libraries(${RENDERER_NAME} PUBLIC glm)
target_link_libraries(${RENDERER_NAME} PUBLIC glfw)
target_link_libraries(${RENDERER_NAME} PUBLIC glad)
target_link_libraries(${RENDERER_NAME} PUBLIC assimp)
target_link_libraries(${RENDERER_NAME} PUBLIC Microsoft.GSL::GSL)
target_link_libraries(${RENDERER_NAME} PUBLIC imgui_glfw)
target_link_libraries(${RENDERER_NAME} PUBLIC stb_image_impl)

set_target_properties(${RENDERER_NAME} PROPERTIES FOLDER "DemonRenderer")

set_target_properties(glad PROPERTIES FOLDER "DemonRenderer/Deps")

set_target_properties(assimp PROPERTIES FOLDER "DemonRenderer/Deps/Assimp")
set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "DemonRenderer/Deps/Assimp")
set_target_properties(zlibstatic PROPERTIES FOLDER "DemonRenderer/Deps/Assimp")

set_target_properties(glfw PROPERTIES FOLDER "DemonRenderer/Deps/GLFW")
set_target_properties(uninstall PROPERTIES FOLDER "DemonRenderer/Deps/GLFW")
set_target_properties(update_mappings PROPERTIES FOLDER "DemonRenderer/Deps/GLFW")

set_target_properties(imgui_glfw PROPERTIES FOLDER "DemonRenderer/Deps")

set_target_properties(glm PROPERTIES FOLDER "DemonRenderer/Deps")


set_target_properties(spdlog PROPERTIES FOLDER "DemonRenderer/Deps")

set_target_properties(stb_image_impl PROPERTIES FOLDER "DemonRenderer/Deps")

