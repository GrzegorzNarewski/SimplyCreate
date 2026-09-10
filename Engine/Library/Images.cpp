#include "Images.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gll
{
    Images::Images(std::string Name, Devices &devices, VkCommandPool &commandPool, std::string path)
    {
        createCubeMap(devices, commandPool, path);
        name = Name;
        paths = path;
    }
    Images::Images(std::string Name, Devices &devices, VkCommandPool &commandPool, std::string path, bool hdr)
    {
        createHDRCubeMap(devices, commandPool, path, hdr);
        name = Name;
        paths = path;
    }
    Images::Images(std::string Name, Devices &devices, VkCommandPool &commandPool, std::vector<std::string> path)
    {
        name = Name;
        mipLevels = 1;
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext = nullptr;
        imageCreateInfo.flags = 0;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent = {256, 256, 1};
        imageCreateInfo.arrayLayers = path.size();
        imageCreateInfo.mipLevels = mipLevels;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        format = VK_FORMAT_R8G8B8A8_SRGB;
        layerCount = path.size();
        aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;

        if (vkCreateImage(devices.LDev, &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(devices.LDev, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = Buffer::findMemoryType(devices, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        imageSizes = memRequirements.size;
        if (vkAllocateMemory(devices.LDev, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(devices.LDev, image, memory, 0);

        for(int i=0; i<path.size(); i++) {
            int texWidth, texHeight, texChannels;
            stbi_uc* pixels = stbi_load(path[i].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            VkDeviceSize imageSize = texWidth * texHeight * 4;

            if (!pixels) {
                throw std::runtime_error("failed to load texture image!");
            }

            Buffer Stagingbuffer;
            Stagingbuffer.createBuffer(devices, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            void* data;
            vkMapMemory(devices.LDev, Stagingbuffer.memory[0], 0, imageSize, 0, &data);
                memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(devices.LDev, Stagingbuffer.memory[0]);

            transitionImageLayout(devices, commandPool, image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            Buffer::copyBufferToImage(devices, Stagingbuffer.handel[0], image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), commandPool, i);
        }
        transitionImageLayout(devices, commandPool, image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        imageViewCreateInfo.image = image;
        imageViewCreateInfo.subresourceRange = {};
        imageViewCreateInfo.subresourceRange.aspectMask = aspectFlag;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = path.size();

        if (vkCreateImageView(devices.LDev, &imageViewCreateInfo, nullptr, &View) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }

        createTextureSampler(devices, true, false, false);
    }
    Images::Images(std::string Name, Devices &devices, VkCommandPool &commandpool, const char* path, VkFormat formats, bool repeat, VkSampleCountFlagBits numSamples)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        if (!pixels) {
            std::string tmp = path;
            throw std::runtime_error("failed to load texture image: " + tmp);
        }

        format = formats;

        Buffer Stagingbuffer;
        Stagingbuffer.createBuffer(devices, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* data;
        vkMapMemory(devices.LDev, Stagingbuffer.memory[0], 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(devices.LDev, Stagingbuffer.memory[0]);

        createImage(devices, texWidth, texHeight, format, 0, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT |  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, numSamples);
        transitionImageLayout(devices, commandpool, image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        Buffer::copyBufferToImage(devices, Stagingbuffer.handel[0], image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), commandpool);
        generateMipmaps(devices, commandpool, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

        View = createImageView(devices, image, format, mipLevels, layerCount);
        createTextureSampler(devices, true, true, repeat);

        stbi_image_free(pixels);
        Stagingbuffer.deleteBuffer(devices);

        name = Name;
        paths = path;
    }
    void Images::generateMipmaps(Devices& devices, VkCommandPool &commandpool, int32_t texWidth, int32_t texHeight)
    {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(devices.PDev, format, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = beginSingleTimeCommands(devices, commandpool);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        for (uint32_t j = 0; j < layerCount; j++) {
            int32_t mipWidth = texWidth;
            int32_t mipHeight = texHeight;
            for (uint32_t i = 1; i < mipLevels; i++) {
                barrier.subresourceRange.baseMipLevel = i - 1;
                barrier.subresourceRange.baseArrayLayer = j;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                    0, nullptr,
                    0, nullptr,
                    1, &barrier);

                VkImageBlit blit{};
                blit.srcOffsets[0] = {0, 0, 0};
                blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcSubresource.baseArrayLayer = j;
                blit.srcSubresource.layerCount = 1;
                blit.dstOffsets[0] = {0, 0, 0};
                blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = i;
                blit.dstSubresource.baseArrayLayer = j;
                blit.dstSubresource.layerCount = 1;

                vkCmdBlitImage(commandBuffer,
                    image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &blit,
                    VK_FILTER_LINEAR);

                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                    0, nullptr,
                    0, nullptr,
                    1, &barrier);

                if (mipWidth > 1) mipWidth /= 2;
                if (mipHeight > 1) mipHeight /= 2;
            }
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.subresourceRange.baseArrayLayer = layerCount - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        endSingleTimeCommands(devices, commandpool, commandBuffer);
    }
    void Images::transitionImageLayout(Devices& devices, VkCommandPool &commandpool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, int backOffset, int baseLevel, int backMipOffset, int baseMipLevel)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(devices, commandpool);
        // create the barrier
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspectFlag;
        barrier.subresourceRange.baseMipLevel = baseMipLevel;
        barrier.subresourceRange.levelCount = mipLevels - backMipOffset;
        barrier.subresourceRange.baseArrayLayer = baseLevel;
        barrier.subresourceRange.layerCount = layerCount - backOffset;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;
        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = 0;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }  else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }  else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        layout = newLayout;

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        endSingleTimeCommands(devices, commandpool, commandBuffer);
    }
    void Images::createImage(Devices &devices, uint32_t widths, uint32_t heights, VkFormat format, VkFlags flags, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkSampleCountFlagBits numSamples, VkImageAspectFlags aspectFlags)
    {
        width = widths;
        height = heights;

        layerCount = 1;
        if(flags == VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
            layerCount = 6;
        aspectFlag = aspectFlags;
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.flags = flags;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = layerCount;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        layout = VK_IMAGE_LAYOUT_UNDEFINED;

        if (vkCreateImage(devices.LDev, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(devices.LDev, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = Buffer::findMemoryType(devices, memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(devices.LDev, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(devices.LDev, image, memory, 0);

        Sampler = 0;
    }
    void Images::createTextureSampler(Devices &devices, bool anisotropyEnable, bool mipmaps, bool repeat)
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(devices.PDev, &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = repeat ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = repeat ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;;
        samplerInfo.addressModeW = repeat ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;;
        samplerInfo.anisotropyEnable = anisotropyEnable;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = mipmaps ? static_cast<float>(mipLevels) : 1.0f;
        samplerInfo.mipLodBias = 0.0f;
        if (vkCreateSampler(devices.LDev, &samplerInfo, nullptr, &Sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }
    VkImageView Images::createImageView(Devices &devices, VkImage &image, VkFormat format, uint32_t mipLevels, int layerCount, VkImageAspectFlags aspectFlags, VkImageViewType imageViewType)
    {
        VkImageView tmp;
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = image;
        createInfo.viewType = imageViewType;
        createInfo.format = format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = aspectFlags;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = mipLevels;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = layerCount;
        if (vkCreateImageView(devices.LDev, &createInfo, nullptr, &tmp) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }

        return tmp;
    }
    void Images::createHDRCubeMap(Devices &devices, VkCommandPool &commandPool, std::string path, bool hdr)
    {
        int texWidth, texHeight, texChannels;
        float* pixels = stbi_loadf(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 16;
        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        format = VK_FORMAT_R32G32B32A32_SFLOAT;

        Buffer Stagingbuffer;
        Stagingbuffer.createBuffer(devices, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* data;
        vkMapMemory(devices.LDev, Stagingbuffer.memory[0], 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(devices.LDev, Stagingbuffer.memory[0]);

        createImage(devices, texWidth, texHeight, format, 0, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT |  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT);
        transitionImageLayout(devices, commandPool, image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        Buffer::copyBufferToImage(devices, Stagingbuffer.handel[0], image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), commandPool);
        transitionImageLayout(devices, commandPool, image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        View = createImageView(devices, image, format, mipLevels, layerCount);
        createTextureSampler(devices, true, false, false);

        stbi_image_free(pixels);
        Stagingbuffer.deleteBuffer(devices);
    }
    void Images::createCubeMap(Devices &devices, VkCommandPool &commandPool, std::string path)
    {
        format = VK_FORMAT_R8G8B8A8_UNORM;
        mipLevels = 1;
        createImage(devices, 1024, 1024, format, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT);
        transitionImageLayout(devices, commandPool, image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        std::vector<std::string> cubemap_images = {
            path + "right.png",
            path + "left.png",
            path + "top.png",
            path + "bottom.png",
            path + "front.png",
            path + "back.png"
        };

        int texWidth, texHeight, texChannels;
        for(int i=0; i<cubemap_images.size(); i++){
            stbi_uc* pixels = stbi_load(cubemap_images[i].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

            if (!pixels) {
                std::string ok = "failed to load texture image : " + cubemap_images[i];
                throw std::runtime_error(ok);
            }

            VkDeviceSize imageSize = (texWidth * texHeight * 4);
            Buffer stagingBuffer;
            stagingBuffer.createBuffer(devices, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            void* data;
            vkMapMemory(devices.LDev, stagingBuffer.memory[0], 0, imageSize, 0, &data);
                memcpy(data, &pixels[0], imageSize);
            vkUnmapMemory(devices.LDev, stagingBuffer.memory[0]);

            copyBufferToImage(devices, commandPool, stagingBuffer.handel[0], image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), i);

            stagingBuffer.deleteBuffer(devices);
        }
        transitionImageLayout(devices, commandPool, image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        //generateMipmaps(devices, commandPool, texWidth, texHeight);

        createTextureSampler(devices, true, true, false);
        View = createImageView(devices, image, format, mipLevels, layerCount, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE);
    }
    void Images::deleteImage(Devices &devices)
    {
        vkDestroyImage(devices.LDev, image, nullptr);
        vkFreeMemory(devices.LDev, memory, nullptr);
        if(View)
            vkDestroyImageView(devices.LDev, View, nullptr);
        if(Sampler)
            vkDestroySampler(devices.LDev, Sampler, nullptr);
    }
    void Images::copyBufferToImage(Devices &devices, VkCommandPool &commandPool, VkBuffer buffer, VkImage &image, uint32_t width, uint32_t height, int imageBaseLayer, int mipMap)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(devices, commandPool);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = mipMap;
        region.imageSubresource.baseArrayLayer = imageBaseLayer;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        endSingleTimeCommands(devices, commandPool, commandBuffer);
    }
    void Images::copyImageToBuffer(Devices &devices, VkCommandPool &commandPool, VkBuffer& buffer, VkImage &image, uint32_t width, uint32_t height, int imageBaseLayer)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(devices, commandPool);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = imageBaseLayer;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };
        vkCmdCopyImageToBuffer(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer, 1, &region);
        endSingleTimeCommands(devices, commandPool, commandBuffer);
    }
    void Images::copyImageToImage(Devices &devices, VkCommandPool &commandPool, VkImage &dstimage, VkImage &image, uint32_t width, uint32_t height, int imageBaseLayer)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(devices, commandPool);

        VkImageCopy region{};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.mipLevel = 0;
        region.srcSubresource.baseArrayLayer = imageBaseLayer;
        region.srcSubresource.layerCount = 1;
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.mipLevel = 0;
        region.dstSubresource.baseArrayLayer = imageBaseLayer;
        region.dstSubresource.layerCount = 1;
        region.extent = {width, height, 1};

        vkCmdCopyImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstimage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        endSingleTimeCommands(devices, commandPool, commandBuffer);
    }

    Images createCubeMapFrom6Images(std::string name, Devices &devices, VkCommandPool &commandPool, std::vector<std::vector<Images*>> faces)
    {
        Images cubmap;
        cubmap.name = name;
        cubmap.format = VK_FORMAT_R8G8B8A8_SRGB;
        cubmap.mipLevels = faces.size();
        cubmap.createImage(devices, faces[0][0]->width, faces[0][0]->height, cubmap.format, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT);

        cubmap.transitionImageLayout(devices, commandPool, cubmap.image, cubmap.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        Buffer stagingBuffer;
        for(int j=0; j<faces.size(); j++) {
            for(int i=0; i<6; i++) {
                VkDeviceSize imageSize = (faces[j][i]->width * faces[j][i]->height * 4);

                stagingBuffer.createBuffer(devices, imageSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                Images::copyImageToBuffer(devices, commandPool, stagingBuffer.handel[0], faces[j][i]->image, static_cast<uint32_t>(faces[j][i]->width), static_cast<uint32_t>(faces[j][i]->height), 0);

                cubmap.copyBufferToImage(devices, commandPool, stagingBuffer.handel[0], cubmap.image, static_cast<uint32_t>(faces[j][i]->width), static_cast<uint32_t>(faces[j][i]->height), i, j);

                stagingBuffer.deleteBuffer(devices);
            }
        }

        cubmap.transitionImageLayout(devices, commandPool, cubmap.image, cubmap.format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        cubmap.createTextureSampler(devices, false, true, false);
        cubmap.View = cubmap.createImageView(devices, cubmap.image, cubmap.format, cubmap.mipLevels, cubmap.layerCount, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE);

        return cubmap;
    }
}
