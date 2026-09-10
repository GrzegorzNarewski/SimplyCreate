#include "Devices.h"

namespace gll
{
    void Devices::createLogicalDevice(Instance &instance, Window &glfwObject)
    {
        std::vector<VkPhysicalDevice> physical_devices;
        EnumerateAvailablePhysicalDevices(instance.instance, physical_devices);

        for( auto & physical_device : physical_devices )
        {
            if( !SelectIndexOfQueueFamilyWithDesiredCapabilities( physical_device, VK_QUEUE_GRAPHICS_BIT, GraphicsQueue.FamilyIndex ) ) {
                continue;
            }

            if( !SelectIndexOfQueueFamilyWithDesiredCapabilities( physical_device, VK_QUEUE_COMPUTE_BIT, ComputeQueue.FamilyIndex ) ) {
                continue;
            }

            if( !SelectQueueFamilyThatSupportsPresentationToGivenSurface( physical_device, glfwObject.surface, PresentQueue.FamilyIndex ) ) {
                continue;
            }

            std::vector<QueueInfo> requested_queues = { { GraphicsQueue.FamilyIndex, { 1.0f } } };
            if( GraphicsQueue.FamilyIndex != ComputeQueue.FamilyIndex )
                requested_queues.push_back( { ComputeQueue.FamilyIndex,{ 1.0f } } );
            if( (GraphicsQueue.FamilyIndex != PresentQueue.FamilyIndex) &&
              (ComputeQueue.FamilyIndex != PresentQueue.FamilyIndex) )
                requested_queues.push_back( { PresentQueue.FamilyIndex, { 1.0f } } );
            std::vector<char const *> device_extensions;
            desired_features = {};
            desired_features.samplerAnisotropy = VK_TRUE;
            desired_features.geometryShader = VK_TRUE;

            if(!makeLogicalDevice( physical_device, requested_queues, desired_extensions, desired_features, LDev ) ) continue;
            else {
                PDev = physical_device;
                vkGetDeviceQueue( LDev, GraphicsQueue.FamilyIndex, 0, &GraphicsQueue.Handle );
                vkGetDeviceQueue( LDev, GraphicsQueue.FamilyIndex, 0, &ComputeQueue.Handle );
                vkGetDeviceQueue( LDev, PresentQueue.FamilyIndex, 0, &PresentQueue.Handle );
            break;
            }
        }
        msaaSamples = getMaxUsableSampleCount();
    }
    void Devices::deleteLogicalDevice()
    {
        if( LDev ) {
          vkDestroyDevice( LDev, nullptr );
        }
    }

    bool Devices::CheckAvailableQueueFamiliesAndTheirProperties(VkPhysicalDevice physical_device, std::vector<VkQueueFamilyProperties>& queue_families)
    {
        uint32_t queue_families_count = 0;

        vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &queue_families_count, nullptr );
        if( queue_families_count == 0 ) {
          std::cout << "Could not get the number of queue families." << std::endl;
          return false;
        }

        queue_families.resize( queue_families_count );
        vkGetPhysicalDeviceQueueFamilyProperties( physical_device, &queue_families_count, queue_families.data() );
        if( queue_families_count == 0 ) {
          std::cout << "Could not acquire properties of queue families." << std::endl;
          return false;
        }

        return true;
    }
    bool Devices::CheckAvailableDeviceExtensions(VkPhysicalDevice physical_device, std::vector<VkExtensionProperties>& available_extensions)
    {
        uint32_t extensions_count = 0;
        VkResult result = VK_SUCCESS;

        result = vkEnumerateDeviceExtensionProperties( physical_device, nullptr, &extensions_count, nullptr );
        if( (result != VK_SUCCESS) ||
            (extensions_count == 0) ) {
          std::cout << "Could not get the number of device extensions." << std::endl;
          return false;
        }

        available_extensions.resize( extensions_count );
        result = vkEnumerateDeviceExtensionProperties( physical_device, nullptr, &extensions_count, available_extensions.data() );
        if( (result != VK_SUCCESS) ||
            (extensions_count == 0) ) {
          std::cout << "Could not enumerate device extensions." << std::endl;
          return false;
        }

        return true;
    }
    bool Devices::EnumerateAvailablePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice> & available_devices)
    {
        uint32_t devices_count = 0;
        VkResult result = VK_SUCCESS;

        result = vkEnumeratePhysicalDevices( instance, &devices_count, nullptr );
        if( (result != VK_SUCCESS) ||
            (devices_count == 0) ) {
          std::cout << "Could not get the number of available physical devices." << std::endl;
          return false;
        }

        available_devices.resize( devices_count );
        result = vkEnumeratePhysicalDevices( instance, &devices_count, available_devices.data() );
        if( (result != VK_SUCCESS) ||
            (devices_count == 0) ) {
          std::cout << "Could not enumerate physical devices." << std::endl;
          return false;
        }

        return true;
    }
    bool Devices::SelectQueueFamilyThatSupportsPresentationToGivenSurface(VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface, uint32_t& queue_family_index)
    {
        std::vector<VkQueueFamilyProperties> queue_families;
        if( !CheckAvailableQueueFamiliesAndTheirProperties( physical_device, queue_families ) ) {
          return false;
        }

        for( uint32_t index = 0; index < static_cast<uint32_t>(queue_families.size()); ++index ) {
          VkBool32 presentation_supported = VK_FALSE;
          VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR( physical_device, index, presentation_surface, &presentation_supported );
          if( (VK_SUCCESS == result) &&
              (VK_TRUE == presentation_supported) ) {
            queue_family_index = index;
            return true;
          }
        }
        return false;
    }
    bool Devices::SelectIndexOfQueueFamilyWithDesiredCapabilities(VkPhysicalDevice physical_device, VkQueueFlags desired_capabilities, uint32_t & queue_family_index)
    {
        std::vector<VkQueueFamilyProperties> queue_families;
        if( !CheckAvailableQueueFamiliesAndTheirProperties( physical_device, queue_families ) ) {
          return false;
        }

        for( uint32_t index = 0; index < static_cast<uint32_t>(queue_families.size()); ++index ) {
          if( (queue_families[index].queueCount > 0) &&
              ((queue_families[index].queueFlags & desired_capabilities) == desired_capabilities) ) {
            queue_family_index = index;
            return true;
          }
        }
        return false;
    }
    bool IsExtensionSupported( std::vector<VkExtensionProperties> const & available_extensions, char const * const extension )
    {
        for( auto & available_extension : available_extensions ) {
          if( strstr( available_extension.extensionName, extension ) ) {
            return true;
          }
        }
        return false;
    }
    bool Devices::makeLogicalDevice(VkPhysicalDevice physical_device, std::vector<QueueInfo> queue_infos, std::vector<char const *> const & desired_extensions, VkPhysicalDeviceFeatures desired_features, VkDevice& logical_device)
    {
        std::vector<VkExtensionProperties> available_extensions;
        if( !CheckAvailableDeviceExtensions( physical_device, available_extensions ) ) {
          return false;
        }

        for( auto & extension : desired_extensions ) {
          if( !IsExtensionSupported( available_extensions, extension ) ) {
            std::cout << "Extension named '" << extension << "' is not supported by a physical device." << std::endl;
            return false;
          }
        }

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

        for( auto & info : queue_infos ) {
          queue_create_infos.push_back( {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,       // VkStructureType                  sType
            nullptr,                                          // const void                     * pNext
            0,                                                // VkDeviceQueueCreateFlags         flags
            info.FamilyIndex,                                 // uint32_t                         queueFamilyIndex
            static_cast<uint32_t>(info.Priorities.size()),    // uint32_t                         queueCount
            info.Priorities.data()                            // const float                    * pQueuePriorities
          } );
        };

        VkDeviceCreateInfo device_create_info = {
          VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,               // VkStructureType                  sType
          nullptr,                                            // const void                     * pNext
          0,                                                  // VkDeviceCreateFlags              flags
          static_cast<uint32_t>(queue_create_infos.size()),   // uint32_t                         queueCreateInfoCount
          queue_create_infos.data(),                          // const VkDeviceQueueCreateInfo  * pQueueCreateInfos
          0,                                                  // uint32_t                         enabledLayerCount
          nullptr,                                            // const char * const             * ppEnabledLayerNames
          static_cast<uint32_t>(desired_extensions.size()),   // uint32_t                         enabledExtensionCount
          desired_extensions.data(),                          // const char * const             * ppEnabledExtensionNames
          &desired_features                                    // const VkPhysicalDeviceFeatures * pEnabledFeatures
        };

        VkResult result = vkCreateDevice( physical_device, &device_create_info, nullptr, &logical_device );
        if( (result != VK_SUCCESS) ||
            (logical_device == VK_NULL_HANDLE) ) {
          std::cout << "Could not create logical device." << std::endl;
          return false;
        }

        return true;
    }
    VkSampleCountFlagBits Devices::getMaxUsableSampleCount()
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(PDev, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }
    QueueFamilyIndices Devices::findQueueFamilies(VkPhysicalDevice device, Window &glfwObject)
    {
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphicsFamily = i;
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, glfwObject.surface, &presentSupport);
            if (presentSupport)
                indices.presentFamily = i;
            if (indices.isComplete())
                break;
            i++;
        }
        return indices;
    }

    VkCommandBuffer beginSingleTimeCommands(Devices &device, VkCommandPool &commandpool)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandpool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device.LDev, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void endSingleTimeCommands(Devices &device, VkCommandPool &commandpool, VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(device.GraphicsQueue.Handle, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(device.GraphicsQueue.Handle);

        vkFreeCommandBuffers(device.LDev, commandpool, 1, &commandBuffer);
    }
}
