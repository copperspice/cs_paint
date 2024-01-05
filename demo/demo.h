/***********************************************************************
*
* Copyright (c) 2019-2024 Barbara Geller
* Copyright (c) 2019-2024 Ansel Sermersheim
*
* This file is part of CsPaint.
*
* CsPaint is free software, released under the BSD 2-Clause license.
* For license details refer to LICENSE provided with this project.
*
* CsPaint is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://opensource.org/licenses/BSD-2-Clause
*
***********************************************************************/

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <SDL.h>

#include <cspaint.h>

#include <glm/glm.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <string>
#include <vector>

struct Vertex {
   glm::vec3 pos;
   glm::vec3 normal;
   glm::vec3 color;
   glm::vec2 texCoords;
};

//
constexpr const int NUM_MODELS = 7;

struct Uniform {
   glm::mat4 modelMatrix[NUM_MODELS];
   glm::mat4 viewMatrix;
   glm::mat4 projMatrix;
   glm::mat4 normalMatrix[NUM_MODELS];
   glm::vec3 lightPosition;
   float waveOffset;
};

//
std::shared_ptr<SDL_Window> init_window();

std::pair<CsPaint::context::handle, CsPaint::surface::handle> init_vulkan(std::shared_ptr<SDL_Window> window);

void init_render(CsPaint::device::handle device, CsPaint::surface::handle surface, CsPaint::command_pool::handle pool,
                 CsPaint::shader::handle vertexShader, CsPaint::shader::handle fragmentShader,
                 CsPaint::shader::handle textFragmentShader, CsPaint::renderpass::handle renderPass);

glm::mat4 transform_matrix(glm::vec3 rotation);

Uniform calculate_uniform(glm::mat4 transform_matrix, float aspect_ratio, float zoom_factor);

void resize_framebuffer();

void draw_frame(CsPaint::device::handle device, CsPaint::surface::handle surface, CsPaint::renderpass::handle renderPass,
                glm::mat4 transform_matrix, float zoom_factor);

extern const std::vector<uint32_t> vertexShaderData;
extern const std::vector<uint32_t> fragmentShaderData;
extern const std::vector<uint32_t> textFragmentShaderData;
extern const std::vector<uint8_t> fontTextureData;

const std::multimap<int, std::vector<Vertex>> &init_vertex_data();
const std::multimap<int, std::vector<Vertex>> &init_text_data();
