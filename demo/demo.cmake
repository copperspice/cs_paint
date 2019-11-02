if(sdl2_FOUND)
   # create the SDL2 target if not found, this can happen when the find-module
   # shipped with SDL2 is not modern

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
      demo/init.cpp
      demo/prototype.cpp
      ${CMAKE_CURRENT_BINARY_DIR}/demo/resources/shader.frag.h
      ${CMAKE_CURRENT_BINARY_DIR}/demo/resources/shader.vert.h
      ${CMAKE_CURRENT_BINARY_DIR}/demo/resources/text.frag.h
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
      ${CMAKE_CURRENT_BINARY_DIR}/demo
   )

   target_link_libraries(CsPaintDemo SDL2::SDL2 CsPaint)

   install(TARGETS CsPaintDemo DESTINATION ${CMAKE_INSTALL_BINDIR})

   if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
      # nothing

   elseif(CMAKE_SYSTEM_NAME MATCHES "(Linux|OpenBSD|FreeBSD|NetBSD|DragonFly)")
      # nothing

   elseif(MSVC)

      if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
         # 64-bit
         install(FILES ${SDL2_PREFIX}/lib/x64/sdl2.dll DESTINATION "${CMAKE_INSTALL_BINDIR}")

      else ()
         #32-bit
         install(FILES ${SDL2_PREFIX}/lib/x86/sdl2.dll DESTINATION "${CMAKE_INSTALL_BINDIR}")

      endif ()

   elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
      install(FILES ${SDL2_PREFIX}/bin/sdl2.dll DESTINATION "${CMAKE_INSTALL_BINDIR}")

   endif()

endif()
