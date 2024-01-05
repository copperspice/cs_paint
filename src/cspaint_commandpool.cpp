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

#include <cspaint_commandpool.h>

#include <cspaint_commandbuffer.h>
#include <cspaint_util.h>

static_assert(CsPaint::util::nomove_nocopy_nodefault<CsPaint::command_pool>());

CsPaint::command_pool::command_pool(const vk::Device &device, uint32_t queueFamily)
    : m_device(device)
{
   m_pool = m_device.createCommandPoolUnique(
       vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queueFamily));
}

CsPaint::command_buffer_handle CsPaint::command_pool::createCommandBuffer()
{
   auto vector = m_device.allocateCommandBuffersUnique(
       vk::CommandBufferAllocateInfo(*m_pool, vk::CommandBufferLevel::ePrimary, 1));
   return std::make_shared<command_buffer>(std::move(vector[0]));
}
