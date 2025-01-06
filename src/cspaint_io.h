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

#ifndef INCLUDED_CSPAINT_IO_H
#define INCLUDED_CSPAINT_IO_H

#include <cspaint_fwd.h>

#include <vector>
#include <string>

namespace CsPaint
{
  namespace io{
    struct Vertex {
      float x, y, z;
      float nx, ny, nz;
    };


    std::vector<Vertex> parse_stl(const char * data, std::size_t size);
    std::vector<Vertex> load_stl(std::string filename);
  };

}; // namespace CsPaint

#endif

