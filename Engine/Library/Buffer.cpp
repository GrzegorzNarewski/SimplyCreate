#include "Buffer.h"

namespace gll
{
    void Buffer::createBuffer(Devices &devices, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        handel = new VkBuffer[1];
        VkResult result = vkCreateBuffer(devices.LDev, &bufferInfo, nullptr, &handel[0]);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(devices.LDev, handel[0], &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(devices, memRequirements.memoryTypeBits, properties);

        memory = new VkDeviceMemory[1];
        if (vkAllocateMemory(devices.LDev, &allocInfo, nullptr, &memory[0]) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        bufferSize = size;

        vkBindBufferMemory(devices.LDev, handel[0], memory[0], 0);
    }
    void Buffer::createBufferView(Devices &devices, VkFormat format)
    {
        /*viewUsed = true;
        VkBufferViewCreateInfo buffer_view_create_info = {
          VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,    // VkStructureType            sType
          nullptr,                                      // const void               * pNext
          0,                                            // VkBufferViewCreateFlags    flags
          handel[0],                                       // VkBuffer                   buffer
          format,                                       // VkFormat                   format
          0,                                            // VkDeviceSize               offset
          VK_WHOLE_SIZE                                 // VkDeviceSize               range
        };
        VkResult result = vkCreateBufferView(devices.LDev, &buffer_view_create_info, nullptr, &view[0]);
        if( VK_SUCCESS != result ) {
          throw std::runtime_error("failed to create buffer view!");
        }*/
    }
    uint32_t Buffer::findMemoryType(Devices &devices, uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(devices.PDev, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
    void Buffer::copyBuffer(Devices &devices, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool &commandPool, size_t offset)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(devices, commandPool);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        copyRegion.dstOffset = offset;
        copyRegion.srcOffset = 0;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(devices, commandPool, commandBuffer);
    }
    void Buffer::deleteBuffer(Devices &devices)
    {
        vkDestroyBuffer(devices.LDev, handel[0], nullptr);
        vkFreeMemory(devices.LDev, memory[0], nullptr);
    }
    void Buffer::copyBufferToImage(Devices &devices, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool &commandPool, int numInArray)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(devices, commandPool);
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = numInArray;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };
        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        endSingleTimeCommands(devices, commandPool, commandBuffer);
    }
    void Buffer::copyImageToBuffer(Devices &devices, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool &commandPool, int numInArray)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(devices, commandPool);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = numInArray;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdPipelineBarrier(
            commandBuffer,
            0, VK_PIPELINE_STAGE_HOST_BIT,
            0,
            0, nullptr,
            0, nullptr,
            0, nullptr
        );

        vkCmdCopyImageToBuffer(
            commandBuffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            buffer,
            1,
            &region
        );

        endSingleTimeCommands(devices, commandPool, commandBuffer);
    }

    void useStagingBufferToCopyData(Devices &devices, Buffer &dstBuffer, std::vector<float> data, VkCommandPool &commandPool)
    {
        size_t dataSize = sizeof(data[0]) * data.size();

        Buffer Stagingbuffer;
        Stagingbuffer.createBuffer(devices, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* datas;
        vkMapMemory(devices.LDev, Stagingbuffer.memory[0], 0, dataSize, 0, &datas);
            memcpy(datas, data.data(),  dataSize);
        vkUnmapMemory(devices.LDev, Stagingbuffer.memory[0]);

        dstBuffer.copyBuffer(devices, Stagingbuffer.handel[0], dstBuffer.handel[0], dataSize, commandPool);

        Stagingbuffer.deleteBuffer(devices);
        delete [] Stagingbuffer.handel;
        delete [] Stagingbuffer.memory;
    }
    void useStagingBufferToCopyData(Devices &devices, Buffer& dstBuffer, std::vector<uint32_t> data, VkCommandPool &commandPool, size_t bufferSize, size_t offset)
    {
        size_t dataSize = sizeof(uint32_t) * data.size();

        Buffer Stagingbuffer;
        Stagingbuffer.createBuffer(devices, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* datas;
        vkMapMemory(devices.LDev, Stagingbuffer.memory[0], 0, dataSize, 0, &datas);
            memcpy(datas, data.data(),  dataSize);
        vkUnmapMemory(devices.LDev, Stagingbuffer.memory[0]);

        dstBuffer.copyBuffer(devices, Stagingbuffer.handel[0], dstBuffer.handel[0], dataSize, commandPool, offset);

        Stagingbuffer.deleteBuffer(devices);
        delete [] Stagingbuffer.handel;
        delete [] Stagingbuffer.memory;
    }
    void useStagingBufferToCopyData(Devices &devices, Buffer& dstBuffer, toSend data, VkCommandPool &commandPool, size_t bufferSize, size_t offset)
    {
        Buffer Stagingbuffer;
        Stagingbuffer.createBuffer(devices, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* datas;
        vkMapMemory(devices.LDev, Stagingbuffer.memory[0], 0, sizeof(Vertex) * data.vertices.size(), 0, &datas);
            memcpy(datas, data.vertices.data(), sizeof(Vertex) * data.vertices.size());
        vkUnmapMemory(devices.LDev, Stagingbuffer.memory[0]);

        void* datas2;
        vkMapMemory(devices.LDev, Stagingbuffer.memory[0], sizeof(Vertex) * data.vertices.size(), sizeof(uint32_t) * data.indices.size(), 0, &datas2);
            memcpy(datas2, data.indices.data(),  sizeof(uint32_t) * data.indices.size());
        vkUnmapMemory(devices.LDev, Stagingbuffer.memory[0]);

        dstBuffer.copyBuffer(devices, Stagingbuffer.handel[0], dstBuffer.handel[0], bufferSize, commandPool);

        Stagingbuffer.deleteBuffer(devices);
        delete []  Stagingbuffer.handel;
        delete []  Stagingbuffer.memory;
    }
    void useStagingBufferToCopyData(Devices &devices, Buffer &dstBuffer, toSendAnim data, VkCommandPool &commandPool, size_t bufferSize, size_t offset)
    {
        Buffer Stagingbuffer;
        Stagingbuffer.createBuffer(devices, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* datas;
        vkMapMemory(devices.LDev, Stagingbuffer.memory[0], 0, sizeof(AnimVertex) * data.vertices.size(), 0, &datas);
            memcpy(datas, data.vertices.data(), sizeof(AnimVertex) * data.vertices.size());
        vkUnmapMemory(devices.LDev, Stagingbuffer.memory[0]);

        void* datas2;
        vkMapMemory(devices.LDev, Stagingbuffer.memory[0], sizeof(AnimVertex) * data.vertices.size(), sizeof(uint32_t) * data.indices.size(), 0, &datas2);
            memcpy(datas2, data.indices.data(),  sizeof(uint32_t) * data.indices.size());
        vkUnmapMemory(devices.LDev, Stagingbuffer.memory[0]);


        dstBuffer.copyBuffer(devices, Stagingbuffer.handel[0], dstBuffer.handel[0], bufferSize, commandPool);

        Stagingbuffer.deleteBuffer(devices);
        delete [] Stagingbuffer.handel;
        delete [] Stagingbuffer.memory;
    }
}
