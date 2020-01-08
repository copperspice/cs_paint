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

#include <cspaint_image.h>

#include <cspaint_device.h>
#include <cspaint_util.h>

#include <stdexcept>

static_assert(CsPaint::util::nomove_nocopy_nodefault<CsPaint::image>());

CsPaint::image::image(std::shared_ptr<device> device, vk::UniqueImage vk_image, vk::Format format,
                      vk::ImageAspectFlags aspectFlags)
   : m_device(std::move(device)), m_image(std::move(vk_image))
{
   const auto &vk_device   = m_device->graphicsDevice();
   auto memoryRequirements = vk_device.getImageMemoryRequirements(*m_image);

   auto memoryInfo = m_device->physicalDevice().getMemoryProperties();

   uint32_t i;
   for (i = 0; i < memoryInfo.memoryTypeCount; ++i) {
      if (memoryInfo.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits ::eHostVisible) {
         if (memoryRequirements.memoryTypeBits & (1 << i)) {
            break;
         }
      }
   }

   if (i == memoryInfo.memoryTypeCount) {
      for (i = 0; i < memoryInfo.memoryTypeCount; ++i) {
         if (memoryRequirements.memoryTypeBits & (1 << i)) {
            break;
         }
      }
   }

   if (i == memoryInfo.memoryTypeCount) {
      throw std::runtime_error(
         "CsPaint::image::image(): Unable to find a usable memory type");
   }

   vk::MemoryAllocateInfo allocInfo{memoryRequirements.size, i};

   m_memory = vk_device.allocateMemoryUnique(allocInfo);
   vk_device.bindImageMemory(*m_image, *m_memory, 0);

   m_view = vk_device.createImageViewUnique(
      vk::ImageViewCreateInfo{{},
                              *m_image,
                              vk::ImageViewType::e2D,
                              format,
                              {vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
                               vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity},
                              {aspectFlags, 0, 1, 0, 1}});
}
/*
void CsPaint::buffer::store(const void *data, uint64_t size)
{
   const auto &vk_device = m_device->graphicsDevice();

   void *deviceAddress = vk_device.mapMemory(*m_memory, 0, size, {});

   if (deviceAddress == nullptr) {
      throw std::runtime_error("Failed to map memory in CsPaint::buffer::store(void*, size_t)");
   }

   memcpy(deviceAddress, data, size);

   vk_device.unmapMemory(*m_memory);
}
*/
