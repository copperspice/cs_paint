/***********************************************************************
*
* Copyright (c) 2019-2025 Barbara Geller
* Copyright (c) 2019-2025 Ansel Sermersheim
*
* This file is part of CsPaint.
*
* CsPaint is free software which is released under the BSD 2-Clause license.
* For license details refer to the LICENSE provided with this project.
*
* CsPaint is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://opensource.org/licenses/BSD-2-Clause
*
***********************************************************************/

#ifndef INCLUDED_CSPAINT_RENDERPASS_H
#define INCLUDED_CSPAINT_RENDERPASS_H

#include <any>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#include <cspaint_fwd.h>

namespace CsPaint
{
class renderpass
{
 public:
   renderpass(std::shared_ptr<const device> device, vk::UniqueRenderPass pass);

   renderpass(renderpass &&) = delete;
   void operator=(renderpass &&) = delete;

   vk::RenderPass get()
   {
      return *m_vk_renderpass;
   }

   using handle = std::shared_ptr<renderpass>;

 private:
   std::shared_ptr<const device> m_device;
   vk::UniqueRenderPass m_vk_renderpass;
};
};

#endif
