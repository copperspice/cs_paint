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

#ifndef INCLUDED_CSPAINT_SHADER_H
#define INCLUDED_CSPAINT_SHADER_H

#include <cspaint_fwd.h>

#include <vulkan/vulkan.hpp>

namespace CsPaint
{
class shader
{
 private:
   struct private_tag {
   };

 public:
   using handle = std::shared_ptr<shader>;

   shader(std::shared_ptr<const CsPaint::device> device, std::vector<char> shader_code);
   shader(std::shared_ptr<const CsPaint::device> device, std::vector<uint32_t> shader_code);

   ~shader()
   {
   }

   shader(const shader &) = delete;
   void operator=(const shader &) = delete;

  const vk::ShaderModule& shaderModule() {
    return *m_shader;
  }

 private:
   std::shared_ptr<const CsPaint::device > m_device;
   vk::UniqueShaderModule m_shader;
};
};

#endif
