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

#include <cspaint_pipeline.h>

#include <cspaint_device.h>
#include <cspaint_renderpass.h>
#include <cspaint_surface.h>
#include <cspaint_util.h>

#include <stdexcept>

static_assert(CsPaint::util::nomove_nocopy_nodefault<CsPaint::pipeline>());

CsPaint::pipeline::pipeline(device_handle device, const surface_handle &surface, const renderpass_handle renderPass,
                            const vk::PipelineVertexInputStateCreateInfo &vertexInputInfo,
                            const std::vector<vk::PipelineShaderStageCreateInfo> &shaderStages,
                            const vk::PipelineLayout &pipelineLayout)
   : m_device(std::move(device))
{
   const float width  = surface->width();
   const float height = surface->height();

   vk::Viewport viewport{0.0f, 0.0f, width, height, 0.0f, 1.0f};

   vk::PipelineRasterizationStateCreateInfo rasterizationState{{},
                                                               VK_FALSE,
                                                               VK_FALSE,
                                                               vk::PolygonMode::eFill,
                                                               vk::CullModeFlagBits::eNone,
                                                               vk::FrontFace::eClockwise,
                                                               VK_FALSE,
                                                               0.0f,
                                                               0.0f,
                                                               0.0f,
                                                               1.0f};

   vk::Rect2D scissor{{}, {uint32_t(width), uint32_t(height)}};

   vk::PipelineInputAssemblyStateCreateInfo inputAssembly{{}, vk::PrimitiveTopology::eTriangleList, VK_FALSE};

   vk::PipelineViewportStateCreateInfo viewportState{{}, 1, &viewport, 1, &scissor};

   vk::PipelineMultisampleStateCreateInfo multisampleState{{}, vk::SampleCountFlagBits::e1};

   vk::PipelineColorBlendAttachmentState blendAttachmentState{VK_TRUE,
                                                              vk::BlendFactor::eSrcAlpha,
                                                              vk::BlendFactor::eOneMinusSrcAlpha,
                                                              vk::BlendOp::eAdd,
                                                              vk::BlendFactor::eSrcAlpha,
                                                              vk::BlendFactor::eOneMinusSrcAlpha,
                                                              vk::BlendOp::eAdd,
                                                              vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                                 vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};

   vk::PipelineColorBlendStateCreateInfo colorBlendState{{},
                                                         VK_FALSE,
                                                         vk::LogicOp::eNoOp,
                                                         1,
                                                         &blendAttachmentState,
                                                         {1.0f, 1.0f, 1.0f, 1.0f}};

   vk::PipelineDepthStencilStateCreateInfo depthStencilState{{}, VK_TRUE, VK_TRUE, vk::CompareOp::eLess};

   vk::GraphicsPipelineCreateInfo createInfo{{},
                                             2,
                                             shaderStages.data(),
                                             &vertexInputInfo,
                                             &inputAssembly,
                                             nullptr,
                                             &viewportState,
                                             &rasterizationState,
                                             &multisampleState,
                                             &depthStencilState,
                                             &colorBlendState,
                                             nullptr,
                                             pipelineLayout,
                                             renderPass->get()};
   auto retval = m_device->graphicsDevice().createGraphicsPipelinesUnique(vk::PipelineCache(), createInfo);
   m_pipeline  = std::move(retval.value[0]);
}
