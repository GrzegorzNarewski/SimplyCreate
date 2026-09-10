#include "VertexBuffer.h"

namespace gll
{
    const unsigned int MAX_FRAMES_IN_FLIGHT = 2;

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class Swapchain
    {
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(Window &glfwObject, const VkSurfaceCapabilitiesKHR& capabilities);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, Window &glfwObject);
    public:
        VkSwapchainKHR swapChain;
        std::vector<VkImage> Image;
        std::vector<VkImageView> ImageViews;
        VkFormat Format;
        VkExtent2D Extent;

        const VkSurfaceFormatKHR desiredFormat = {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        const VkPresentModeKHR desiredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

        void createSwapchain(Devices &devices, Window &glfwObject);
        void deleteSwapchain(Devices &devices);


    };
}
