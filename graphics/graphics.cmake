#graphics engine cpp sources. defines RD_API_WEBGL to be compiled

add_definitions(-DRD_API_WEBGL)
add_definitions(-D__EMSCRIPTEN__)

set(GRAPHICS_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/)

message(${GRAPHICS_SOURCE_DIR})

set(GRAPHICS_SOURCE_FILES
			${GRAPHICS_SOURCE_DIR}/objects/webgl/gl/GLState.cpp
			${GRAPHICS_SOURCE_DIR}/objects/buffer.cpp
			${GRAPHICS_SOURCE_DIR}/objects/device.cpp
			${GRAPHICS_SOURCE_DIR}/objects/framebuffer.cpp
			${GRAPHICS_SOURCE_DIR}/objects/mipmap_gen.cpp
			${GRAPHICS_SOURCE_DIR}/objects/pipeline_state.cpp
			${GRAPHICS_SOURCE_DIR}/objects/render_pass.cpp
			${GRAPHICS_SOURCE_DIR}/objects/shader.cpp
			${GRAPHICS_SOURCE_DIR}/objects/shader_desc.cpp
			${GRAPHICS_SOURCE_DIR}/objects/shader_parser.cpp
			${GRAPHICS_SOURCE_DIR}/objects/shader_resource_manager.cpp
			${GRAPHICS_SOURCE_DIR}/objects/texture.cpp
			${GRAPHICS_SOURCE_DIR}/objects/uniform_buffer.cpp
			${GRAPHICS_SOURCE_DIR}/objects/vertex_buffer.cpp
			
			${GRAPHICS_SOURCE_DIR}/utils/platform.cpp
			
			${GRAPHICS_SOURCE_DIR}/ext/material/material.cpp
			# ${GRAPHICS_SOURCE_DIR}/ext/model/mesh.cpp
)
			
