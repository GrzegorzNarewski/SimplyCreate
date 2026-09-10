#include "Uniform.h"

namespace gll
{
    void Uniform::createUniformBuffer(Devices &devices, std::string descSLName, VkDescriptorSetLayout DescriptorSetLayout, std::vector<VkDeviceSize> bufferSizes, std::vector<Images*> Textures,
                                      std::vector<Buffer> texelBufferSizes, int MAX_FRAMES_IN_FLIGHT, std::vector<std::vector<Buffer*>> publicBuffers)
    {
        usedThisFrame = false;
        decriptorSetName = descSLName;
        descriptorPool = new VkDescriptorPool[1];

        uniformBuffers.resize(bufferSizes.size());
        for(int j=0;j<uniformBuffers.size();j++){
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                Buffer tmp; tmp.createBuffer(devices, bufferSizes[j], VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                uniformBuffers[j].push_back(tmp);
            }
        }
        std::vector<VkDescriptorPoolSize> poolSizes(bufferSizes.size() + Textures.size() + texelBufferSizes.size() + publicBuffers.size());
        int i = 0, j = 0, k = 0, l = 0;
        for(i=0; i<bufferSizes.size(); i++)
        {
            poolSizes[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[i].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        }
        for(j=i; j<publicBuffers.size() + i; j++)
        {
            poolSizes[j].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[j].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        }
        for(k=j; k<Textures.size() + j; k++)
        {
            poolSizes[k].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[k].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        }
        for(l=k; l<texelBufferSizes.size() + k; l++)
        {
            poolSizes[l].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
            poolSizes[l].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        }

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        if (vkCreateDescriptorPool(devices.LDev, &poolInfo, nullptr, &descriptorPool[0]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        bufferdata.resize(bufferSizes.size());
        for(int i=0;i<bufferdata.size();i++)
        {
            bufferdata[i].size = bufferSizes[i];
        }

        createDescriptiorSets(devices, DescriptorSetLayout, bufferSizes, Textures, texelBufferSizes, MAX_FRAMES_IN_FLIGHT, publicBuffers);
    }
    void Uniform::createDescriptiorSets(Devices &devices, VkDescriptorSetLayout DescriptorSetLayout, std::vector<VkDeviceSize> bufferSizes, std::vector<Images*> Textures, std::vector<Buffer> texelBufferSizes, int MAX_FRAMES_IN_FLIGHT, std::vector<std::vector<Buffer*>> publicBuffers)
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, DescriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool[0];
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();
        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(devices.LDev, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            std::vector<VkDescriptorBufferInfo> bufferInfos;
            std::vector<VkDescriptorBufferInfo> publicbufferInfos;
            std::vector<VkDescriptorImageInfo> imageInfos;

            bufferInfos.resize(bufferSizes.size());
            for(int j=0;j<bufferSizes.size();j++)
            {
                bufferInfos[j].buffer = uniformBuffers[j][i].handel[0];
                bufferInfos[j].offset = 0;
                bufferInfos[j].range = bufferSizes[j];
            }

            publicbufferInfos.resize(publicBuffers.size());
            for(int j=0;j<publicbufferInfos.size();j++)
            {
                publicbufferInfos[j].buffer = publicBuffers[j][i][0].handel[0];
                publicbufferInfos[j].offset = 0;
                publicbufferInfos[j].range = publicBuffers[j][i][0].bufferSize;
            }

            imageInfos.resize(Textures.size());
            for(int j=0;j<Textures.size();j++)
            {
                imageInfos[j].imageLayout = Textures[j][0].layout;
                imageInfos[j].imageView = Textures[j][0].View;
                imageInfos[j].sampler = Textures[j][0].Sampler;
            }

            std::vector<VkWriteDescriptorSet> descriptorWrites(bufferSizes.size() + Textures.size() + texelBufferSizes.size() + publicbufferInfos.size());
            int j, jj, kk, ll;
            for(j=0; j<bufferSizes.size(); j++)
            {
                descriptorWrites[j] = createVkWriteDescriptorSet(descriptorSets[i], j, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, bufferInfos[j]);
            }
            for(jj=j; jj<publicbufferInfos.size() + j; jj++)
            {
                descriptorWrites[jj] = createVkWriteDescriptorSet(descriptorSets[i], jj, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, publicbufferInfos[jj - j]);
            }
            for(kk=jj; kk<Textures.size() + jj; kk++)
            {
                descriptorWrites[kk] = createVkWriteDescriptorSet(descriptorSets[i], kk, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, imageInfos[kk - jj]);
            }

            vkUpdateDescriptorSets(devices.LDev, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }
    void Uniform::updateUniformBuffer(Devices &devices, uint32_t currentImage)
    {
        for(int i=0;i<bufferdata.size();i++)
        {
            void* data;
            vkMapMemory(devices.LDev, uniformBuffers[i][currentImage].memory[0], 0, bufferdata[i].size, 0, &data);
            memcpy(data, bufferdata[i].data, bufferdata[i].size);
            vkUnmapMemory(devices.LDev, uniformBuffers[i][currentImage].memory[0]);
        }
    }
    void Uniform::deleteUniformBuffer(Devices &devices)
    {
        vkDestroyDescriptorPool(devices.LDev, descriptorPool[0], nullptr);
        for (size_t j = 0; j < uniformBuffers.size(); j++) {
            for (size_t i = 0; i < uniformBuffers[j].size(); i++) {
                uniformBuffers[j][i].deleteBuffer(devices);
            }
        }
    }
    VkWriteDescriptorSet Uniform::createVkWriteDescriptorSet(VkDescriptorSet &set, int binding, int arrayElement, VkDescriptorType descriptorType, int descriptorCount, VkDescriptorBufferInfo &bufferInfo)
    {
        VkWriteDescriptorSet descriptorWrites{};
        descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites.pNext = nullptr;
        descriptorWrites.dstSet = set;
        descriptorWrites.dstBinding = binding;
        descriptorWrites.dstArrayElement = arrayElement;
        descriptorWrites.descriptorType = descriptorType;
        descriptorWrites.descriptorCount = descriptorCount;
        descriptorWrites.pBufferInfo = &bufferInfo;
        return descriptorWrites;
    }
    VkWriteDescriptorSet Uniform::createVkWriteDescriptorSet(VkDescriptorSet &set, int binding, int arrayElement, VkDescriptorType descriptorType, int descriptorCount, VkDescriptorImageInfo &imageInfo)
    {
        VkWriteDescriptorSet descriptorWrites{};
        descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites.pNext = nullptr;
        descriptorWrites.dstSet = set;
        descriptorWrites.dstBinding = binding;
        descriptorWrites.dstArrayElement = arrayElement;
        descriptorWrites.descriptorType = descriptorType;
        descriptorWrites.descriptorCount = descriptorCount;
        descriptorWrites.pImageInfo = &imageInfo;
        return descriptorWrites;
    }
    VkWriteDescriptorSet Uniform::createVkWriteDescriptorSet(VkDescriptorSet &set, int binding, int arrayElement, VkDescriptorType descriptorType, int descriptorCount, VkBufferView &texelBufferView)
    {
        VkWriteDescriptorSet descriptorWrites{};
        descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites.pNext = nullptr;
        descriptorWrites.dstSet = set;
        descriptorWrites.dstBinding = binding;
        descriptorWrites.dstArrayElement = arrayElement;
        descriptorWrites.descriptorType = descriptorType;
        descriptorWrites.descriptorCount = descriptorCount;
        descriptorWrites.pTexelBufferView = &texelBufferView;
        return descriptorWrites;
    }
}
