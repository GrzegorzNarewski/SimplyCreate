#include "Shader.h"

namespace gll
{
    void Shader::createShader(std::string names, Devices &devices, RenderPass &renderPass, VertexInput vInput, VkPrimitiveTopology topology, std::vector<VkPushConstantRange> const & pushConstants, std::vector<DescriptorSetLayout> DdescriptorSetLayouts, VkSampleCountFlagBits numSamples,
                   int blendMode, int msaaMode, int depthMode, bool is2ds, const char* pathv, const char* pathf, const char* pathg, const char* pathtc, const char* pathte)
    {
        is2D = is2ds;
        descriptorSetLayouts = DdescriptorSetLayouts;

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        name = names;

        std::vector<char> vertShaderCode;
        VkPipelineShaderStageCreateInfo vertShaderStageInfo;
        fragShaderModule = nullptr;
        if(pathv != nullptr) {
            vertShaderCode = readFile(pathv);
            vertShaderStageInfo = createShaderModule(devices, vertShaderModule, VK_SHADER_STAGE_VERTEX_BIT, vertShaderCode);
            shaderStages.push_back(vertShaderStageInfo);
        }

        std::vector<char> fragShaderCode;
        VkPipelineShaderStageCreateInfo fragShaderStageInfo;
        fragShaderModule = nullptr;
        if(pathf != nullptr) {
            fragShaderCode = readFile(pathf);
            fragShaderStageInfo = createShaderModule(devices, fragShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderCode);
            shaderStages.push_back(fragShaderStageInfo);
        }

        std::vector<char> geomShaderCode;
        VkPipelineShaderStageCreateInfo geomShaderStageInfo;
        geomShaderModule = nullptr;
        if(pathg != nullptr) {
            geomShaderCode = readFile(pathg);
            geomShaderStageInfo = createShaderModule(devices, geomShaderModule, VK_SHADER_STAGE_GEOMETRY_BIT, geomShaderCode);
            shaderStages.push_back(geomShaderStageInfo);
        }

        std::vector<char> tescShaderCode;
        VkPipelineShaderStageCreateInfo tescShaderStageInfo;
        tescShaderModule = nullptr;
        if(pathtc != nullptr) {
            tescShaderCode = readFile(pathtc);
            tescShaderStageInfo = createShaderModule(devices, tescShaderModule, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, tescShaderCode);
            shaderStages.push_back(tescShaderStageInfo);
        }

        std::vector<char> teseShaderCode;
        VkPipelineShaderStageCreateInfo teseShaderStageInfo;
        teseShaderModule = nullptr;
        if(pathte != nullptr) {
            teseShaderCode = readFile(pathte);
            teseShaderStageInfo = createShaderModule(devices, teseShaderModule, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, teseShaderCode);
            shaderStages.push_back(teseShaderStageInfo);
        }

        VkVertexInputBindingDescription bindingDescription = vInput.bindingDescription;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions = vInput.attributeDescriptions;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = topology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = numSamples;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;
        if(blendMode == 2)
            colorBlendAttachment.blendEnable = VK_TRUE;
        else
            colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_STENCIL_WRITE_MASK
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        std::vector<VkDescriptorSetLayout> DescriptorSetLayoutss;
        for(int i=0; i<descriptorSetLayouts.size(); i++) {
            DescriptorSetLayoutss.push_back(descriptorSetLayouts[i].handel);
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(DescriptorSetLayoutss.size());
        pipelineLayoutInfo.pSetLayouts = DescriptorSetLayoutss.data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
        pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();

        if (vkCreatePipelineLayout(devices.LDev, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineTessellationStateCreateInfo tessellation_state_create_info = {
          VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,    // VkStructureType                            sType
          nullptr,                                                      // const void                               * pNext
          0,                                                            // VkPipelineTessellationStateCreateFlags     flags
          3                                                             // uint32_t                                   patchControlPoints
        };

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pTessellationState = &tessellation_state_create_info;
        if(blendMode == 0)
            pipelineInfo.pColorBlendState = nullptr;
        else
            pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass.renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(devices.LDev, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
        if(tescShaderModule)
            vkDestroyShaderModule(devices.LDev, tescShaderModule, nullptr);
        if(teseShaderModule)
            vkDestroyShaderModule(devices.LDev, teseShaderModule, nullptr);
        if(geomShaderModule)
            vkDestroyShaderModule(devices.LDev, geomShaderModule, nullptr);
        if(fragShaderModule)
            vkDestroyShaderModule(devices.LDev, fragShaderModule, nullptr);
        if(vertShaderModule)
            vkDestroyShaderModule(devices.LDev, vertShaderModule, nullptr);
    }
    void Shader::createShader(std::string names, Devices &devices, std::vector<VkPushConstantRange> const & pushConstants, std::vector<VkDescriptorSetLayout> DescriptorSetLayouts, const char* pathc )
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(DescriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = DescriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
        pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();

        name = names;

        if (vkCreatePipelineLayout(devices.LDev, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        std::vector<char> compShaderCode;
        VkPipelineShaderStageCreateInfo compShaderStageInfo;
        compShaderModule = nullptr;
        if(pathc != nullptr) {
            compShaderCode = readFile(pathc);
            compShaderStageInfo = createShaderModule(devices, compShaderModule, VK_SHADER_STAGE_COMPUTE_BIT, compShaderCode);
        }

        VkComputePipelineCreateInfo compute_pipeline_create_info = {
          VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,   // VkStructureType                    sType
          nullptr,                                          // const void                       * pNext
          0,                                                // VkPipelineCreateFlags              flags
          compShaderStageInfo,                              // VkPipelineShaderStageCreateInfo    stage
          pipelineLayout,                                   // VkPipelineLayout                   layout
          VK_NULL_HANDLE,                                   // VkPipeline                         basePipelineHandle
          -1                                                // int32_t                            basePipelineIndex
        };

        VkResult result = vkCreateComputePipelines(devices.LDev, VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &graphicsPipeline );
        if( VK_SUCCESS != result ) {
          throw std::runtime_error("could not create compute pipeline!");
        }
        if(compShaderModule)
            vkDestroyShaderModule(devices.LDev, compShaderModule, nullptr);
    }

    void Shader::deleteShader(Devices &devices)
    {
        vkDestroyPipelineLayout(devices.LDev, pipelineLayout, nullptr);
        vkDestroyPipeline(devices.LDev, graphicsPipeline, nullptr);
    }

    VkPipelineShaderStageCreateInfo Shader::createShaderModule(Devices &devices, VkShaderModule &sm, VkShaderStageFlagBits stf, const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(devices.LDev, &createInfo, nullptr, &sm) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        VkPipelineShaderStageCreateInfo ShaderStageInfo{};
        ShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStageInfo.stage = stf;
        ShaderStageInfo.module = sm;
        ShaderStageInfo.pName = "main";

        return ShaderStageInfo;
    }
    std::vector<char> Shader::readFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            std::string error = "failed to open file: " + filename;
            throw std::runtime_error(error);
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    DescriptorSetLayout Shader::createDescriptorSetLayout(Devices& devices, std::string name, std::vector<VkDescriptorSetLayoutBinding> bindings, int i)
    {
        DescriptorSetLayout toReturn;
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();
        toReturn.name = name;
        toReturn.id = i;
        if (vkCreateDescriptorSetLayout(devices.LDev, &layoutInfo, nullptr, &toReturn.handel) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
        return toReturn;
    }
    VkDescriptorSetLayoutBinding Shader::createUboLayout(int binding, VkDescriptorType dt, int dc, int ssfb)
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = binding;
        uboLayoutBinding.descriptorType = dt;
        uboLayoutBinding.descriptorCount = dc;
        uboLayoutBinding.stageFlags = ssfb;
        return uboLayoutBinding;
    }
}
