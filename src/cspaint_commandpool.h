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

#ifndef INCLUDED_CSPAINT_COMMANDPOOL_H
#define INCLUDED_CSPAINT_COMMANDPOOL_H

#include <cspaint_fwd.h>

#include <vulkan/vulkan.hpp>

namespace CsPaint
{

class command_pool
{
 public:
   using handle = std::shared_ptr<command_pool>;

   command_pool(const vk::Device &device, uint32_t queueFamily);

   command_pool(const command_pool &) = delete;
   void operator=(const command_pool &) = delete;

   //   const vk::UniqueCommandPool &pool() const;

   command_buffer_handle createCommandBuffer();

 private:
   const vk::Device &m_device;
   vk::UniqueCommandPool m_pool;
};
};

#endif

