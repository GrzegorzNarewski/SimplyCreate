#include "Deleter.h"

namespace gll
{
	class Stock
	{
	public:
        // Basic Vulkan Headers
        Instance instance;
        Devices devices;
        Swapchain swapchain;
        SemFence semfences;
        Images depthImage;
        Deleter deleter;

        // used for frame rescale and image presenting
        bool framebufferResized = false, windowNeedsCreating = false;
        uint32_t prevImageIndex, imageIndex, currentFrame = 0;

        // rendering resources
        glm::vec3 bgColor;
        std::map<std::string, RenderPass> renderPasses;
        std::map<std::string, CommandBuffer> cBuffers;
        std::map<std::string, std::vector<FrameBuffer>> fBuffers;
        std::map<std::string, DescriptorSetLayout> descriptorSetLayouts;

		void createStock();
		void deleteStock();

        void cleanUpSwapchain();
        void recreateSwapchain(bool createWindow);
        void submitCommandBuffer(CommandBuffer& commandbuffer, std::vector<VkSemaphore> waitSemaphores, std::vector<VkSemaphore> signalSemaphores, VkFence& fence, uint32_t currentFrame, bool present);
        bool waitForFences(SemFence& semfences, CommandBuffer& commandbuffer, uint32_t currentFrame, bool aquire);
        void beginRender();
        void beginRecordCommandBuffer(std::string rendPass, std::string fBuffer, uint32_t imgIndex = -1);
        void endRecordCommandBuffer();
        void submitRender(bool present);

        void createFrameBuffer(std::string name, int width, int height, std::string rendid, std::vector<VkImageView> attachments);
	};

	Stock* stock();
}