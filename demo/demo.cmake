if(sdl2_FOUND)

  add_executable(CsPaintDemo
    demo/data.cpp
    demo/demo.cpp
    demo/prototype.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/demo/resources/shader.frag.h
    ${CMAKE_CURRENT_BINARY_DIR}/demo/resources/shader.vert.h
    ${CMAKE_CURRENT_BINARY_DIR}/demo/resources/text.frag.h
    ${CMAKE_CURRENT_BINARY_DIR}/demo/resources/text.vert.h
    )

  enable_language(GLSL)

  compile_shaders(
    demo/resources/shader.frag
    demo/resources/shader.vert
    demo/resources/text.frag
    demo/resources/text.vert
    )

  target_compile_definitions(CsPaintDemo PRIVATE
    DEMO_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/demo/resources"
    )

  target_include_directories(CsPaintDemo PRIVATE ${SDL2_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/demo
    ${CMAKE_CURRENT_BINARY_DIR}/demo)

  target_link_libraries(CsPaintDemo ${SDL2_LIBRARIES} CsPaint)
  target_compile_options(CsPaintDemo PUBLIC ${SDL2_CFLAGS_OTHER})
endif()
