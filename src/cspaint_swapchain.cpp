/***********************************************************************
*
* Copyright (c) 2019-2020 Barbara Geller
* Copyright (c) 2019-2020 Ansel Sermersheim
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

#include <cspaint_swapchain.h>

#include <cspaint_device.h>
#include <cspaint_renderpass.h>
#include <cspaint_util.h>
#include <cspaint_image.h>

static_assert(CsPaint::util::nomove_nocopy_nodefault<CsPaint::swapchain>());

CsPaint::swapchain::swapchain(std::shared_ptr<const surface> surface, CsPaint::device::handle device,
                              CsPaint::renderpass::handle renderpass, vk::UniqueSwapchainKHR chain, uint32_t width,
                              uint32_t height, std::vector<CsPaint::image_handle> images)
   : m_surface(std::move(surface)), m_swapchain(std::move(chain))
{
   assert(m_swapchain);
   const auto &vk_device = device->graphicsDevice();
   auto vk_images        = vk_device.getSwapchainImagesKHR(*m_swapchain);

   auto format = vk::Format::eB8G8R8A8Unorm; // BROOM

   m_framebuffers = util::map_vector(vk_images, [&vk_device, format, width, height, &images, &renderpass](auto vk_image) {
      auto view = vk_device.createImageViewUnique(
         vk::ImageViewCreateInfo{{},
                                 vk_image,
                                 vk::ImageViewType::e2D,
                                 format,
                                 {vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
                                  vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity},
                                 {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}});

      std::vector<vk::ImageView> tmp_images;
      tmp_images.push_back(*view);
      for (const auto &item : images) {
         tmp_images.push_back(item->getImageView());
      }

      auto framebuffer =
        vk_device.createFramebufferUnique({{}, renderpass->get(), uint32_t(tmp_images.size()), tmp_images.data(), width, height, 1});

      return std::make_pair(std::move(view), std::move(framebuffer));
   });
}

std::vector<vk::Framebuffer> CsPaint::swapchain::framebuffers()
{
   return util::map_vector(m_framebuffers, [](const auto &x) { return *(x.second); });
}
