#include "FrameBuffer.h"

namespace gll
{
    FrameBuffer::FrameBuffer(Devices &devices, uint32_t width, uint32_t height, int ids, RenderPass &renderpass, std::vector<VkImageView> attachments)
    {
        createFrameBuffer(devices, width, height, ids, renderpass, attachments);
    }
    void FrameBuffer::createFrameBuffer(Devices &devices, uint32_t width, uint32_t height, int ids, RenderPass &renderpass, std::vector<VkImageView> attachments)
    {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderpass.renderPass;
        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = width;
        framebufferInfo.height = height;
        framebufferInfo.layers = 1;
        id = ids;

        size = {width, height};

        Framebuffers = new VkFramebuffer[1];
        if (vkCreateFramebuffer(devices.LDev, &framebufferInfo, nullptr, Framebuffers) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
    void FrameBuffer::deleteFrameBuffer(Devices &devices)
    {
        vkDestroyFramebuffer(devices.LDev, Framebuffers[0], nullptr);
    }
}
