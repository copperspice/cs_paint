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

#ifndef INCLUDED_CSPAINT_DEVICE_H
#define INCLUDED_CSPAINT_DEVICE_H

#include <any>
#include <future>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#include <cspaint_fwd.h>

namespace CsPaint
{

class device : public std::enable_shared_from_this<device>
{
 private:
   struct private_tag {
   };

 public:
   using handle = std::shared_ptr<device>;

   device(CsPaint::context_handle context_handle, vk::PhysicalDevice vk_device, private_tag = private_tag{});

   buffer_handle createBuffer(uint64_t bufferSize, vk::BufferUsageFlagBits usage,
                              vk::SharingMode mode = vk::SharingMode::eExclusive);

   template <typename T, typename Alloc>
   buffer_handle
   createBuffer(const std::vector<T, Alloc> &vec, vk::BufferUsageFlagBits usage, vk::SharingMode = vk::SharingMode::eExclusive);

   buffer_handle createBuffer(const void *data, uint64_t size, vk::BufferUsageFlagBits usage,
                              vk::SharingMode = vk::SharingMode::eExclusive);

   image_handle createImage(uint32_t width, uint32_t height, vk::Format imageFormat, vk::ImageTiling imageTiling,
                            vk::ImageUsageFlags imageUsage, vk::MemoryPropertyFlags memoryFlags, vk::ImageAspectFlags aspectFlags);

  std::future<image_handle> createTexture(const void *data, uint64_t size, uint32_t width, uint32_t height, vk::Format imageFormat,
                              vk::MemoryPropertyFlags memoryFlags, vk::ImageAspectFlags aspectFlags);

  template <typename T, typename Alloc>
  std::future<image_handle> createTexture(const std::vector<T, Alloc> data, uint32_t width, uint32_t height,
                                          vk::Format imageFormat, vk::MemoryPropertyFlags memoryFlags,
                                          vk::ImageAspectFlags aspectFlags);

  descriptor_pool_handle createDescriptorPool(uint32_t size, vk::DescriptorType, uint32_t count);

  shader_handle createShader(std::vector<char> input) const;
  shader_handle createShader(std::vector<uint32_t> input) const;

  renderpass_handle createRenderPass() const;

  command_pool_handle graphicsCommandPool() const;
  command_pool_handle transferCommandPool() const;

  queue_handle graphicsQueue() const;
  queue_handle transferQueue() const;

  const vk::PhysicalDevice &physicalDevice() const;
  const vk::Device &graphicsDevice() const;

  bool hasDeviceMemory();

  memorypool_handle memoryPool(vk::MemoryPropertyFlags memoryProperties);

  device(const device &) = delete;
  void operator=(const device &) = delete;

private:
  std::pair<vk::UniqueDevice, std::vector<vk::Queue>> createLogicalDevice(const vk::QueueFamilyProperties &properties,
                                                                          uint32_t id);

  context_handle m_context;
  vk::PhysicalDevice m_device;
  vk::UniqueDevice m_graphicsDevice;
  vk::UniqueDevice m_transferDevice;
  uint32_t m_graphicsCommandQueueFamily;
  uint32_t m_transferCommandQueueFamily;
  mutable command_pool_handle m_graphicsCommandPool;
  mutable command_pool_handle m_transferCommandPool;
  std::vector<vk::Queue> m_graphicsQueues;
  std::vector<vk::Queue> m_transferQueues;
};
}; // namespace CsPaint

template <typename T, typename Alloc>
CsPaint::buffer_handle
CsPaint::device::createBuffer(const std::vector<T, Alloc> &vec, vk::BufferUsageFlagBits usage, vk::SharingMode sharingMode)
{
   return createBuffer(static_cast<const void *>(vec.data()), vec.size() * sizeof(T), usage, sharingMode);
}

template <typename T, typename Alloc>
std::future<CsPaint::image_handle>
CsPaint::device::createTexture(const std::vector<T, Alloc> vec, uint32_t width, uint32_t height, vk::Format imageFormat,
                               vk::MemoryPropertyFlags memoryFlags, vk::ImageAspectFlags aspectFlags)
{
   return createTexture(static_cast<const void *>(vec.data()), vec.size() * sizeof(T), width, height, imageFormat, memoryFlags,
                       aspectFlags);
}


#endif
