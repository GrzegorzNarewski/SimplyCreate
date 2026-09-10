#include "Shader.h"

namespace gll
{
    class SemFence
    {
    public:
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        void createSemFences(Devices &devices, int amount);
        void deleteSemFences(Devices &devices);

        VkResult waitForFences(Devices &devices, VkCommandBuffer &commandbuffer, Swapchain &swapchain, uint32_t &imageIndex, uint32_t currentFrame, bool aquire = true);
    };
}
