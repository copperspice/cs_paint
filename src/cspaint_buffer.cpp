/***********************************************************************
*
* Copyright (c) 2019 Barbara Geller
* Copyright (c) 2019 Ansel Sermersheim
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

#include <cspaint_buffer.h>

#include <cspaint_device.h>
#include <cspaint_util.h>

#include <stdexcept>

static_assert(CsPaint::util::nomove_nocopy_nodefault<CsPaint::buffer>());

CsPaint::buffer::buffer(std::shared_ptr<device> device, vk::UniqueBuffer vk_buffer)
   : m_device(std::move(device)), m_buffer(std::move(vk_buffer))
{
   const auto &vk_device   = m_device->graphicsDevice();
   auto memoryRequirements = vk_device.getBufferMemoryRequirements(*m_buffer);

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
      throw std::runtime_error(
         "CsPaint::buffer::buffer(): Unable to find a memory type with VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT");
   }

   vk::MemoryAllocateInfo allocInfo{memoryRequirements.size, i};

   m_memory = vk_device.allocateMemoryUnique(allocInfo);
   vk_device.bindBufferMemory(*m_buffer, *m_memory, 0);
}

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
