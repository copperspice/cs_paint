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

#ifndef INCLUDED_CSPAINT_MEMORYPOOL_H
#define INCLUDED_CSPAINT_MEMORYPOOL_H

#include <cspaint_fwd.h>

#include <vulkan/vulkan.hpp>

namespace CsPaint
{
class memorypool
{
 private:

 public:
   using handle = std::shared_ptr<queue>;

   memorypool(std::shared_ptr<const device> device, uint32_t memoryType);

   memorypool(const memorypool &) = delete;
   void operator=(const memorypool &) = delete;

 private:
   std::shared_ptr<const device> m_device;
   uint32_t m_memoryType;
};
};

#endif
