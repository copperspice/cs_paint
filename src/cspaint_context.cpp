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

#include <cspaint_context.h>

#include <cspaint_util.h>

#include <algorithm>
#include <functional>
#include <vulkan/vulkan.hpp>

static_assert(CsPaint::util::nomove_nocopy_nodefault<CsPaint::context>());

CsPaint::context::context(vk::Instance instance)
   : m_instance(instance),
     m_dynamicLoader(m_instance, vkGetInstanceProcAddr),
     m_debugReport(vk::DebugReportCallbackEXT(),
                   vk::ObjectDestroy<vk::Instance, vk::DispatchLoaderDynamic>(vk::Instance(),
                                                                              vk::Optional<const vk::AllocationCallbacks>(
                                                                                 nullptr),
                                                                              vk::DispatchLoaderDynamic()))
{
}

CsPaint::context::handle CsPaint::context::create(const std::string &app_name, uint32_t app_version,
                                                  const std::vector<std::string> &extensions,
                                                  const std::vector<std::string> &layers)
{
   using namespace util;

   auto extension_ptrs = map_vector(extensions, [](const std::string &s) { return s.c_str(); });
   auto layer_ptrs     = map_vector(layers, [](const std::string &s) { return s.c_str(); });

   return create(app_name.c_str(), app_version, extension_ptrs, layer_ptrs);
}

CsPaint::context::handle CsPaint::context::create(const char *app_name, uint32_t app_version,
                                                  const std::vector<const char *> &extensions,
                                                  const std::vector<const char *> &layers)
{
   vk::ApplicationInfo applicationInfo = {app_name, app_version, "CsPaint", 0, VK_API_VERSION_1_1};

   handle retval;

   try {
      std::vector<const char *> tmp_extensions = extensions;
      tmp_extensions.push_back("VK_EXT_debug_report");

      vk::InstanceCreateInfo createInfo({}, &applicationInfo, static_cast<uint32_t>(layers.size()), layers.data(),
                                        static_cast<uint32_t>(tmp_extensions.size()), tmp_extensions.data());

      retval = std::make_shared<context>(vk::createInstance(createInfo));

      retval->setDebugCallback([](vk::DebugReportFlagsEXT flags, vk::DebugReportObjectTypeEXT objectType, uint64_t object,
                                  size_t location, int32_t messageCode, const char *pLayerPrefix,
                                  const char *pMessage) { fprintf(stderr, "VULKAN MESSAGE: %s\n\n", pMessage); });

   } catch (...) {
   }

   if (retval == nullptr) {
      vk::InstanceCreateInfo createInfo({}, &applicationInfo, static_cast<uint32_t>(layers.size()), layers.data(),
                                        static_cast<uint32_t>(extensions.size()), extensions.data());

      retval = std::make_shared<context>(vk::createInstance(createInfo));
   }

   return retval;
}

void CsPaint::context::setDebugCallback(std::function<error_callback_t> callback, vk::DebugReportFlagsEXT flags)
{
   m_errorCallback = std::move(callback);

   vk::DebugReportCallbackCreateInfoEXT info{flags,
                                             [](VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
                                                uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix,
                                                const char *pMessage, void *pUserData) -> VkBool32 {
                                                context *c = static_cast<context *>(pUserData);
                                                c->m_errorCallback(vk::DebugReportFlagsEXT(flags),
                                                                   vk::DebugReportObjectTypeEXT(objectType), object, location,
                                                                   messageCode, pLayerPrefix, pMessage);
                                                return VK_FALSE;
                                             },
                                             this};

   m_debugReport = m_instance.createDebugReportCallbackEXTUnique(info, nullptr, m_dynamicLoader);
}
