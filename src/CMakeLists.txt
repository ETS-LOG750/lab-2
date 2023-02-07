# Add source files
SET(SOURCE_FILES 
	Main.cpp MainWindow.cpp ShaderProgram.cpp
)
set(HEADER_FILES 
	MainWindow.h ShaderProgram.h
)
set(SHADER_FILES 
	basic.vert basic.frag
)

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${SHADER_FILES})

# Definition (SHADER_FILES)
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")
target_compile_definitions(${PROJECT_NAME} PUBLIC SHADERS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})