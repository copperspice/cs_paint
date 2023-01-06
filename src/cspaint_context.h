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

#ifndef INCLUDED_CSPAINT_CONTEXT_H
#define INCLUDED_CSPAINT_CONTEXT_H

#include <cspaint_fwd.h>

#include <any>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace CsPaint
{

class context
{
 private:
   struct private_tag {
   };

 public:
   using handle = std::shared_ptr<context>;

   context(vk::Instance instance);

   ~context()
   {
      m_instance.destroy();
   }

   context(const context &) = delete;
   void operator=(const context &) = delete;

   static handle create(const std::string &app_name, uint32_t app_version = 0,
                        const std::vector<std::string> &extensions = std::vector<std::string>(),
                        const std::vector<std::string> &layers     = std::vector<std::string>());

   static handle create(const char *app_name, uint32_t app_version = 0,
                        const std::vector<const char *> &extensions = std::vector<const char *>(),
                        const std::vector<const char *> &layers     = std::vector<const char *>());

   static handle create(vk::Instance instance);

   vk::Instance instance() const
   {
      return m_instance;
   }

   using error_callback_t = void(vk::DebugReportFlagsEXT, vk::DebugReportObjectTypeEXT, uint64_t, size_t, int32_t, const char *,
                                 const char *);

   void setDebugCallback(std::function<error_callback_t> callback,
                         vk::DebugReportFlagsEXT flags = vk::DebugReportFlagBitsEXT::eError |
                                                         vk::DebugReportFlagBitsEXT::eWarning);

 private:
   vk::Instance m_instance;
   vk::DispatchLoaderDynamic m_dynamicLoader;
   std::function<error_callback_t> m_errorCallback;
   vk::UniqueHandle<vk::DebugReportCallbackEXT, vk::DispatchLoaderDynamic> m_debugReport;
};
}; // namespace CsPaint

#endif
