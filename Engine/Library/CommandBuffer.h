#include "SemFence.h"

namespace gll
{
    struct ToRender
    {
        std::vector<Shader*> shader;
        VertexBuffer* vBuffer;
        Uniform* uniforms;
        glm::vec3* Pos;
        glm::vec3* Size;
        glm::vec4* Rot;
        bool isShadow;
        bool isStencil;
        float* parallaxMapHieght;

        ToRender(std::vector<Shader*> p_shader, VertexBuffer &p_vBuffer, Uniform &p_uniforms,
                 glm::vec3& p_Pos, glm::vec3& p_Size, glm::vec4& p_Rot, bool p_isShadow, bool p_isStencil, float &p_parallaxMapHieght)
        {
            shader = p_shader;
            vBuffer = &p_vBuffer;
            uniforms = &p_uniforms;
            Pos = &p_Pos;
            Size = &p_Size;
            Rot = &p_Rot;
            isShadow = p_isShadow;
            isStencil = p_isStencil;
            parallaxMapHieght = &p_parallaxMapHieght;
        }
    };

    class CommandBuffer
    {
        VkDescriptorSetLayoutBinding createUboLayout(int binding, VkDescriptorType dt, int dc, VkShaderStageFlagBits ssfb);
    public:
        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;

        void beginCommandBuffer(VkCommandBuffer &commandBuffer);
        void endCommandBuffer(VkCommandBuffer &commandBuffer);

        CommandBuffer() {}
        CommandBuffer(Devices &devices, Window &glfwObject, int amount) {createCommandBuffers(devices, glfwObject, amount);}
        void createCommandBuffers(Devices &devices, Window &glfwObject, int amount);
        void beginRecordCommandBuffer(Devices &devices, VkCommandBuffer &commandBuffer, int shadow, RenderPass &renderpass, FrameBuffer &framebuffer);
        void endRecordCommandBuffer(VkCommandBuffer &commandBuffer);
        void deleteCommandBuffers(Devices &devices);

        VkResult submitCommandBuffer(Devices &devices, std::vector<VkSemaphore> waitSemaphores, std::vector<VkSemaphore> signalSemaphores, VkFence &fence, Swapchain &swapchain, uint32_t imageIndex, uint32_t currentFrame, bool present);
    };
}
