add_library(CsPaint SHARED
   src/cspaint_buffer.cpp
   src/cspaint_commandbuffer.cpp
   src/cspaint_commandpool.cpp
   src/cspaint_context.cpp
   src/cspaint_descriptor.cpp
   src/cspaint_descriptorpool.cpp
   src/cspaint_device.cpp
   src/cspaint_io.cpp
   src/cspaint_image.cpp
   src/cspaint_memorypool.cpp
   src/cspaint_pipeline.cpp
   src/cspaint_queue.cpp
   src/cspaint_renderpass.cpp
   src/cspaint_shader.cpp
   src/cspaint_surface.cpp
   src/cspaint_swapchain.cpp
)

target_include_directories(CsPaint
   PUBLIC
   ${Vulkan_INCLUDE_DIRS}
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
   $<INSTALL_INTERFACE:include/CsPaint>)

target_link_libraries(CsPaint ${Vulkan_LIBRARIES} glm)
target_compile_features(CsPaint PUBLIC cxx_std_17)

if(MSVC)
   set_target_properties(CsPaint
      PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE
   )
endif()

install(
   TARGETS CsPaint
   EXPORT  CsPaintLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS}
   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
