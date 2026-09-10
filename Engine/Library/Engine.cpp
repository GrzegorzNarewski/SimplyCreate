#include "Engine.h"

namespace gll
{
    Engine engine;

    void Engine::createEngine(std::string windowTitle, glm::ivec2 windowSize, glm::vec3 windowColor)
    {
        window.createWindow(windowTitle, windowSize.x, windowSize.y, windowColor);
        lastX = (float)windowSize.x / 2.0f;
        lastY = (float)windowSize.y / 2.0f;
        instance.createInstance();
        window.createWindowSurface(instance);

        devices.createLogicalDevice(instance, window);

        swapchain.createSwapchain(devices, window);

        semfences.createSemFences(devices, swapchain.Image.size());

        cBuffers["default"].createCommandBuffers(devices, window, (int)MAX_FRAMES_IN_FLIGHT);

        depthImage.createImage(devices, swapchain.Extent.width, swapchain.Extent.height, VK_FORMAT_D32_SFLOAT, 0, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
        depthImage.View = Images::createImageView(devices, depthImage.image, VK_FORMAT_D32_SFLOAT, 1, 1, VK_IMAGE_ASPECT_DEPTH_BIT);

        RenderpassCreateInfo rpci;
        rpci = RenderPass::createDefulatRenderpassCreateInfo(swapchain.Format);
        renderPasses["default"].createRenderPass(devices, rpci);

        rpci.attachmentDescriptions[0].format = VK_FORMAT_R8G8B8A8_SRGB;
        rpci.attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        renderPasses["Off_Screen"].createRenderPass(devices, rpci);

        fBuffers["default"] = {};
        for(int i=0;i<swapchain.ImageViews.size();i++) {
            fBuffers["default"].push_back({});
            fBuffers["default"].back().createFrameBuffer(devices, (int)swapchain.Extent.width, (int)swapchain.Extent.height, i, renderPasses["default"], {swapchain.ImageViews[i], depthImage.View});
        }

        std::vector<VkDescriptorSetLayoutBinding> bindings = {Shader::createUboLayout(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT),
                                                              Shader::createUboLayout(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT)};
        descriptorSetLayouts["Buffer-Image"] = Shader::createDescriptorSetLayout(devices, "Buffer-Image", bindings, 0);
        std::vector<VkDescriptorSetLayoutBinding> bindings2 = {Shader::createUboLayout(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT),
                                                               Shader::createUboLayout(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT),
                                                               Shader::createUboLayout(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT)};
        descriptorSetLayouts["Buffer-Image-Image"] = Shader::createDescriptorSetLayout(devices, "Buffer-Image-Image", bindings2, 1);
        std::vector<VkDescriptorSetLayoutBinding> bindings3 = {Shader::createUboLayout(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT),
                                                               Shader::createUboLayout(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT),
                                                               Shader::createUboLayout(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT),
                                                               Shader::createUboLayout(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT)};
        descriptorSetLayouts["Buffer-Buffer-Image-Image"] = Shader::createDescriptorSetLayout(devices, "Buffer-Buffer-Image-Image", bindings3, 1);
    }

    void Engine::updateEngine()
    {
        window.updateWindow();
    }

    void Engine::deleteEngine()
    {
        deleter.deleteDeleter(devices);

        semfences.deleteSemFences(devices);
        for(auto f : fBuffers) {
            for(int i=0;i<f.second.size();i++)
                f.second[i].deleteFrameBuffer(devices);
        }
        fBuffers.clear();
        for(auto d : descriptorSetLayouts) {
            vkDestroyDescriptorSetLayout(devices.LDev, d.second.handel, nullptr);
        }
        descriptorSetLayouts.clear();
        for(auto r : renderPasses) {
            r.second.deleteRenderPass(devices);
        }
        renderPasses.clear();
        for(auto c : cBuffers) {
            c.second.deleteCommandBuffers(devices);
        }
        cBuffers.clear();

        swapchain.deleteSwapchain(devices);
        depthImage.deleteImage(devices);

        devices.deleteLogicalDevice();

        window.deleteWindow();
        window.deleteWindowSurface(instance);

        instance.deleteInstance();
    }

    void Engine::cleanUpSwapchain()
    {
        for(int i=0;i<fBuffers["default"].size();i++)
            fBuffers["default"][i].deleteFrameBuffer(devices);
        fBuffers.erase("default");
        swapchain.deleteSwapchain(devices);
        depthImage.deleteImage(devices);
    }
    void Engine::recreateSwapchain(bool createWindow)
    {
        vkDeviceWaitIdle(devices.LDev);

        cleanUpSwapchain();

        swapchain.createSwapchain(devices, window);
        depthImage.createImage(devices, swapchain.Extent.width, swapchain.Extent.height, VK_FORMAT_D32_SFLOAT, 0, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
        depthImage.View = Images::createImageView(devices, depthImage.image, VK_FORMAT_D32_SFLOAT, 1, 1, VK_IMAGE_ASPECT_DEPTH_BIT);
        fBuffers["default"] = {};
        for(int i=0;i<swapchain.ImageViews.size();i++) {
            fBuffers["default"].push_back({});
            fBuffers["default"].back().createFrameBuffer(devices, (int)swapchain.Extent.width, (int)swapchain.Extent.height, i, renderPasses["default"], {swapchain.ImageViews[i], depthImage.View});
        }
    }
    void Engine::submitCommandBuffer(CommandBuffer &commandbuffer, std::vector<VkSemaphore> waitSemaphores, std::vector<VkSemaphore> signalSemaphores, VkFence &fence, uint32_t currentFrame, bool present)
    {
        VkResult result = commandbuffer.submitCommandBuffer(devices, waitSemaphores, signalSemaphores, fence, swapchain, imageIndex, currentFrame, present);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            recreateSwapchain(windowNeedsCreating);
            windowNeedsCreating = false; framebufferResized = false;
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }
    bool Engine::waitForFences(SemFence &semfences, CommandBuffer &commandbuffer, uint32_t currentFrame, bool aquire)
    {
        prevImageIndex = imageIndex;
        VkResult result = semfences.waitForFences(devices, commandbuffer.commandBuffers[currentFrame], swapchain, imageIndex, currentFrame, aquire);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapchain(false);
            return true;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
        vkResetFences(devices.LDev, 1, &semfences.inFlightFences[currentFrame]);
        return false;
    }
    void Engine::beginRender()
    {
        renderPasses["default"].bColor = glm::vec4(window.color, 1.0f);

        if(waitForFences(semfences, cBuffers["default"], currentFrame, true)) {return;}
        cBuffers["default"].beginCommandBuffer(cBuffers["default"].commandBuffers[currentFrame]);

        deleter.update(devices, fBuffers["default"][imageIndex].id);
    }
    void Engine::beginRecordCommandBuffer(std::string rendPass, std::string fBuffer, uint32_t imgIndex)
    {
        cBuffers["default"].beginRecordCommandBuffer(devices, cBuffers["default"].commandBuffers[currentFrame], 0, renderPasses[rendPass], fBuffers[fBuffer][imgIndex == -1 ? imageIndex : imgIndex]);
    }
    void Engine::endRecordCommandBuffer()
    {
        cBuffers["default"].endRecordCommandBuffer(cBuffers["default"].commandBuffers[currentFrame]);
    }
    void Engine::sumbitRender(bool present)
    {
        cBuffers["default"].endCommandBuffer(cBuffers["default"].commandBuffers[currentFrame]);
        submitCommandBuffer(cBuffers["default"], {semfences.imageAvailableSemaphores[currentFrame]}, {semfences.renderFinishedSemaphores[currentFrame]}, semfences.inFlightFences[currentFrame], currentFrame, present);
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Engine::createFrameBuffer(std::string name, int width, int height, std::string rendid, std::vector<VkImageView> attachments)
    {
        fBuffers[name] = {};
        fBuffers[name].push_back({});
        fBuffers[name].back().createFrameBuffer(devices, width, height, 0, renderPasses[rendid], attachments);
    }
}
