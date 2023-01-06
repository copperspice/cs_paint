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

#ifndef INCLUDED_CSPAINT_DESCRIPTOR_H
#define INCLUDED_CSPAINT_DESCRIPTOR_H

#include <cspaint_fwd.h>

#include <vulkan/vulkan.hpp>

namespace CsPaint
{

class descriptor
{
 public:
   using handle = std::shared_ptr<descriptor>;

   descriptor(CsPaint::descriptor_pool_handle device, vk::UniqueDescriptorSet descriptorSet);

   descriptor(const descriptor &) = delete;
   void operator=(const descriptor &) = delete;

   vk::DescriptorSet descriptorSet() const
   {
      return *m_descriptorSet;
   }

 private:
   CsPaint::descriptor_pool_handle m_pool;
   vk::UniqueDescriptorSet m_descriptorSet;
};
}; // namespace CsPaint

#endif
