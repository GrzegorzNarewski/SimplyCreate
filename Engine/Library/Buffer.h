#include "Devices.h"

namespace gll
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 nor;
        glm::vec2 uvs;
        glm::vec3 tan;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };
    struct AnimVertex
    {
        // geom
        glm::vec3 pos;
        glm::vec3 nor;
        glm::vec2 uvs;
        // anim
        glm::vec4 wei;
        glm::ivec4 joi;
        glm::vec3 tan;

        AnimVertex() {}
        AnimVertex(glm::vec3 Pos, glm::vec3 Nor, glm::vec2 Uvs, float Wei[4], int Joi[4])
        {
            pos = Pos;
            nor = Nor;
            uvs = Uvs;
            wei = { Wei[0], Wei[1], Wei[2], Wei[3] };
            joi = { Joi[0], Joi[1], Joi[2], Joi[3] };
        }

        static VkVertexInputBindingDescription getBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    struct toSend
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };
    struct toSendAnim
    {
        std::vector<AnimVertex> vertices;
        std::vector<uint32_t> indices;
    };

    class Buffer
    {
    public:
        VkBuffer *handel;
        VkDeviceMemory *memory;
        VkDeviceSize bufferSize;

        static uint32_t findMemoryType(Devices &devices, uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void createBuffer(Devices &devices, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
        void createBufferView(Devices &devices, VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT);
        void copyBuffer(Devices &devices, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool &commandPool, size_t = 0);
        void deleteBuffer(Devices &devices);
        static void copyBufferToImage(Devices &devices, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool &commandPool, int numInArray = 0);
        static void copyImageToBuffer(Devices &devices, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool &commandPool, int numInArray = 0);
    };

    void useStagingBufferToCopyData(Devices &devices, Buffer &dstBuffer, std::vector<float> data, VkCommandPool &commandPool);
    void useStagingBufferToCopyData(Devices &devices, Buffer &dstBuffer, std::vector<uint32_t> data, VkCommandPool &commandPool, size_t bufferSize, size_t offset = 0);
    void useStagingBufferToCopyData(Devices &devices, Buffer &dstBuffer, toSend data, VkCommandPool &commandPool, size_t bufferSize, size_t offset = 0);
    void useStagingBufferToCopyData(Devices &devices, Buffer &dstBuffer, toSendAnim data, VkCommandPool &commandPool, size_t bufferSize, size_t offset = 0);
}
