#include "FileManager.h"

namespace gll
{
    class Engine
    {
        bool firstMouse = true;
        float lastX = 0.0f;
        float lastY = 0.0f;
    public:
        bool framebufferResized = false, windowNeedsCreating = false;
        uint32_t prevImageIndex, imageIndex, currentFrame = 0;

        Instance instance;
        Window window;
        Devices devices;
        Swapchain swapchain;
        SemFence semfences;
        Images depthImage;
        Deleter deleter;

        std::map<std::string, RenderPass> renderPasses;
        std::map<std::string, CommandBuffer> cBuffers;
        std::map<std::string, std::vector<FrameBuffer>> fBuffers;
        std::map<std::string, DescriptorSetLayout> descriptorSetLayouts;

        void createEngine(std::string windowTitle, glm::ivec2 windowSize, glm::vec3 windowColor);

        void updateEngine();
        void beginRender();
        void beginRecordCommandBuffer(std::string rendPass = "default", std::string fBuffer = "default", uint32_t imgIndex = -1);
        void endRecordCommandBuffer();
        void sumbitRender(bool present = true);
        bool waitForFences(SemFence &semfences, CommandBuffer &commandbuffer, uint32_t currentFrame, bool aquire);
        void submitCommandBuffer(CommandBuffer &commandbuffer, std::vector<VkSemaphore> waitSemaphores, std::vector<VkSemaphore> signalSemaphores, VkFence &fence, uint32_t currentFrame, bool present);
        void cleanUpSwapchain();
        void recreateSwapchain(bool createWindow);

        void deleteEngine();

        void createFrameBuffer(std::string name, int width, int height, std::string rendid, std::vector<VkImageView> attachments);
    };

    extern Engine engine;
}

