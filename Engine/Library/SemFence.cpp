#include "SemFence.h"

namespace gll
{
    void SemFence::createSemFences(Devices &devices, int amount)
    {
        imageAvailableSemaphores.resize(amount);
        renderFinishedSemaphores.resize(amount);
        inFlightFences.resize(amount);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < amount; i++) {
            if (vkCreateSemaphore(devices.LDev, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(devices.LDev, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(devices.LDev, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }
    void SemFence::deleteSemFences(Devices &devices)
    {
        for (size_t i = 0; i < renderFinishedSemaphores.size(); i++) {
            vkDestroySemaphore(devices.LDev, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(devices.LDev, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(devices.LDev, inFlightFences[i], nullptr);
        }
    }

    VkResult SemFence::waitForFences(Devices &devices, VkCommandBuffer &commandbuffer, Swapchain &swapchain, uint32_t &imageIndex, uint32_t currentFrame, bool aquire)
    {
        vkWaitForFences(devices.LDev, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(devices.LDev, 1, &inFlightFences[currentFrame]);
        VkResult result;
        if(aquire)result = vkAcquireNextImageKHR(devices.LDev, swapchain.swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
        else result = VK_SUCCESS;
        vkResetCommandBuffer(commandbuffer, /*VkCommandBufferResetFlagBits*/ 0);
        return result;
    }
}
