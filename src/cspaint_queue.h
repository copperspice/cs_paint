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

#ifndef INCLUDED_CSPAINT_QUEUE_H
#define INCLUDED_CSPAINT_QUEUE_H

#include <cspaint_fwd.h>

#include <vulkan/vulkan.hpp>

namespace CsPaint
{
class queue
{
 private:
   struct private_tag {
   };

 public:
   using handle = std::shared_ptr<queue>;

   queue(std::shared_ptr<const device> device, vk::Queue vk_queue);

   queue(const queue &) = delete;
   void operator=(const queue &) = delete;

   vk::Queue getQueue() const
   {
      return m_vk_queue;
   };

 private:
   std::shared_ptr<const device> m_device;
   vk::Queue m_vk_queue;
};
};

#endif
