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

#include <cspaint_descriptorpool.h>

#include <cspaint_descriptor.h>
#include <cspaint_device.h>
#include <cspaint_util.h>

static_assert(CsPaint::util::nomove_nocopy_nodefault<CsPaint::descriptor_pool>());

CsPaint::descriptor_pool::descriptor_pool(CsPaint::device::handle device, vk::UniqueDescriptorPool pool)
   : m_device(device), m_pool(std::move(pool))
{
}

CsPaint::descriptor::handle CsPaint::descriptor_pool::createDescriptor(vk::DescriptorSetLayout layout)
{
   auto vector = m_device->graphicsDevice().allocateDescriptorSetsUnique(vk::DescriptorSetAllocateInfo(*m_pool, 1, &layout));
   return std::make_shared<descriptor>(shared_from_this(), std::move(vector[0]));
}
