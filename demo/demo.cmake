if(sdl2_FOUND)
  # Create the target if it isn't there, this can happen on a system that has a
  # very simple (old) find-module for SDL2.
  if (NOT TARGET SDL2::SDL2)
    add_library(SDL2::SDL2 INTERFACE IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${SDL2_LIBRARIES}"
      )
  endif()


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

  target_include_directories(CsPaintDemo PRIVATE SDL2::SDL2
    ${CMAKE_CURRENT_SOURCE_DIR}/demo
    ${CMAKE_CURRENT_BINARY_DIR}/demo)

  target_link_libraries(CsPaintDemo SDL2::SDL2 CsPaint)
endif()
