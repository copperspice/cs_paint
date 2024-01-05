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

#include <demo.h>

constexpr const int FRAME_COUNT = 2;

static uint32_t frameIndex = 0;
CsPaint::command_buffer_handle commandBuffers[FRAME_COUNT];
static VkFence frameFences[FRAME_COUNT]; // Create with VK_FENCE_CREATE_SIGNALED_BIT.
static VkSemaphore imageAvailableSemaphores[FRAME_COUNT];
static VkSemaphore renderFinishedSemaphores[FRAME_COUNT];
static CsPaint::pipeline::handle graphicsPipeline;
static CsPaint::pipeline::handle textPipeline;

static vk::VertexInputBindingDescription bindingDescription{0, sizeof(Vertex), vk::VertexInputRate::eVertex};
static std::vector<vk::VertexInputAttributeDescription> attributeDescriptions =
   {{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos)},
    {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)},
    {2, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)},
    {3, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoords)}};
static std::multimap<int, std::pair<CsPaint::buffer::handle, uint32_t>> vertexBuffers;
static std::multimap<int, std::pair<CsPaint::buffer::handle, uint32_t>> textVertexBuffers;
static std::vector<CsPaint::buffer::handle> uniformBuffers;

static vk::UniqueDescriptorSetLayout descriptorSetLayout;
static VkPipelineLayout pipelineLayout;

static vk::UniqueDescriptorSetLayout textDescriptorSetLayout;
static VkPipelineLayout textPipelineLayout;

static CsPaint::descriptor_pool::handle descriptorPool;
static std::vector<CsPaint::descriptor::handle> descriptors;

static CsPaint::descriptor_pool::handle textDescriptorPool;
static std::vector<CsPaint::descriptor::handle> textDescriptors;

static std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
static std::vector<vk::PipelineShaderStageCreateInfo> textShaderStages;
static CsPaint::image::handle depthBuffer;
static CsPaint::image::handle textImage;
static vk::Sampler sampler;
static CsPaint::swapchain::handle s_swapchain;
static std::vector<vk::Framebuffer> s_framebuffers;

void init_pipeline(CsPaint::device::handle device, CsPaint::surface::handle surface, CsPaint::renderpass::handle renderPass)
{
   vk::PipelineVertexInputStateCreateInfo vertexInputInfo{{},
                                                          1,
                                                          &bindingDescription,
                                                          uint32_t(attributeDescriptions.size()),
                                                          attributeDescriptions.data()};

   graphicsPipeline = surface->createPipeline(renderPass, vertexInputInfo, shaderStages, pipelineLayout);

   textPipeline = surface->createPipeline(renderPass, vertexInputInfo, textShaderStages, pipelineLayout);
}

void init_texture(CsPaint::device::handle device)
{
   textImage = device
                  ->createTexture(fontTextureData, 640, 480, vk::Format::eR8G8B8A8Unorm, vk::MemoryPropertyFlagBits::eDeviceLocal,
                                  vk::ImageAspectFlagBits::eColor)
                  .get();

   vk::SamplerCreateInfo samplerInfo = {};
   samplerInfo.magFilter             = vk::Filter::eLinear;
   samplerInfo.minFilter             = vk::Filter::eLinear;
   samplerInfo.anisotropyEnable      = VK_FALSE;
   samplerInfo.maxAnisotropy         = 1;

   sampler = device->graphicsDevice().createSampler(samplerInfo);
}

void init_render(CsPaint::device::handle device, CsPaint::surface::handle surface, CsPaint::command_pool::handle pool,
                 CsPaint::shader::handle vertexShader, CsPaint::shader::handle fragmentShader,
                 CsPaint::shader::handle textFragmentShader, CsPaint::renderpass::handle renderPass)
{
   for (int i = 0; i < FRAME_COUNT; ++i) {
      commandBuffers[i] = pool->createCommandBuffer();
   }

   init_texture(device);

   depthBuffer = device->createImage(surface->width(), surface->height(), vk::Format::eD32SfloatS8Uint, vk::ImageTiling::eOptimal,
                                     vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal,
                                     vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
   std::vector<CsPaint::image::handle> imageHandles;
   imageHandles.push_back(depthBuffer);
   s_swapchain    = surface->createSwapchain(renderPass, imageHandles);
   s_framebuffers = s_swapchain->framebuffers();

   const auto &vk_device = device->graphicsDevice();

   VkSemaphoreCreateInfo semaphoreCreateInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
   vkCreateSemaphore(vk_device, &semaphoreCreateInfo, 0, &imageAvailableSemaphores[0]);
   vkCreateSemaphore(vk_device, &semaphoreCreateInfo, 0, &imageAvailableSemaphores[1]);
   vkCreateSemaphore(vk_device, &semaphoreCreateInfo, 0, &renderFinishedSemaphores[0]);
   vkCreateSemaphore(vk_device, &semaphoreCreateInfo, 0, &renderFinishedSemaphores[1]);

   vk::FenceCreateInfo fenceCreateInfo{vk::FenceCreateFlagBits::eSignaled};
   frameFences[0] = vk_device.createFence(fenceCreateInfo);
   frameFences[1] = vk_device.createFence(fenceCreateInfo);

   vk::DescriptorSetLayoutBinding descriptorSetLayoutBindings[] = {{0, vk::DescriptorType::eUniformBuffer, 1,
                                                                    vk::ShaderStageFlagBits::eVertex},
                                                                   {1, vk::DescriptorType::eCombinedImageSampler, 1,
                                                                    vk::ShaderStageFlagBits::eFragment}};
   descriptorSetLayout =
      vk_device.createDescriptorSetLayoutUnique(vk::DescriptorSetLayoutCreateInfo{{}, 2, descriptorSetLayoutBindings});

   shaderStages.push_back(
      vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertexShader->shaderModule(), "main"));
   shaderStages.push_back(
      vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragmentShader->shaderModule(), "main"));

   textShaderStages.push_back(shaderStages[0]); // Use the same vertex shader
   textShaderStages.push_back(
      vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, textFragmentShader->shaderModule(), "main"));

   descriptorPool = device->createDescriptorPool(FRAME_COUNT, vk::DescriptorType::eUniformBuffer, FRAME_COUNT * 2);

   for (int i = 0; i < FRAME_COUNT; ++i) {
      descriptors.push_back(descriptorPool->createDescriptor(*descriptorSetLayout));
   }

   VkDescriptorSetLayout dsl                     = *descriptorSetLayout;
   VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
   pipelineLayoutInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
   pipelineLayoutInfo.setLayoutCount             = 1;
   pipelineLayoutInfo.pSetLayouts                = &dsl;

   auto retval = vkCreatePipelineLayout(vk_device, &pipelineLayoutInfo, nullptr, &pipelineLayout);

   init_pipeline(device, surface, renderPass);

   // initialize models from the resource files
   for (const auto &item : init_vertex_data()) {
      vertexBuffers.emplace(item.first, std::make_pair(device->createBuffer(item.second, vk::BufferUsageFlagBits::eVertexBuffer),
                                                       item.second.size()));
   }

   // initialize text from string literals
   for (const auto &item : init_text_data()) {
      textVertexBuffers.emplace(item.first,
                                std::make_pair(device->createBuffer(item.second, vk::BufferUsageFlagBits::eVertexBuffer),
                                               item.second.size()));
   }

   for (int i = 0; i < FRAME_COUNT; ++i) {
      uniformBuffers.push_back(device->createBuffer(sizeof(Uniform), vk::BufferUsageFlagBits::eUniformBuffer));
      vk::DescriptorBufferInfo bufferInfo{uniformBuffers[i]->getBuffer(), 0, sizeof(Uniform)};
      vk::DescriptorImageInfo imageInfo{sampler, textImage->getImageView(), vk::ImageLayout::eShaderReadOnlyOptimal};

      std::vector<vk::WriteDescriptorSet> descriptorWrites = {{descriptors[i]->descriptorSet(), 0, 0, 1,
                                                               vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo},
                                                              {descriptors[i]->descriptorSet(), 1, 0, 1,
                                                               vk::DescriptorType::eCombinedImageSampler, &imageInfo, nullptr}};
      vk_device.updateDescriptorSets(descriptorWrites, nullptr);
   }
}

static bool framebufferResized = false;

void resize_framebuffer()
{
   framebufferResized = true;
}

void reinitialize_framebuffer(CsPaint::device::handle device, CsPaint::surface::handle surface, CsPaint::renderpass::handle renderpass )
{
   framebufferResized    = false;
   const auto &vk_device = device->graphicsDevice();
   vk_device.waitIdle();
   for (int i = 0; i < FRAME_COUNT; ++i) {
      vkWaitForFences(vk_device, 1, &frameFences[i], VK_TRUE, UINT64_MAX);
   }

   surface->recalculateSize();
   depthBuffer = device->createImage(surface->width(), surface->height(), vk::Format::eD32SfloatS8Uint, vk::ImageTiling::eOptimal,
                                     vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal,
                                     vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);

   std::vector<CsPaint::image::handle> imageHandles;
   imageHandles.push_back(depthBuffer);
   s_swapchain    = surface->createSwapchain(renderpass, imageHandles);
   s_framebuffers = s_swapchain->framebuffers();
   init_pipeline(device, surface, renderpass);
}

void draw_frame(CsPaint::device::handle device, CsPaint::surface::handle surface, CsPaint::renderpass::handle renderPass,
                glm::mat4 transform_matrix, float zoom_factor)
{
   if (framebufferResized == true) {
      reinitialize_framebuffer(device, surface, renderPass);
   }

   uint32_t index = (frameIndex++) % FRAME_COUNT;
   uint32_t imageIndex;
   const auto &vk_device     = device->graphicsDevice();
   const auto &vk_queue      = device->graphicsQueue()->getQueue();
   const auto &vk_swapchain  = *(s_swapchain->chain());
   const auto &vk_renderpass = renderPass->get();
   const auto &vk_buffer     = commandBuffers[index]->buffer();

   vkWaitForFences(vk_device, 1, &frameFences[index], VK_TRUE, UINT64_MAX);
   vkResetFences(vk_device, 1, &frameFences[index]);
   vkAcquireNextImageKHR(vk_device, vk_swapchain, UINT64_MAX, imageAvailableSemaphores[index], VK_NULL_HANDLE, &imageIndex);

   VkCommandBufferBeginInfo beginInfo = {};
   beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
   beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
   vkBeginCommandBuffer(vk_buffer, &beginInfo);

   VkClearValue clearColor[2];
   clearColor[0].color        = {0.9f, 0.9f, 0.9f, 0.0f};
   clearColor[1].depthStencil = {1.0f, 0};

   VkExtent2D swapchainExtent = {surface->width(), surface->height()};
   VkBuffer vertexBufferArray[1];

   VkDeviceSize offsets[]               = {0};
   VkImageSubresourceRange imageRange   = {};
   imageRange.aspectMask                = VK_IMAGE_ASPECT_COLOR_BIT;
   imageRange.levelCount                = 1;
   imageRange.layerCount                = 1;
   VkRenderPassBeginInfo renderPassInfo = {};
   renderPassInfo.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
   renderPassInfo.renderPass            = vk_renderpass;
   renderPassInfo.framebuffer           = s_framebuffers[imageIndex];
   renderPassInfo.renderArea.extent     = swapchainExtent;
   renderPassInfo.clearValueCount       = 2;
   renderPassInfo.pClearValues          = clearColor;

   vkCmdBeginRenderPass(vk_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
   vkCmdBindPipeline(vk_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipeline());

   Uniform uniformData = calculate_uniform(transform_matrix, float(surface->width()) / float(surface->height()), zoom_factor);

   uniformBuffers[index]->store(&uniformData, sizeof(uniformData));

   VkDescriptorSet tmpDescriptor = descriptors[index]->descriptorSet();

   vkCmdBindDescriptorSets(vk_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &tmpDescriptor, 0, nullptr);

   for (const auto &item : vertexBuffers) {
      vertexBufferArray[0] = item.second.first->getBuffer();
      vkCmdBindVertexBuffers(vk_buffer, 0, 1, vertexBufferArray, offsets);
      vkCmdDraw(vk_buffer, uint32_t(item.second.second), 1, 0, item.first);
   }

   vkCmdBindPipeline(vk_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, textPipeline->getPipeline());

   for (const auto &item : textVertexBuffers) {
      vertexBufferArray[0] = item.second.first->getBuffer();
      vkCmdBindVertexBuffers(vk_buffer, 0, 1, vertexBufferArray, offsets);
      vkCmdDraw(vk_buffer, uint32_t(item.second.second), 1, 0, item.first);
   }

   vkCmdEndRenderPass(vk_buffer);

   auto retval = vkEndCommandBuffer(vk_buffer);
   assert(retval == VK_SUCCESS);

   VkPipelineStageFlags tmpMask[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

   VkCommandBuffer tmpBuffer       = vk_buffer;
   VkSubmitInfo submitInfo         = {};
   submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
   submitInfo.waitSemaphoreCount   = 1;
   submitInfo.pWaitSemaphores      = &imageAvailableSemaphores[index];
   submitInfo.pWaitDstStageMask    = tmpMask;
   submitInfo.commandBufferCount   = 1;
   submitInfo.pCommandBuffers      = &tmpBuffer;
   submitInfo.signalSemaphoreCount = 1;
   submitInfo.pSignalSemaphores    = &renderFinishedSemaphores[index];

   VkSwapchainKHR tmpChain        = vk_swapchain;
   VkPresentInfoKHR presentInfo   = {};
   presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
   presentInfo.waitSemaphoreCount = 1;
   presentInfo.pWaitSemaphores    = &renderFinishedSemaphores[index];
   presentInfo.swapchainCount     = 1;
   presentInfo.pSwapchains        = &tmpChain;
   presentInfo.pImageIndices      = &imageIndex;

   vkQueueSubmit(vk_queue, 1, &submitInfo, frameFences[index]);
   if (vkQueuePresentKHR(vk_queue, &presentInfo) == VK_ERROR_OUT_OF_DATE_KHR) {
      framebufferResized = true;
   };
}
