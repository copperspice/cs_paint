/***********************************************************************
*
* Copyright (c) 2019-2021 Barbara Geller
* Copyright (c) 2019-2021 Ansel Sermersheim
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

#ifndef INCLUDED_CSPAINT_IMAGE_H
#define INCLUDED_CSPAINT_IMAGE_H

#include <cspaint_fwd.h>

#include <vulkan/vulkan.hpp>

namespace CsPaint
{
class image
{
 private:
 public:
   using handle = std::shared_ptr<image>;

   image(device_handle, vk::UniqueImage, vk::Format imageFormat, vk::ImageAspectFlags aspectFlags);

   image(const image &) = delete;
   void operator=(const image &) = delete;

   vk::Image getImage()
   {
      return *m_image;
   }

   vk::ImageView getImageView()
   {
      return *m_view;
   }

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
   vk::UniqueImage m_image;
   vk::UniqueDeviceMemory m_memory;
   vk::UniqueImageView m_view;
};
}; // namespace CsPaint

#endif
