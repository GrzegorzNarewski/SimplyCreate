#include "RenderPass.h"

namespace gll
{
    void RenderPass::createRenderPass(Devices &devices, RenderpassCreateInfo createInfo)
    {
        std::vector<VkSubpassDescription> subpass_descriptions;
        SpecifySubpassDescriptions({createInfo.sp}, subpass_descriptions );

        VkRenderPassCreateInfo render_pass_create_info = {
          VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,                // VkStructureType                    sType
          nullptr,                                                  // const void                       * pNext
          0,                                                        // VkRenderPassCreateFlags            flags
          static_cast<uint32_t>(createInfo.attachmentDescriptions.size()),     // uint32_t                           attachmentCount
          createInfo.attachmentDescriptions.data(),                            // const VkAttachmentDescription    * pAttachments
          static_cast<uint32_t>(subpass_descriptions.size()),       // uint32_t                           subpassCount
          subpass_descriptions.data(),                              // const VkSubpassDescription       * pSubpasses
          1,                                                        // uint32_t                           dependencyCount
          &createInfo.dependency                                               // const VkSubpassDependency        * pDependencies
        };
        if (vkCreateRenderPass(devices.LDev, &render_pass_create_info, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }
    void RenderPass::deleteRenderPass(Devices &devices)
    {
        vkDestroyRenderPass(devices.LDev, renderPass, nullptr);
    }
    void RenderPass::beginRenderPass(VkFramebuffer &framebuffer, VkExtent2D &size, VkCommandBuffer &commandbuffer, int shadow)
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = size;
        std::vector<VkClearValue> clearValues{};
        if(shadow) {
            clearValues.resize(1);
            clearValues[0].depthStencil = {1.0f, 0};
        }
        else {
            clearValues.resize(2);
            clearValues[0].color = {{bColor.x, bColor.y, bColor.z, bColor.w}};
            clearValues[1].depthStencil = {1.0f, 0};
        }
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandbuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }
    void RenderPass::SpecifySubpassDescriptions(SubpassParameters &subpass_parameters, std::vector<VkSubpassDescription>& subpass_descriptions)
    {
        subpass_descriptions.clear();
        subpass_descriptions.push_back({
            0,                                                                      // VkSubpassDescriptionFlags        flags
            subpass_parameters.PipelineType,                                       // VkPipelineBindPoint              pipelineBindPoint
            static_cast<uint32_t>(subpass_parameters.InputAttachments.size()),     // uint32_t                         inputAttachmentCount
            subpass_parameters.InputAttachments.data(),                            // const VkAttachmentReference    * pInputAttachments
            static_cast<uint32_t>(subpass_parameters.ColorAttachments.size()),     // uint32_t                         colorAttachmentCount
            subpass_parameters.ColorAttachments.data(),                            // const VkAttachmentReference    * pColorAttachments
            subpass_parameters.ResolveAttachments.data(),                          // const VkAttachmentReference    * pResolveAttachments
            (subpass_parameters.DepthStencilAttachment.attachment != -1) ? &subpass_parameters.DepthStencilAttachment : nullptr,                            // const VkAttachmentReference    * pDepthStencilAttachment
            static_cast<uint32_t>(subpass_parameters.PreserveAttachments.size()),  // uint32_t                         preserveAttachmentCount
            subpass_parameters.PreserveAttachments.data()                          // const uint32_t                 * pPreserveAttachments
        });
    }

    RenderpassCreateInfo RenderPass::createDefulatRenderpassCreateInfo(VkFormat format)
    {
        RenderpassCreateInfo createInfo;

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = VK_FORMAT_D32_SFLOAT;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;;

        std::vector<SubpassParameters> subpass_parameters = {
          {
            VK_PIPELINE_BIND_POINT_GRAPHICS, {}, // std::vector<VkAttachmentReference>   InputAttachments
            {{                                             // std::vector<VkAttachmentReference>   ColorAttachments
                0,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            }},
            {}, // std::vector<VkAttachmentReference>   ResolveAttachments
            {
                depthAttachmentRef
            },  // VkAttachmentReference const        * DepthStencilAttachment
            {}  // std::vector<uint32_t>                PreserveAttachments
          }
        };

        createInfo.sp = subpass_parameters[0];
        createInfo.sp.DepthStencilAttachment.attachment = 1; // set to -1 if no depth !!
        createInfo.attachmentDescriptions = {colorAttachment, depthAttachment};
        createInfo.dependency = dependency;

        return createInfo;
    }
}
