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

#ifndef INCLUDED_CSPAINT_DESCRIPTORPOOL_H
#define INCLUDED_CSPAINT_DESCRIPTORPOOL_H

#include <cspaint_fwd.h>

#include <vulkan/vulkan.hpp>

namespace CsPaint
{

  class descriptor_pool: public std::enable_shared_from_this<descriptor_pool>
{
 public:
   using handle = std::shared_ptr<descriptor_pool>;

   descriptor_pool(CsPaint::device_handle device, vk::UniqueDescriptorPool pool);

   descriptor_pool(const descriptor_pool &) = delete;
   void operator=(const descriptor_pool &) = delete;

   descriptor_handle createDescriptor(vk::DescriptorSetLayout layout);

 private:
   CsPaint::device_handle m_device;
   vk::UniqueDescriptorPool m_pool;
};
}; // namespace CsPaint

#endif
