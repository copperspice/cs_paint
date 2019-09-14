# Find glslangValidator

find_program(
  CMAKE_GLSL_COMPILER
  NAMES "glslangValidator"
  DOC "LunarG glslangValidator"
  HINTS "$ENV{VULKAN_SDK}/bin"
)

mark_as_advanced(CMAKE_GLSL_COMPILER)

set(CMAKE_GLSL_SOURCE_FILE_EXTENSIONS vert;tesc;tese;geom;frag;comp;mesh;task;rgen;rint;rint;rahit;rchit;rmiss;rcall;glsl;hlsl)
set(CMAKE_GLSL_OUTPUT_EXTENSION .h)
set(CMAKE_GLSL_COMPILER_ENV_VAR "GLSLANGVALIDATOR")

configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/CMakeGLSLCompiler.cmake.in
  ${CMAKE_PLATFORM_INFO_DIR}/CMakeGLSLCompiler.cmake
)

