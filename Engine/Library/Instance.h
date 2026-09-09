#include "Helper.h"

namespace gll
{
    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    const float Pi = 3.1415926536f;

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    class Instance
    {
        bool checkValidationLayerSupport();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void setupDebugMessenger();
        std::vector<const char*> getRequiredExtensions();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    public:
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;

        const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation",
        };

        void createInstance();
        void deleteInstance();
    };
}
