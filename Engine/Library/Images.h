#include "Buffer.h"

namespace gll
{
    class Images
    {
    public:
        std::string name, paths;

        VkImage image;
        VkDeviceMemory memory;
        VkImageView View;
        VkSampler Sampler;
        VkImageAspectFlags aspectFlag;
        VkFormat format;
        VkImageLayout layout;
        int width, height;
        int imageSizes;
        uint32_t mipLevels = 1;
        int layerCount;

        Images() {}
        Images(std::string name, Devices &devices, VkCommandPool &commandpool, const char* path, VkFormat formats = VK_FORMAT_R8G8B8A8_SRGB, bool repeat = true, VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT);
        Images(std::string name, Devices &devices, VkCommandPool &commandPool, std::vector<std::string> path);
        Images(std::string name, Devices &devices, VkCommandPool &commandPool, std::string path);
        Images(std::string name, Devices &devices, VkCommandPool &commandPool, std::string path, bool hdr);
        void createImage(Devices &devices, uint32_t width, uint32_t height, VkFormat format, VkFlags flags, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkSampleCountFlagBits numSamples, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);
        static VkImageView createImageView(Devices &devices, VkImage &image, VkFormat format, uint32_t mipLevels = 1, int layerCount = 1, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT, VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D);
        void createCubeMap(Devices &devices, VkCommandPool &commandPool, std::string path);
        void transitionImageLayout(Devices& devices, VkCommandPool &commandpool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, int backOffset = 0, int baseLevel = 0, int backMipOffset = 0, int baseMipLevel = 0);
        void createTextureSampler(Devices &devices, bool anisotropyEnable, bool mipmaps, bool repeat);
        void createHDRCubeMap(Devices &devices, VkCommandPool &commandPool, std::string path, bool hdr);
        void generateMipmaps(Devices& devices, VkCommandPool &commandpool, int32_t texWidth, int32_t texHeight);

        static void copyImageToBuffer(Devices &devices, VkCommandPool &commandPool, VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height, int imageBaseLayer);
        void copyBufferToImage(Devices &devices, VkCommandPool &commandPool, VkBuffer buffer, VkImage &image, uint32_t width, uint32_t height, int imageBaseLayer, int mipMap = 0);
        static void copyImageToImage(Devices &devices, VkCommandPool &commandPool, VkImage &dstimage, VkImage &image, uint32_t width, uint32_t height, int imageBaseLayer);

        void deleteImage(Devices &devices);
    };

    Images createCubeMapFrom6Images(std::string name, Devices &devices, VkCommandPool &commandPool, std::vector<std::vector<Images*>> faces);
}
