#include "Swapchain.h"

namespace gll
{
    struct BufferData
    {
        void* data;
        size_t size;
    };

    class Uniform
    {
        VkWriteDescriptorSet createVkWriteDescriptorSet(VkDescriptorSet &set, int binding, int arrayElement, VkDescriptorType descriptorType, int descriptorCount, VkDescriptorBufferInfo &bufferInfo);
        VkWriteDescriptorSet createVkWriteDescriptorSet(VkDescriptorSet &set, int binding, int arrayElement, VkDescriptorType descriptorType, int descriptorCount, VkDescriptorImageInfo &imageInfo);
        VkWriteDescriptorSet createVkWriteDescriptorSet(VkDescriptorSet &set, int binding, int arrayElement, VkDescriptorType descriptorType, int descriptorCount, VkBufferView &texelBufferView);
        void createDescriptiorSets(Devices &devices, VkDescriptorSetLayout DescriptorSetLayout, std::vector<VkDeviceSize> bufferSizes, std::vector<Images*> Textures, std::vector<Buffer> texelBufferSizes, int MAX_FRAMES_IN_FLIGHT, std::vector<std::vector<Buffer*>> publicBuffers = {});
    public:
        bool usedThisFrame;
        VkDescriptorPool *descriptorPool;
        std::string decriptorSetName;
        std::vector<VkDescriptorSet> descriptorSets;
        std::vector<std::vector<Buffer>> uniformBuffers;
        std::vector<BufferData> bufferdata;

        void createUniformBuffer(Devices &devices, std::string descSLName, VkDescriptorSetLayout DescriptorSetLayout, std::vector<VkDeviceSize> bufferSize, std::vector<Images*> Textures, std::vector<Buffer> texelBufferSizes, int MAX_FRAMES_IN_FLIGHT, std::vector<std::vector<Buffer*>> publicBuffers = {});
        void updateUniformBuffer(Devices &devices, uint32_t currentImage);
        void deleteUniformBuffer(Devices &devices);
    };
}
