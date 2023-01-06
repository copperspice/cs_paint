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

#include <cspaint_commandbuffer.h>

#include <cspaint_util.h>
#include <cspaint_commandpool.h>

static_assert(CsPaint::util::nomove_nocopy_nodefault<CsPaint::command_buffer>());

CsPaint::command_buffer::command_buffer(vk::UniqueCommandBuffer buffer)
  : m_buffer(std::move(buffer))
{
}
