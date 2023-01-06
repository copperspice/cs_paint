/***********************************************************************
*
* Copyright (c) 2019-2023 Barbara Geller
* Copyright (c) 2019-2023 Ansel Sermersheim
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

#include <demo.h>

#include "SDL_vulkan.h"

std::shared_ptr<SDL_Window> init_window()
{
   const uint32_t width  = 1280;
   const uint32_t height = 720;

   auto retval =
      std::shared_ptr<SDL_Window>(SDL_CreateWindow("CsPaint Demonstration", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                                   width, height, SDL_WINDOW_VULKAN),
                                  [](SDL_Window *w) { SDL_DestroyWindow(w); });

   if (!retval) {
      throw std::runtime_error(std::string("SDL_CreateWindow failed, ") + SDL_GetError());
   }

   return retval;
}

std::pair<CsPaint::context::handle, CsPaint::surface::handle> init_vulkan(std::shared_ptr<SDL_Window> window)
{
   unsigned int count;
   if (!SDL_Vulkan_GetInstanceExtensions(window.get(), &count, nullptr)) {
      throw std::runtime_error(std::string("SDL_Vulkan_GetInstanceExtensions failed, ") + SDL_GetError());
   }

   std::vector<const char *> extensions;
   extensions.resize(count);
   if (!SDL_Vulkan_GetInstanceExtensions(window.get(), &count, &extensions[0])) {
      throw std::runtime_error(std::string("SDL_Vulkan_GetInstanceExtensions failed, ") + SDL_GetError());
   }

   CsPaint::context::handle context = CsPaint::context::create("Triangle sample", 1, extensions);

   VkSurfaceKHR vk_surface;
   if (!SDL_Vulkan_CreateSurface(window.get(), context->instance(), &vk_surface)) {
      throw std::runtime_error(std::string("SDL_Vulkan_CreateSurface failed, ") + SDL_GetError());
   }

   return std::make_pair(context, CsPaint::surface::create(context, vk_surface));
}
