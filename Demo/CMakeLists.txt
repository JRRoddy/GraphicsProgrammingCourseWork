set(APPLICATION_NAME "Demo")

project(${APPLICATION_NAME} VERSION 1.0.0)

# Application Files
set (PROJECT_HEADER_FILES
	"include/app.hpp"
	"include/demo.hpp"
)
set (PROJECT_SOURCE_FILES
	"src/app.cpp"
	"src/demo.cpp"
)

# YOU WILL NEED TO ADD YOUR SCRIPT FILES HERE
set (SCRIPT_HEADER_FILES
	"scripts/include/camera.hpp"
)
set (SCRIPT_SOURCE_FILES
	"scripts/src/camera.cpp"
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

# Use C++20
set_target_properties(${APPLICATION_NAME} PROPERTIES CXX_STANDARD 20)

target_link_libraries(${APPLICATION_NAME} PRIVATE "DemonRenderer")

set_target_properties(${APPLICATION_NAME} PROPERTIES FOLDER "Demo")
set_target_properties(${APPLICATION_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
