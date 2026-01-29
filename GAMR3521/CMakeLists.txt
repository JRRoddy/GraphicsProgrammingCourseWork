set(APPLICATION_NAME "GAMR3521")

project(${APPLICATION_NAME} VERSION 1.0.0)

# Shader Files
set (PROJECT_SHADER_FILES
	"assets/shaders/phongVert.glsl"
	"assets/shaders/phongFrag.glsl" 
        "assets/shaders/BaseVertexShader.glsl" 
        "assets/shaders/BaseFragementShader.glsl" 
        "assets/shaders/skyBoxVert.glsl" 
        "assets/shaders/skyBoxFrag.glsl" 
        "assets/shaders/PostProcessingVert.glsl" 
        "assets/shaders/PostProcessingPassFrag.glsl"
        "assets/shaders/GammaCorrectionVert.glsl" 
        "assets/shaders/GammaCorrectionFrag.glsl"
        "assets/shaders/colourInverseFrag.glsl"
        "assets/shaders/relativeLuminanceFrag.glsl"
        "assets/shaders/luminanceSaturationFrag.glsl"
        "assets/shaders/luminanceContrastFrag.glsl"
        "assets/shaders/blurFrag.glsl"
        "assets/shaders/edgeDetectionFrag.glsl"
        "assets/shaders/visualiseDepthVert.glsl"
        "assets/shaders/visualiseDepthFrag.glsl"
        "assets/shaders/fogFrag.glsl"
        "assets/shaders/ZPrePassVert.glsl"
        "assets/shaders/ZPrePassFrag.glsl"
        "assets/shaders/shadowMapPrePassVert.glsl"
        "assets/shaders/shadowMapPrePassFrag.glsl"
        "assets/shaders/shadowMapVisualiseFrag.glsl"
        "assets/shaders/shadowMapVisualisationVert.glsl"
        "assets/shaders/deferredPrePassVert.glsl"
        "assets/shaders/deferredPrePassFrag.glsl"
        "assets/shaders/diffuseOnlyDRPassFrag.glsl"
        "assets/shaders/diffuseOnlyDRPassVert.glsl"
        "assets/shaders/floorGeo.glsl"
 	"assets/shaders/normalVisualVert.glsl"
        "assets/shaders/normalVisualGeo.glsl"
        "assets/shaders/normalVisualFrag.glsl"
        "assets/shaders/bilboardVert.glsl"
        "assets/shaders/bilboardGeo.glsl"
        "assets/shaders/bilboardFrag.glsl"
        "assets/shaders/moonBilboardFrag.glsl"
        "assets/shaders/normalOverlayVert.glsl"
	"assets/shaders/normalOverlayFrag.glsl"
 	"assets/shaders/sandingVert.glsl"
 	"assets/shaders/sandingGeo.glsl"
        "assets/shaders/sandingFrag.glsl"
        "assets/shaders/floorTSControl.glsl"
        "assets/shaders/floorTSEval.glsl"
        "assets/shaders/computeNormalsCDM.glsl"
        "assets/shaders/heightMapCompute.glsl"
        "assets/shaders/computeParticleInit.glsl"
        "assets/shaders/computeParticleUpdate.glsl"
        "assets/shaders/particleEmitVert.glsl"
        "assets/shaders/particleEmitGeo.glsl"
        "assets/shaders/particleEmitFrag.glsl"
        "assets/shaders/combineForwardAndDepthVert.glsl"
        "assets/shaders/combineForwardAndDepthFrag.glsl"
	"assets/shaders/ShipPBRDeferredFrag.glsl"
	"assets/shaders/PBRVert.glsl"
	"assets/shaders/PBRFrag.glsl"
        "assets/shaders/PBRDebugFrag.glsl"
 	"assets/shaders/PBRDebugVert.glsl"
        "assets/shaders/InitIBLEnviromentMap.glsl"
        "assets/shaders/InitIBLIrMap.glsl"
        "assets/shaders/specualrIndirectReflectance.glsl"
        "assets/shaders/BDRFPreCompute.glsl"

)

# Application Files
set (PROJECT_HEADER_FILES
	"include/app.hpp"
	"include/GAMR3521.hpp"
	"include/grid.hpp"
)
set (PROJECT_SOURCE_FILES
	"src/app.cpp"
	"src/GAMR3521.cpp"
	"src/grid.cpp"
)

# YOU WILL NEED TO ADD YOUR SCRIPT FILES HERE
set (SCRIPT_HEADER_FILES
	"scripts/include/camera.hpp"
	"scripts/include/controller.hpp"
	"scripts/include/controllerWASD.hpp"
	"scripts/include/followCamera.hpp"
	"scripts/include/rotation.hpp"
)
set (SCRIPT_SOURCE_FILES
	"scripts/src/camera.cpp"
	"scripts/src/controller.cpp"
	"scripts/src/controllerWASD.cpp"
	"scripts/src/followCamera.cpp"
	"scripts/src/rotation.cpp"
)

# Add excutable target and include directory
add_executable(${APPLICATION_NAME} ${PROJECT_SOURCE_FILES} ${PROJECT_HEADER_FILES} ${SCRIPT_SOURCE_FILES} ${SCRIPT_HEADER_FILES})
target_include_directories(${APPLICATION_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${APPLICATION_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/DemonRenderer/include")
target_include_directories(${APPLICATION_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/scripts/include/") # For scripts, might change this

# Visual Studio filters
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "src" FILES ${PROJECT_SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "include" FILES ${PROJECT_HEADER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/scripts" PREFIX "scripts" FILES ${SCRIPT_HEADER_FILES} ${SCRIPT_SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders" PREFIX "shaders" FILES ${PROJECT_SHADER_FILES})

# Add the shader files to the target so they appear in Visual Studio
target_sources(${APPLICATION_NAME} PRIVATE ${PROJECT_SHADER_FILES})

# Use C++20
set_target_properties(${APPLICATION_NAME} PROPERTIES CXX_STANDARD 20)

target_link_libraries(${APPLICATION_NAME} PRIVATE "DemonRenderer")

set_target_properties(${APPLICATION_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
