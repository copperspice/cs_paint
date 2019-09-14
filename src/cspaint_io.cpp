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

#include <cspaint_io.h>

#include <fstream>
#include <iostream>
#include <regex>

#include <glm/vec3.hpp>

std::vector<CsPaint::io::Vertex> CsPaint::io::load_stl(std::string filename)
{
   std::vector<Vertex> retval;
   std::ifstream input(filename);
   std::regex r("(facet normal|vertex) (-?\\d+\\.\\d+) (-?\\d+\\.\\d+) (-?\\d+\\.\\d+)\\s*");
   std::string line;

   if (input.fail()) {
      throw std::runtime_error("CsPaint::io::load_stl(): Unable to open input file " + filename);
   }

   Vertex tmp = {};

   while (getline(input, line, '\n')) {
      std::smatch results;

      if (regex_match(line, results, r)) {
         std::string tag = results[1];
         if (tag == "vertex") {
            tmp.x = std::stof(results[2]);
            tmp.y = std::stof(results[3]);
            tmp.z = std::stof(results[4]);
            retval.push_back(tmp);

         } else if (tag == "facet normal") {
            tmp.nx = std::stof(results[2]);
            tmp.ny = std::stof(results[3]);
            tmp.nz = std::stof(results[4]);
         }
      }
   }

   return retval;
}
