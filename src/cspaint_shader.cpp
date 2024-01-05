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

#include <cspaint_shader.h>

#include <cspaint_device.h>
#include <cspaint_util.h>

static_assert(CsPaint::util::nomove_nocopy_nodefault<CsPaint::shader>());

#include <iostream>
template <typename T>
void roundup_container(T &data, typename T::size_type multiple)
{
   auto size = data.size();
   std::cout << "Starting size " << size << std::endl;

   auto newSize = size;
   while (newSize % multiple != 0) {
      ++newSize;
   }

   std::cout << "New size " << newSize << std::endl;

   data.resize(newSize);
}

CsPaint::shader::shader(std::shared_ptr<const CsPaint::device> device, std::vector<char> input) : m_device(device)
{
   roundup_container(input, 4);

   m_shader = m_device->graphicsDevice().createShaderModuleUnique(
      vk::ShaderModuleCreateInfo({}, input.size(), reinterpret_cast<const uint32_t *>(input.data())));
}

CsPaint::shader::shader(std::shared_ptr<const CsPaint::device> device, std::vector<uint32_t> input) : m_device(device)
{
   m_shader = m_device->graphicsDevice().createShaderModuleUnique(
      vk::ShaderModuleCreateInfo({}, input.size() * sizeof(uint32_t), input.data()));
}
