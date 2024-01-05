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

#include <cspaint_surface.h>

#include <cspaint_device.h>
#include <cspaint_image.h>
#include <cspaint_pipeline.h>
#include <cspaint_renderpass.h>
#include <cspaint_swapchain.h>
#include <cspaint_util.h>

#include <algorithm>
#include <functional>
#include <vulkan/vulkan.hpp>

CsPaint::surface::surface(context::handle context_handle, vk::SurfaceKHR vk_surface, private_tag)
   : m_context(context_handle), m_surface(vk_surface)
{

   using namespace CsPaint::util;

   auto deviceQueues = map_vector(m_context->instance().enumeratePhysicalDevices(),
                                  [](auto device) { return std::make_pair(device, device.getQueueFamilyProperties()); });

   deviceQueues = filter_vector(std::move(deviceQueues), [this](auto arg) {
      auto &[device, queues] = arg;
      for (uint32_t i = 0; i < queues.size(); ++i) {
         if (queues[i].queueFlags & vk::QueueFlagBits::eGraphics && device.getSurfaceSupportKHR(i, m_surface)) {
            return true;
         }
      }
      return false;
   });

   if (deviceQueues.empty()) {
      throw std::runtime_error("No usable device in CsPaint::surface::surface()");
   }

   m_graphicsDevice = std::make_shared<CsPaint::device>(m_context, deviceQueues.front().first);

   auto &phys_device = m_graphicsDevice->physicalDevice();

   auto surfaceFormats = phys_device.getSurfaceFormatsKHR(m_surface);
   auto presentModes   = phys_device.getSurfacePresentModesKHR(m_surface);

   auto capabilities = phys_device.getSurfaceCapabilitiesKHR(m_surface);

   m_width  = capabilities.currentExtent.width;
   m_height = capabilities.currentExtent.height;

   if (surfaceFormats.empty()) {
      throw std::runtime_error("No surface formats available in CsPaint::device::device");
   }
   if (presentModes.empty()) {
      throw std::runtime_error("No presentation modes available in CsPaint::device::device");
   }

   m_format     = surfaceFormats.front().format;
   m_colorSpace = surfaceFormats.front().colorSpace;
   m_mode       = vk::PresentModeKHR::eFifo; // All GPUs must support FIFO presentation

   if (std::any_of(presentModes.begin(), presentModes.end(), [](auto &data) { return data == vk::PresentModeKHR::eMailbox; })) {
      // We prefer mailbox when it is available
      m_mode = vk::PresentModeKHR::eMailbox;
   }
}

CsPaint::surface::~surface()
{
   m_context->instance().destroy(m_surface);
}

CsPaint::surface::handle CsPaint::surface::create(context::handle context_handle, vk::SurfaceKHR vk_surface)
{
   return std::make_shared<surface>(context_handle, vk_surface);
}

CsPaint::device::handle CsPaint::surface::graphicsDevice() const
{
   return m_graphicsDevice;
}

void CsPaint::surface::recalculateSize()
{
   auto &phys_device = m_graphicsDevice->physicalDevice();

   auto capabilities = phys_device.getSurfaceCapabilitiesKHR(m_surface);

   m_width  = capabilities.currentExtent.width;
   m_height = capabilities.currentExtent.height;
}

CsPaint::swapchain::handle CsPaint::surface::createSwapchain(CsPaint::renderpass::handle renderpass,
                                                             std::vector<CsPaint::image::handle> images)
{
   constexpr const uint32_t Image_Count = 3;

   auto &phys_device = m_graphicsDevice->physicalDevice();

   auto capabilities = phys_device.getSurfaceCapabilitiesKHR(m_surface);

   auto swapchain = m_graphicsDevice->graphicsDevice().createSwapchainKHRUnique(
      vk::SwapchainCreateInfoKHR({}, m_surface, Image_Count, m_format, m_colorSpace,
                                 {capabilities.currentExtent.width, capabilities.currentExtent.height}, 1,
                                 vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
                                 vk::SharingMode::eExclusive, 0, nullptr, capabilities.currentTransform,
                                 vk::CompositeAlphaFlagBitsKHR::eOpaque, m_mode, VK_TRUE, nullptr));

   return std::make_shared<CsPaint::swapchain>(shared_from_this(), m_graphicsDevice, renderpass, std::move(swapchain), m_width,
                                               m_height, std::move(images));
}

CsPaint::pipeline::handle CsPaint::surface::createPipeline(CsPaint::renderpass::handle renderpass,
                                                           const vk::PipelineVertexInputStateCreateInfo &vertexInputInfo,
                                                           const std::vector<vk::PipelineShaderStageCreateInfo> &shaderStages,
                                                           const vk::PipelineLayout &pipelineLayout)
{
   return std::make_shared<CsPaint::pipeline>(m_graphicsDevice, shared_from_this(), renderpass, vertexInputInfo, shaderStages,
                                              pipelineLayout);
}
