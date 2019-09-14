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

#ifndef INCLUDED_CSPAINT_BUFFER_H
#define INCLUDED_CSPAINT_BUFFER_H

#include <cspaint_fwd.h>

#include <vulkan/vulkan.hpp>

namespace CsPaint
{
class buffer
{
 private:
 public:
   using handle = std::shared_ptr<buffer>;

   buffer(device_handle, vk::UniqueBuffer);

   buffer(const buffer &) = delete;
   void operator=(const buffer &) = delete;

   vk::Buffer getBuffer()
   {
      return *m_buffer;
   };

   vk::DeviceMemory getDeviceMemory()
   {
      return *m_memory;
   };

   template <typename T, typename Alloc>
   void store(const std::vector<T, Alloc> &vec)
   {
      store(static_cast<const void *>(vec.data()), vec.size() * sizeof(T));
   };

   void store(const void *data, uint64_t size);

 private:
   device_handle m_device;
   vk::UniqueBuffer m_buffer;
   vk::UniqueDeviceMemory m_memory;
};
}; // namespace CsPaint

#endif
