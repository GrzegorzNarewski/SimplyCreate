#include "Window.h"

namespace gll
{
    struct QueueInfo
    {
        uint32_t           FamilyIndex;
        std::vector<float> Priorities;
    };
    struct QueueParameters
    {
        VkQueue   Handle;
        uint32_t  FamilyIndex;
    };
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    class Devices
    {
        bool EnumerateAvailablePhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice> & available_devices);
        bool CheckAvailableDeviceExtensions(VkPhysicalDevice physical_device, std::vector<VkExtensionProperties>& available_extensions);
        bool CheckAvailableQueueFamiliesAndTheirProperties(VkPhysicalDevice physical_device, std::vector<VkQueueFamilyProperties>& queue_families);
        bool SelectQueueFamilyThatSupportsPresentationToGivenSurface(VkPhysicalDevice physical_device, VkSurfaceKHR presentation_surface, uint32_t& queue_family_index);
        bool SelectIndexOfQueueFamilyWithDesiredCapabilities(VkPhysicalDevice physical_device, VkQueueFlags desired_capabilities, uint32_t & queue_family_index);
        bool makeLogicalDevice(VkPhysicalDevice physical_device, std::vector<QueueInfo> queue_infos, std::vector<char const *> const & desired_extensions, VkPhysicalDeviceFeatures desired_features, VkDevice& logical_device);
        VkSampleCountFlagBits getMaxUsableSampleCount();
    public:
        VkPhysicalDevice PDev;
        VkDevice         LDev;

        std::vector<char const *> desired_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        VkPhysicalDeviceFeatures desired_features;

        QueueParameters GraphicsQueue;
        QueueParameters ComputeQueue;
        QueueParameters PresentQueue;

        void createLogicalDevice(Instance &instance, Window &glfwObject);
        void deleteLogicalDevice();

        static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, Window &glfwObject);
    };

    VkCommandBuffer beginSingleTimeCommands(Devices &device, VkCommandPool &commandpool);
    void endSingleTimeCommands(Devices &device, VkCommandPool &commandpool, VkCommandBuffer commandBuffer);
};

