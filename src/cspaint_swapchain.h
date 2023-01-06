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

#ifndef INCLUDED_CSPAINT_SWAPCHAIN_H
#define INCLUDED_CSPAINT_SWAPCHAIN_H

#include <vulkan/vulkan.hpp>

#include <cspaint_fwd.h>

namespace CsPaint
{

class swapchain
{
 private:
   struct private_tag {
   };

 public:
   using handle = std::shared_ptr<swapchain>;

   swapchain(std::shared_ptr<const surface> surface, CsPaint::device_handle device, CsPaint::renderpass_handle renderpass,
             vk::UniqueSwapchainKHR chain, uint32_t width, uint32_t height, std::vector<CsPaint::image_handle> imageAttachments);

   swapchain(const swapchain &) = delete;
   void operator=(const swapchain &) = delete;

   const vk::UniqueSwapchainKHR &chain() const
   {
      return m_swapchain;
   };

   std::vector<vk::Framebuffer> framebuffers();

 private:
   std::shared_ptr<const surface> m_surface;
   CsPaint::device_handle m_device;
   vk::UniqueSwapchainKHR m_swapchain;
   std::vector<std::pair<vk::UniqueImageView, vk::UniqueFramebuffer>> m_framebuffers;
};
}; // namespace CsPaint

#endif
