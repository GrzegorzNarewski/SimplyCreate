#include "CommandBuffer.h"

namespace gll
{
    struct ToDelete
    {
        VkDescriptorPool* descriptorPool;
        std::vector<VkBuffer*> handel;
        std::vector<VkBufferView*> view;
        std::vector<VkDeviceMemory*> memory;
        std::vector<int> framebuffersPassed;
        Images* image; VkFramebuffer* framebuffer; RenderPass* renderpass;
        int uniform;

        ToDelete(Uniform &p_uniform, size_t framebuffers)
        {
            descriptorPool = p_uniform.descriptorPool;
            for(uint32_t i=0;i<p_uniform.uniformBuffers.size();i++){
                for(uint32_t j=0;j<p_uniform.uniformBuffers[i].size();j++){
                    handel.push_back(p_uniform.uniformBuffers[i][j].handel);
                    memory.push_back(p_uniform.uniformBuffers[i][j].memory);
                }
            }
            for(uint32_t i=0;i<framebuffers;i++){
                framebuffersPassed.push_back(i);
            }
            uniform = 1;
        }
        ToDelete(VertexBuffer &p_vertex, size_t framebuffers)
        {
            handel.push_back(p_vertex.vertexBuffer.handel);
            memory.push_back(p_vertex.vertexBuffer.memory);
            for(uint32_t i=0;i<framebuffers;i++){
                framebuffersPassed.push_back(i);
            }
            uniform = 0;
        }
        ToDelete(Images* image, size_t framebuffers)
        {
            this->image = image; uniform = 2;
        }
        ToDelete(FrameBuffer* framebuffer, size_t framebuffers)
        {
            this->framebuffer = framebuffer->Framebuffers; uniform = 3;
        }
        ToDelete(RenderPass* renderpass, size_t framebuffers)
        {
            this->renderpass = renderpass; uniform = 4;
        }
    };

    class Deleter
    {
    public:
        std::vector<ToDelete> toDelete;

        void addToDelete(Uniform &p_uniform, size_t framebuffers);
        void addToDelete(VertexBuffer &p_vertex, size_t framebuffers);
        void addToDelete(Images* image, size_t framebuffers);
        void addToDelete(FrameBuffer* framebuffer, size_t framebuffers);
        void addToDelete(RenderPass* renderpass, size_t framebuffers);
        void update(Devices &devices, int framebufferUsed);
        void deleteDeleter(Devices &devices);
        static bool findAndDeleteIntFromVector(std::vector<int> &vc, int in);

        void deleteUniform(int i, Devices &devices);
        void deleteVertexBuffer(int i, Devices &devices);
        void deleteImage(int i, Devices &devices);
        void deleteFrameBuffer(int i, Devices &devices);
        void deleteRenderPass(int i, Devices &devices);
    };
}
