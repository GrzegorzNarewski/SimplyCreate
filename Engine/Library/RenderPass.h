#include "Images.h"

namespace gll
{
    struct AttachmentReference
    {
        int use;
        VkAttachmentReference attachDes;

        AttachmentReference(int p_use, VkAttachmentReference &p_attachDes)
        {
            use = p_use;
            attachDes = p_attachDes;
        }
    };

    struct SubpassParameters
    {
        VkPipelineBindPoint                  PipelineType;
        std::vector<VkAttachmentReference>   InputAttachments;
        std::vector<VkAttachmentReference>   ColorAttachments;
        std::vector<VkAttachmentReference>   ResolveAttachments;
        VkAttachmentReference                DepthStencilAttachment;
        std::vector<uint32_t>                PreserveAttachments;
    };

    struct RenderpassCreateInfo
    {
        VkSubpassDependency dependency;
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        SubpassParameters sp;
    };

    class RenderPass
    {
    public:
        VkRenderPass renderPass;

        int usedBy = 0;
        glm::vec4 bColor;

        RenderPass() {}
        void createRenderPass(Devices &devices, RenderpassCreateInfo createInfo);
        void SpecifySubpassDescriptions(SubpassParameters &subpass_parameters, std::vector<VkSubpassDescription>& subpass_descriptions);
        void deleteRenderPass(Devices &devices);
        void beginRenderPass(VkFramebuffer &framebuffer, VkExtent2D &size, VkCommandBuffer &commandbuffer, int shadow);

        static RenderpassCreateInfo createDefulatRenderpassCreateInfo(VkFormat format);
    };
}
