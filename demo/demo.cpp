/***********************************************************************
*
* Copyright (c) 2019-2021 Barbara Geller
* Copyright (c) 2019-2021 Ansel Sermersheim
*
* This file is part of CsPaint.
*
* CsPaint is free software, released under the BSD 2-Clause license.
* For license details refer to LICENSE provided with this project.
*
* CopperSpice is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://opensource.org/licenses/BSD-2-Clause
*
***********************************************************************/

#include "SDL.h"

#include <cspaint.h>
#include <demo.h>

int main(int argc, char *argv[])
{
   SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

   try {
      auto window             = init_window();
      auto [context, surface] = init_vulkan(window);
      auto device             = surface->graphicsDevice();
      auto vertexShader       = device->createShader(vertexShaderData);
      auto fragmentShader     = device->createShader(fragmentShaderData);
      auto textFragmentShader = device->createShader(textFragmentShaderData);

      auto graphicsQueue = device->graphicsQueue();
      auto transferQueue = device->transferQueue();
      auto renderPass    = device->createRenderPass();
      auto commandpool   = device->graphicsCommandPool();
      auto commandbuffer = commandpool->createCommandBuffer();

      init_render(device, surface, commandpool, vertexShader, fragmentShader, textFragmentShader, renderPass);

      bool run          = true;
      int count         = 60;
      float x_rotation  = 0;
      float y_rotation  = 0;
      float zoom_factor = 0;

      while (run) {
         SDL_Event evt;

         while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) {
               run = 0;
            } else if (evt.type == SDL_KEYDOWN) {
               if (evt.key.keysym.sym == SDLK_UP) {
                  x_rotation += 0.05;
               } else if (evt.key.keysym.sym == SDLK_DOWN) {
                  x_rotation -= 0.05;
               } else if (evt.key.keysym.sym == SDLK_LEFT) {
                  y_rotation += 0.05;
               } else if (evt.key.keysym.sym == SDLK_RIGHT) {
                  y_rotation -= 0.05;
               } else if (evt.key.keysym.sym == SDLK_KP_PLUS) {
                  zoom_factor = std::min(1.0f, zoom_factor + 0.05f);
               } else if (evt.key.keysym.sym == SDLK_KP_MINUS) {
                  zoom_factor = std::max(0.0f, zoom_factor - 0.05f);
               }
            }
         }

         draw_frame(device, surface, renderPass, transform_matrix(glm::vec3(x_rotation, y_rotation, 0.0)), zoom_factor);
      }

      abort();
      return 0;

   } catch (std::exception &e) {
      printf("%s, exiting\n", e.what());
      return 1;
   }

   SDL_Quit();

   return 0;
}
