/***********************************************************************
*
* Copyright (c) 2019-2022 Barbara Geller
* Copyright (c) 2019-2022 Ansel Sermersheim
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

#ifndef INCLUDED_CSPAINT_SURFACE_H
#define INCLUDED_CSPAINT_SURFACE_H

#include <cspaint_context.h>
#include <cspaint_fwd.h>

namespace CsPaint
{
class surface : public std::enable_shared_from_this<surface>
{
 private:
   struct private_tag {
   };

 public:
   surface(context_handle context_handle, vk::SurfaceKHR surface, private_tag = private_tag{});

   surface(const surface &) = delete;
   void operator=(const surface &) = delete;

   ~surface();

   using handle = std::shared_ptr<surface>;

   static handle create(context_handle context_handle, vk::SurfaceKHR vk_surface);

   device_handle graphicsDevice() const;

   swapchain_handle createSwapchain(renderpass_handle renderpass,
                                    std::vector<CsPaint::image_handle> images = std::vector<CsPaint::image_handle>());

   pipeline_handle createPipeline(renderpass_handle renderpass, const vk::PipelineVertexInputStateCreateInfo &vertexInputInfo,
                                  const std::vector<vk::PipelineShaderStageCreateInfo> &shaderStages,
                                  const vk::PipelineLayout &pipelineLayout);

   void recalculateSize();

   uint32_t width() const
   {
      return m_width;
   }

   uint32_t height() const
   {
      return m_height;
   }

 private:
   context_handle m_context;
   vk::SurfaceKHR m_surface;
   device_handle m_graphicsDevice;
   vk::Format m_format;
   vk::ColorSpaceKHR m_colorSpace;
   vk::PresentModeKHR m_mode;
   uint32_t m_width;
   uint32_t m_height;
};
}; // namespace CsPaint

#endif
