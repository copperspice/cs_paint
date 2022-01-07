/***********************************************************************
*
* Copyright (c) 2019-2022 Barbara Geller
* Copyright (c) 2019-2022 Ansel Sermersheim
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

#ifndef INCLUDED_CSPAINT_COMMANDBUFFER_H
#define INCLUDED_CSPAINT_COMMANDBUFFER_H

#include <cspaint_fwd.h>

#include <vulkan/vulkan.hpp>

namespace CsPaint
{

class command_buffer
{
 public:
   using handle = std::shared_ptr<command_buffer>;

   command_buffer(vk::UniqueCommandBuffer buffer);

   command_buffer(const command_buffer &) = delete;
   void operator=(const command_buffer &) = delete;

   const vk::CommandBuffer &buffer() const
   {
      return *m_buffer;
   };

 private:
   vk::UniqueCommandBuffer m_buffer;
};
};

#endif
