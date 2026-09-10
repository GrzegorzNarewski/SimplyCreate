#include "RenderPass.h"

namespace gll
{
    class FrameBuffer
    {
    public:
        VkFramebuffer* Framebuffers;
        VkExtent2D size;
        int id;

        FrameBuffer(){};
        FrameBuffer(Devices &devices, uint32_t width, uint32_t height, int ids, RenderPass &renderpass, std::vector<VkImageView> attachments);
        void createFrameBuffer(Devices &devices, uint32_t width, uint32_t height, int ids, RenderPass &renderpass, std::vector<VkImageView> attachments);
        void deleteFrameBuffer(Devices &devices);
    };
}
