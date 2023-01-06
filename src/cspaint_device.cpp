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

#include <cspaint_device.h>

#include <cspaint_buffer.h>
#include <cspaint_commandbuffer.h>
#include <cspaint_commandpool.h>
#include <cspaint_context.h>
#include <cspaint_descriptorpool.h>
#include <cspaint_image.h>
#include <cspaint_queue.h>
#include <cspaint_renderpass.h>
#include <cspaint_shader.h>
#include <cspaint_surface.h>
#include <cspaint_swapchain.h>
#include <cspaint_util.h>

static_assert(CsPaint::util::nomove_nocopy_nodefault<CsPaint::device>());

template <typename T, typename Flags>
std::vector<T> filter_sort_queues(std::vector<T> data, Flags f)
{
   using namespace CsPaint::util;
   auto retval = filter_vector(std::move(data), [f](auto &arg) {
      auto &[properties, index] = arg;
      return (properties.queueFlags & f) != Flags{};
   });

   std::stable_sort(retval.begin(), retval.end(),
                    [](auto &a, auto &b) { return uint32_t(a.first.queueFlags) < uint32_t(b.first.queueFlags); });

   return retval;
}

CsPaint::device::device(CsPaint::context::handle context_handle, vk::PhysicalDevice vk_device, private_tag)
   : m_context(context_handle), m_device(vk_device)
{
   using namespace CsPaint::util;

   auto queues = map_vector(m_device.getQueueFamilyProperties(), [i = 0](auto data) mutable {
      // Any queue that supports graphics or compute also supports transfer implicitly
      if (data.queueFlags & (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)) {
         data.queueFlags |= vk::QueueFlagBits::eTransfer;
      }

      return std::make_pair(std::move(data), i++);
   });

   auto graphicsQueues = filter_sort_queues(queues, vk::QueueFlagBits::eGraphics);
   auto transferQueues = filter_sort_queues(queues, vk::QueueFlagBits::eTransfer);

   if (graphicsQueues.empty()) {
      throw std::runtime_error("No usable graphics queue in CsPaint::device::device()");
   }

   if (transferQueues.empty()) {
      throw std::runtime_error("No usable transfer queue in CsPaint::device::device()");
   }

   std::tie(m_graphicsDevice, m_graphicsQueues) =
      createLogicalDevice(graphicsQueues.front().first, graphicsQueues.front().second);
   m_graphicsCommandQueueFamily = graphicsQueues.front().second;

   if (graphicsQueues.front() == transferQueues.front()) {
      /* The rendering and transfer hardware are not seperate on this GPU*/
      m_transferQueues.push_back(m_graphicsQueues.back());
      m_transferCommandQueueFamily = m_graphicsCommandQueueFamily;
   } else {
      std::tie(m_transferDevice, m_transferQueues) =
         createLogicalDevice(transferQueues.front().first, transferQueues.front().second);
      m_transferCommandQueueFamily = transferQueues.front().second;
      m_transferCommandPool = std::make_shared<command_pool>(*m_transferDevice, transferQueues.front().second);
   }
}

std::pair<vk::UniqueDevice, std::vector<vk::Queue>>
CsPaint::device::createLogicalDevice(const vk::QueueFamilyProperties &properties, uint32_t id)
{
   const uint32_t count = properties.queueCount;

   std::vector<float> priorities(count, 1.0f);

   vk::DeviceQueueCreateInfo createInfo({}, id, count, priorities.data());

   std::vector<const char *> extensions;

   if (properties.queueFlags & vk::QueueFlagBits::eGraphics) {
         extensions.push_back("VK_KHR_swapchain");
   }

   vk::UniqueDevice dev =
      m_device.createDeviceUnique(vk::DeviceCreateInfo({}, 1, &createInfo, 0, nullptr, extensions.size(), extensions.data()));

   std::vector<vk::Queue> queues;
   queues.reserve(count);

   for (uint32_t i = 0; i < count; ++i) {
      queues.push_back(dev->getQueue(id, i));
   }

   return std::make_pair(std::move(dev), std::move(queues));
}

CsPaint::shader::handle CsPaint::device::createShader(std::vector<char> data) const
{
   return std::make_shared<CsPaint::shader>(shared_from_this(), std::move(data));
}

CsPaint::shader::handle CsPaint::device::createShader(std::vector<uint32_t> data) const
{
   return std::make_shared<CsPaint::shader>(shared_from_this(), std::move(data));
}

CsPaint::renderpass::handle CsPaint::device::createRenderPass() const
{
   vk::Format swapChainImageFormat = vk::Format::eB8G8R8A8Unorm; // broom

   vk::AttachmentDescription colorAttachment = {{},
                                                swapChainImageFormat,
                                                vk::SampleCountFlagBits::e1,
                                                vk::AttachmentLoadOp::eClear,
                                                vk::AttachmentStoreOp::eStore,
                                                vk::AttachmentLoadOp::eDontCare,
                                                vk::AttachmentStoreOp::eDontCare,
                                                vk::ImageLayout::ePresentSrcKHR,
                                                vk::ImageLayout::ePresentSrcKHR};

   vk::AttachmentReference colorAttachmentRef = {0, vk::ImageLayout::eColorAttachmentOptimal};

   vk::AttachmentDescription depthAttachment = {{},
                                                vk::Format::eD32SfloatS8Uint,
                                                vk::SampleCountFlagBits::e1,
                                                vk::AttachmentLoadOp::eClear,
                                                vk::AttachmentStoreOp::eStore,
                                                vk::AttachmentLoadOp::eClear,
                                                vk::AttachmentStoreOp::eDontCare,
                                                vk::ImageLayout::eUndefined,
                                                vk::ImageLayout::eDepthStencilAttachmentOptimal};

   vk::AttachmentReference depthAttachmentRef = {1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

   vk::AttachmentDescription attachments[] = {colorAttachment, depthAttachment};

   vk::SubpassDescription subpass{{},      vk::PipelineBindPoint::eGraphics,
                                  0,       nullptr,
                                  1,       &colorAttachmentRef,
                                  nullptr, &depthAttachmentRef,
                                  0,       nullptr};

   vk::RenderPassCreateInfo renderPassInfo = {{}, 2, attachments, 1, &subpass, 0, nullptr};

   auto retval = m_graphicsDevice->createRenderPassUnique(renderPassInfo, nullptr);

   return std::make_shared<CsPaint::renderpass>(shared_from_this(), std::move(retval));
}

CsPaint::command_pool::handle CsPaint::device::graphicsCommandPool() const
{
   if (!m_graphicsCommandPool) {
      m_graphicsCommandPool = std::make_shared<command_pool>(*m_graphicsDevice, m_graphicsCommandQueueFamily);
   }
   return m_graphicsCommandPool;
}

CsPaint::command_pool::handle CsPaint::device::transferCommandPool() const
{
   if (!m_transferCommandPool) {
      m_transferCommandPool = std::make_shared<command_pool>(*m_transferDevice, m_transferCommandQueueFamily);
   }
   return m_transferCommandPool;
}

CsPaint::queue::handle CsPaint::device::graphicsQueue() const
{
   return std::make_shared<queue>(shared_from_this(), m_graphicsQueues.front());
}

CsPaint::queue::handle CsPaint::device::transferQueue() const
{
   return std::make_shared<queue>(shared_from_this(), m_transferQueues.front());
}

const vk::PhysicalDevice &CsPaint::device::physicalDevice() const
{
   return m_device;
}

const vk::Device &CsPaint::device::graphicsDevice() const
{
   return *m_graphicsDevice;
}

CsPaint::buffer::handle CsPaint::device::createBuffer(uint64_t bufferSize, vk::BufferUsageFlagBits usage, vk::SharingMode mode)
{
   return std::make_shared<CsPaint::buffer>(shared_from_this(),
                                            m_graphicsDevice->createBufferUnique({{}, bufferSize, usage, mode, 0, nullptr}));
}

CsPaint::buffer::handle CsPaint::device::createBuffer(const void *buffer, uint64_t bufferSize, vk::BufferUsageFlagBits usage,
                                                      vk::SharingMode mode)
{
   auto retval = createBuffer(bufferSize, usage, mode);
   retval->store(buffer, bufferSize);
   return retval;
}

CsPaint::descriptor_pool::handle CsPaint::device::createDescriptorPool(uint32_t size, vk::DescriptorType type, uint32_t count)
{

   vk::DescriptorPoolSize poolSize{type, count};
   auto descriptorPool = m_graphicsDevice->createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo{{}, size, 1, &poolSize});

   return std::make_shared<CsPaint::descriptor_pool>(shared_from_this(), std::move(descriptorPool));
}

CsPaint::image::handle CsPaint::device::createImage(uint32_t width, uint32_t height, vk::Format imageFormat,
                                                    vk::ImageTiling imageTiling, vk::ImageUsageFlags imageUsage,
                                                    vk::MemoryPropertyFlags memoryFlags, vk::ImageAspectFlags aspectFlags)
{

   vk::UniqueImage vk_image = m_graphicsDevice->createImageUnique(vk::ImageCreateInfo{{},
                                                                                      vk::ImageType::e2D,
                                                                                      imageFormat,
                                                                                      {width, height, 1},
                                                                                      1,
                                                                                      1,
                                                                                      vk::SampleCountFlagBits::e1,
                                                                                      imageTiling,
                                                                                      imageUsage,
                                                                                      vk::SharingMode::eExclusive,
                                                                                      0,
                                                                                      nullptr,
                                                                                      vk::ImageLayout::eUndefined});

   return std::make_shared<CsPaint::image>(shared_from_this(), std::move(vk_image), imageFormat, aspectFlags);
}

std::future<CsPaint::image::handle>
CsPaint::device::createTexture(const void *data, uint64_t size, uint32_t width, uint32_t height, vk::Format imageFormat,
                               vk::MemoryPropertyFlags memoryFlags, vk::ImageAspectFlags aspectFlags)
{
   std::promise<CsPaint::image::handle> promise;

   CsPaint::buffer::handle tmpBuffer = createBuffer(data, size, vk::BufferUsageFlagBits::eTransferSrc);

   CsPaint::image::handle image =
      createImage(width, height, imageFormat, vk::ImageTiling::eOptimal,
                  vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, memoryFlags, aspectFlags);

   auto transferPool                    = graphicsCommandPool();
   auto transferBuffer                  = transferPool->createCommandBuffer();
   auto commandBuffer                   = transferBuffer->buffer();
   vk::CommandBufferBeginInfo beginInfo = {};
   beginInfo.flags                      = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

   commandBuffer.begin(beginInfo);

   vk::ImageMemoryBarrier transferBarrier1{{},
                                           vk::AccessFlagBits::eTransferWrite,
                                           vk::ImageLayout::eUndefined,
                                           vk::ImageLayout::eTransferDstOptimal,
                                           0,
                                           0,
                                           image->getImage(),
                                           {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};

   commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, 0, nullptr, 0,
                                 nullptr, 1, &transferBarrier1);

   vk::BufferImageCopy bufferCopy{0, 0, 0, {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, {0, 0, 0}, {640, 480, 1}};
   commandBuffer.copyBufferToImage(tmpBuffer->getBuffer(), image->getImage(), vk::ImageLayout::eTransferDstOptimal, 1,
                                   &bufferCopy);

   vk::ImageMemoryBarrier transferBarrier2{vk::AccessFlagBits::eTransferWrite,
                                           vk::AccessFlagBits::eShaderRead,
                                           vk::ImageLayout::eTransferDstOptimal,
                                           vk::ImageLayout::eShaderReadOnlyOptimal,
                                           0,
                                           0,
                                           image->getImage(),
                                           {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};

   commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, 0, nullptr,
                                 0, nullptr, 1, &transferBarrier2);
   commandBuffer.end();

   vk::SubmitInfo submitInfo     = {};
   submitInfo.commandBufferCount = 1;
   submitInfo.pCommandBuffers    = &commandBuffer;

   auto vk_queue = graphicsQueue()->getQueue();
   vk_queue.submit(1, &submitInfo, nullptr);
   vk_queue.waitIdle();

   promise.set_value(std::move(image));

   return promise.get_future();
}
