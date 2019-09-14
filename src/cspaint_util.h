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

#ifndef INCLUDED_CSPAINT_UTIL_H
#define INCLUDED_CSPAINT_UTIL_H
#include <algorithm>
#include <functional>
#include <type_traits>
#include <vector>

namespace CsPaint
{

namespace util
{
template <typename T>
class make_shared_with_init
{
 public:
   template <typename... Args>
   static std::shared_ptr<T> create(Args &&... args)
   {
      auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
      ptr->init(ptr);
      return ptr;
   }

 private:
   friend T;
   struct private_tag {
   };
};

template <typename T, typename Func>
//  std::vector<decltype(std::declval<Func>()(std::declval<T>()))> map_vector(const std::vector<T>
//  &input, Func f)
std::vector<std::invoke_result_t<Func, T>> map_vector(const std::vector<T> &input, Func f)
{
   std::vector<std::invoke_result_t<Func, T>> newData;
   newData.reserve(input.size());
   for (auto &item : input) {
      newData.push_back(f(item));
   }

   return newData;
}

template <typename T, typename Func>
std::vector<T> filter_vector(std::vector<T> input, Func f)
{
   input.erase(std::remove_if(input.begin(), input.end(), [&f](const T &data) { return !f(data); }),
               input.end());
   return input;
}

template <typename T>
constexpr bool nomove_nocopy_nodefault()
{
   return !std::is_move_constructible_v<T> && !std::is_copy_constructible_v<T> &&
          !std::is_move_assignable_v<T> && !std::is_copy_assignable_v<T> &&
          !std::is_constructible_v<T>;
}
};
};
#endif
