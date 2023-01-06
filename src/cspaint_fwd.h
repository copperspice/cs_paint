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

#ifndef INCLUDED_CSPAINT_FWD_H
#define INCLUDED_CSPAINT_FWD_H

#include <memory>

namespace CsPaint
{
  class buffer;
  using buffer_handle = std::shared_ptr<buffer>;

  class command_buffer;
  using command_buffer_handle = std::shared_ptr<command_buffer>;

  class command_pool;
  using command_pool_handle = std::shared_ptr<command_pool>;

  class context;
  using context_handle = std::shared_ptr<context>;

  class descriptor;
  using descriptor_handle = std::shared_ptr<descriptor>;

  class descriptor_pool;
  using descriptor_pool_handle = std::shared_ptr<descriptor_pool>;

  class device;
  using device_handle = std::shared_ptr<device>;

  class image;
  using image_handle = std::shared_ptr<image>;

  class memorypool;
  using memorypool_handle = std::shared_ptr<memorypool>;

  class pipeline;
  using pipeline_handle = std::shared_ptr<pipeline>;

  class queue;
  using queue_handle = std::shared_ptr<queue>;

  class renderpass;
  using renderpass_handle = std::shared_ptr<renderpass>;

  class shader;
  using shader_handle = std::shared_ptr<shader>;

  class surface;
  using surface_handle = std::shared_ptr<surface>;

  class swapchain;
  using swapchain_handle = std::shared_ptr<swapchain>;

  class texture;
  using texture_handle = std::shared_ptr<texture>;

};

#endif
