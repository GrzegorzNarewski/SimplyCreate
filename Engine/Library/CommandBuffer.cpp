#include "CommandBuffer.h"

namespace gll
{
    void CommandBuffer::createCommandBuffers(Devices &devices, Window &glfwObject, int amount)
    {
        QueueFamilyIndices queueFamilyIndices = Devices::findQueueFamilies(devices.PDev, glfwObject);
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        if (vkCreateCommandPool(devices.LDev, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }

        commandBuffers.resize(amount);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();
        if (vkAllocateCommandBuffers(devices.LDev, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }
    void CommandBuffer::beginRecordCommandBuffer(Devices &devices, VkCommandBuffer &commandBuffer, int shadow, RenderPass &renderpass, FrameBuffer &framebuffer)
    {
        renderpass.beginRenderPass(framebuffer.Framebuffers[0], framebuffer.size, commandBuffer, shadow);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(framebuffer.size.width);
        viewport.height = static_cast<float>(framebuffer.size.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = framebuffer.size;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }
    void CommandBuffer::endRecordCommandBuffer(VkCommandBuffer &commandBuffer)
    {
        vkCmdEndRenderPass(commandBuffer);
    }
    void CommandBuffer::deleteCommandBuffers(Devices &devices)
    {
        vkDestroyCommandPool(devices.LDev, commandPool, nullptr);
    }

    void CommandBuffer::beginCommandBuffer(VkCommandBuffer &commandBuffer)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
    }
    void CommandBuffer::endCommandBuffer(VkCommandBuffer &commandBuffer)
    {
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    VkResult CommandBuffer::submitCommandBuffer(Devices &devices, std::vector<VkSemaphore> waitSemaphores, std::vector<VkSemaphore> signalSemaphores, VkFence &fence, Swapchain &swapchain, uint32_t imageIndex, uint32_t currentFrame, bool present)
    {
        std::vector<VkPipelineStageFlags> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT};
        if(waitSemaphores.size() == 0) waitStages.clear();
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        submitInfo.pWaitSemaphores = waitSemaphores.data();
        submitInfo.pWaitDstStageMask = waitStages.data();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
        submitInfo.signalSemaphoreCount = signalSemaphores.size();
        submitInfo.pSignalSemaphores = signalSemaphores.data();

        if (vkQueueSubmit(devices.GraphicsQueue.Handle, 1, &submitInfo, fence) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        if(present)
        {
            VkSwapchainKHR swapChains[] = {swapchain.swapChain};

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.waitSemaphoreCount = signalSemaphores.size();
            presentInfo.pWaitSemaphores = signalSemaphores.data();

            return vkQueuePresentKHR(devices.PresentQueue.Handle, &presentInfo);
        }
        return VK_SUCCESS;
    }
}
