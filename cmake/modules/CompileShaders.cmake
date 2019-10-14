function(compile_shaders)
  foreach(X ${ARGV})
    set(OUTPUT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${X}.h")
    set(INPUT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/${X}")

    get_filename_component(OUTPUT_DIR ${OUTPUT_FILE_NAME} DIRECTORY)

    add_custom_command(OUTPUT "${OUTPUT_FILE_NAME}"
      COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
      COMMAND ${CMAKE_GLSL_COMPILER} ${INPUT_FILE_NAME} -V --vn shader -o ${OUTPUT_FILE_NAME}
      DEPENDS ${INPUT_FILE_NAME})

  endforeach()
endfunction()
