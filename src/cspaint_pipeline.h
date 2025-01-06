/***********************************************************************
*
* Copyright (c) 2019-2025 Barbara Geller
* Copyright (c) 2019-2025 Ansel Sermersheim
*
* This file is part of CsPaint.
*
* CsPaint is free software which is released under the BSD 2-Clause license.
* For license details refer to the LICENSE provided with this project.
*
* CsPaint is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://opensource.org/licenses/BSD-2-Clause
*
***********************************************************************/

#ifndef INCLUDED_CSPAINT_PIPELINE_H
#define INCLUDED_CSPAINT_PIPELINE_H

#include <cspaint_fwd.h>

#include <vulkan/vulkan.hpp>

namespace CsPaint
{
class pipeline
{
 private:
 public:
   using handle = std::shared_ptr<pipeline>;

   pipeline(device_handle device, const surface_handle &surface, const renderpass_handle renderPass,
            const vk::PipelineVertexInputStateCreateInfo &vertexInputInfo,
            const std::vector<vk::PipelineShaderStageCreateInfo> &shaderStages, const vk::PipelineLayout &pipelineLayout);

   pipeline(const pipeline &) = delete;
   void operator=(const pipeline &) = delete;

   vk::Pipeline getPipeline()
   {
      return *m_pipeline;
   }

 private:
   device_handle m_device;
   vk::UniquePipeline m_pipeline;
};
}; // namespace CsPaint

#endif
