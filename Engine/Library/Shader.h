#include "Uniform.h"

namespace gll
{
    struct DescriptorSetLayout
    {
        std::string name; int id;
        VkDescriptorSetLayout handel;
    };

    struct VertexInput
    {
        VkVertexInputBindingDescription bindingDescription;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        VertexInput() {}
        VertexInput(VkVertexInputBindingDescription bindingDescriptions, std::vector<VkVertexInputAttributeDescription> attributeDescriptionss)
        {
            bindingDescription = bindingDescriptions;
            attributeDescriptions = attributeDescriptionss;
        }
    };

    class Shader
    {
        std::vector<char> readFile(const std::string& filename);
        VkPipelineShaderStageCreateInfo createShaderModule(Devices &devices, VkShaderModule &sm, VkShaderStageFlagBits stf, const std::vector<char>& code);
    public:
        std::string name; bool is2D = false;
        std::vector<DescriptorSetLayout> descriptorSetLayouts;

        VkPipelineLayout pipelineLayout;

        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
        VkShaderModule geomShaderModule;
        VkShaderModule tescShaderModule;
        VkShaderModule teseShaderModule;

        VkShaderModule compShaderModule;

        VkPipeline graphicsPipeline;

        static DescriptorSetLayout createDescriptorSetLayout(Devices& devices, std::string name, std::vector<VkDescriptorSetLayoutBinding> bindings, int i);
        static VkDescriptorSetLayoutBinding createUboLayout(int binding, VkDescriptorType dt, int dc, int ssfb);

        void createShader(std::string name, Devices &devices, RenderPass &renderPass, VertexInput vInput, VkPrimitiveTopology topology, std::vector<VkPushConstantRange> const & pushConstants, std::vector<DescriptorSetLayout> DescriptorSetLayouts, VkSampleCountFlagBits numSamples,
               int blendMode, int msaaMode, int depthMode,  bool is2ds, const char* pathv = nullptr, const char* pathf = nullptr, const char* pathg = nullptr, const char* pathtc = nullptr, const char* pathte = nullptr);
        void createShader(std::string name, Devices &devices, std::vector<VkPushConstantRange> const & pushConstants, std::vector<VkDescriptorSetLayout> DescriptorSetLayouts, const char* pathc = nullptr);
        void deleteShader(Devices &devices);
    };
}
