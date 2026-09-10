#include "ResourceManager.h"

namespace gll
{
    ResourceManager resources;

    void ResourceManager::createResourceManager()
    {
        lightinfos.clear();
    }

    void ResourceManager::restartUBOs()
    {
        for(int i=0; i<uniformBuffers.size(); i++) {
            if(uniformBuffers[i].usedThisFrame == false) {
                stock()->deleter.addToDelete(uniformBuffers[i], stock()->fBuffers["default"].size());
                uniformBuffers.erase(uniformBuffers.begin() + i);
                i--;
                continue;
            }
            uniformBuffers[i].usedThisFrame = false;
        }
    }
    void ResourceManager::updateViewMatrices(Camera* camera, int width, int height)
	{
        maxX = (float)((float)width/(float)height);

        view = camera->GetViewMatrix();
        proj = glm::perspective(glm::radians(camera->Zoom), maxX, 0.2f, 1000000.0f);

        viewPos = camera->Position;
        proj2d = glm::ortho(-maxX, maxX, -1.0f, 1.0f, -100.0f, 100.0f);

        glm::vec2 tmpMP = window()->mousePos;

        mousePos.x = ((tmpMP.x / window()->width) * 2.0f) - 1.0f;
        mousePos.y = ((tmpMP.y / window()->height) * 2.0f) - 1.0f;

        glm::vec4 camPos = glm::vec4(camera->Position, 1.0f);

        for(auto r : shaders) {
            for(auto s : r.second) {
                vkCmdPushConstants( stock()->cBuffers["default"].commandBuffers[stock()->currentFrame], s.second.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, 16 * sizeof(float), &view );
                vkCmdPushConstants( stock()->cBuffers["default"].commandBuffers[stock()->currentFrame], s.second.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 32 * sizeof(float), 4 * sizeof(float), &camPos);
            }
        }
	}

	Animation* ResourceManager::findAnimation(std::string name)
	{
        if(animations.find(name) != animations.end()) {
            return animations[name];
        }

        animations[name] = new Animation[1];
        std::string path = "Resources/Meshes/" + name;
        animations[name]->createAnimation(path.c_str());
        return animations[name];
	}

    Shader* ResourceManager::findShader(std::string name, std::string renderPass)
    {
        if(shaders.find(renderPass) != shaders.end()) {
            if(shaders[renderPass].find(name) != shaders[renderPass].end()) {
                return &shaders[renderPass][name];
            }
        }

        std::string path1 = "Resources/Shaders/" + name + "_vert.spv";
        std::string path2 = "Resources/Shaders/" + name + "_frag.spv";
        std::string path3 = "Resources/Shaders/" + name + "_geom.spv";

        std::string descript = "Buffer-Image", vInp = "Vertex";

        std::vector<File_Data> data = FileManager::readFile("Shaders/details/" + name + ".save");

        for(int i=0; i<data.size(); i++) {
            if(data[i].title == "descriptor") {
                descript = data[i].content[0];
            }
            if(data[i].title == "vertexinput") {
                vInp = data[i].content[0];
            }
        }

        if (FileManager::File_Exists(path3))
            shaders[renderPass][name].createShader(name, stock()->devices, stock()->renderPasses[renderPass], vertexInputs[vInp], VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, {viewMatricesPushConstants}, {stock()->descriptorSetLayouts[descript]}, VK_SAMPLE_COUNT_1_BIT, 2, 1, 1, false, path1.c_str(), path2.c_str(), path3.c_str());
        else
            shaders[renderPass][name].createShader(name, stock()->devices, stock()->renderPasses[renderPass], vertexInputs[vInp], VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, {viewMatricesPushConstants}, {stock()->descriptorSetLayouts[descript]}, VK_SAMPLE_COUNT_1_BIT, 2, 1, 1, false, path1.c_str(), path2.c_str());
        return &shaders[renderPass][name];

    }
    VertexBuffer* ResourceManager::findVertexBuffer(std::string name)
    {
        auto it = vBuffers.find(name);
        if(it != vBuffers.end()) {
            return vBuffers[name];
        }
        else {
            std::string path1 = "Resources/Meshes/" + name;
            bool collada = name.substr(name.size() - 4, 4) == ".dae" ? true : false;
            if(name == "square") {
                vBuffers[name] = new VertexBuffer[1]{{name, stock()->devices, stock()->cBuffers["default"].commandPool}};
            }
            else {
                vBuffers[name] = new VertexBuffer[1]{{name, path1, stock()->devices, stock()->cBuffers["default"].commandPool, collada}};
            }
            return vBuffers[name];
        }
    }
    Images* ResourceManager::findTexture(std::string name)
    {
        auto it = textures.find(name);
        if(it != textures.end()) {
            return textures[name];
        }
        else {
            std::string path1 = "Resources/Textures/" + name;
            textures[name] = new Images[1]{{name, stock()->devices, stock()->cBuffers["default"].commandPool, path1.c_str(), VK_FORMAT_R8G8B8A8_SRGB, true, VK_SAMPLE_COUNT_1_BIT}};
            return textures[name];
        }
    }
    int ResourceManager::findUBO(std::string name)
	{
        int decId = -1;
        for(int j=0; j<uniformBuffers.size(); j++) {
            if(uniformBuffers[j].decriptorSetName == name && uniformBuffers[j].usedThisFrame == 0) {
                decId = j;
                uniformBuffers[j].usedThisFrame = true;
                break;
            }
        }
        return decId;
	}

	void ResourceManager::renderVBuffer(std::string vBuffer, Shader* tmpS, int& decId, glm::mat4& proj)
	{
        CommandBuffer* cbuff = &stock()->cBuffers["default"];

        vkCmdPushConstants(cbuff->commandBuffers[stock()->currentFrame], tmpS->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 16 * sizeof(float), 16 * sizeof(float), &proj);

        VertexBuffer* vbuff = findVertexBuffer(vBuffer);
        vbuff->bindVertexBuffer(cbuff->commandBuffers[stock()->currentFrame]);
        vkCmdBindPipeline(cbuff->commandBuffers[stock()->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, tmpS->graphicsPipeline);
        vkCmdBindDescriptorSets(cbuff->commandBuffers[stock()->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, tmpS->pipelineLayout, 0, 1, &uniformBuffers[decId].descriptorSets[stock()->imageIndex], 0, nullptr);
        vbuff->renderVertexBuffer(cbuff->commandBuffers[stock()->currentFrame]);
	}
	int ResourceManager::findUBOToUse(std::string& uboName, Shader* tmpS, std::vector<std::string> texts, std::vector<size_t> ubufferSizes)
	{
        int decId = findUBO(uboName);
        if(decId == -1) {
            decId = uniformBuffers.size();

            std::vector<Images*> imgs;
            for(int i=0; i<texts.size(); i++) { imgs.push_back(findTexture(texts[i])); };

            Uniform tmp; tmp.createUniformBuffer(stock()->devices, uboName, tmpS->descriptorSetLayouts[0].handel, ubufferSizes, imgs, {}, stock()->fBuffers["default"].size(), {});
            uniformBuffers.push_back(tmp);
            uniformBuffers.back().usedThisFrame = true;
        }

        return decId;
	}

	glm::mat4 ResourceManager::calcModel(glm::vec3 pos, glm::vec3 size, glm::vec3 rot)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
		model = glm::scale(model, glm::vec3(size.x, size.y, size.z));
		model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
		return model;
	}

    void ResourceManager::renderMesh(std::string shader, std::string vBuffer, std::vector<std::string> texts, glm::vec3 pos, glm::vec3 size, glm::vec3 rot, glm::vec4 color, std::string currentRenderPass, glm::mat4 outerModel)
    {
	    Shader* tmpS = findShader(shader, currentRenderPass);

	    std::string uboName = "MODEL_SQUARE" + shader + " ";
	    for(int i=0; i<texts.size(); i++) { uboName += texts[i]; }
        int decId = findUBOToUse(uboName, tmpS, texts, {sizeof(ModelUBO), sizeof(LightUBO)});

        glm::mat4 model = calcModel(pos, size, rot);

        ModelUBO ubo;
        ubo.model = model * outerModel;
        ubo.color = color;
        uniformBuffers[decId].bufferdata[0].data = &ubo;

        LightUBO ubo2;
        if(lightinfos.size() > 0) {
            ubo2 = lightinfos[0];
        }
        else {
            ubo2.position = glm::vec4(100000.0f);
            ubo2.color = glm::vec4(1.0f);
        }
        uniformBuffers[decId].bufferdata[1].data = &ubo2;

        uniformBuffers[decId].updateUniformBuffer(stock()->devices, stock()->imageIndex);

        renderVBuffer(vBuffer, tmpS, decId, proj);
    }
    void ResourceManager::renderAnimMesh(std::string shader, std::string vBuffer, std::vector<std::string> texts, std::vector<glm::mat4>& models, std::string currentRenderPass)
    {
	    Shader* tmpS = findShader(shader, currentRenderPass);

	    std::string uboName = "MODEL_SQUARE" + shader + " ";
	    for(int i=0; i<texts.size(); i++) { uboName += texts[i]; }
        int decId = findUBOToUse(uboName, tmpS, texts, {sizeof(AnimModelUBO), sizeof(LightUBO)});

        AnimModelUBO ubo;
        for(int i=0; i<models.size(); i++) {
            ubo.model[i] = models[i];
        }
        uniformBuffers[decId].bufferdata[0].data = &ubo;

        LightUBO ubo2;
        if(lightinfos.size() > 0) {
            ubo2 = lightinfos[0];
        }
        uniformBuffers[decId].bufferdata[1].data = &ubo2;

        uniformBuffers[decId].updateUniformBuffer(stock()->devices, stock()->imageIndex);

        renderVBuffer(vBuffer, tmpS, decId, proj);
    }
    void ResourceManager::renderSprite(std::string shader, std::string vBuffer, std::string texture, glm::vec3 pos, glm::vec3 size, glm::vec3 rot, glm::vec4 color, std::string currentRenderPass)
    {
	    Shader* tmpS = findShader(shader, currentRenderPass);

	    std::string uboName = "MODEL_SQUARE" + shader + " " + texture;
        int decId = findUBOToUse(uboName, tmpS, {texture}, {sizeof(ModelUBO)});

        glm::mat4 model = calcModel(pos, size, rot);

        ModelUBO ubo;
        ubo.model = model;
        ubo.color = color;

        uniformBuffers[decId].bufferdata[0].data = &ubo;
        uniformBuffers[decId].updateUniformBuffer(stock()->devices, stock()->imageIndex);

        renderVBuffer(vBuffer, tmpS, decId, proj2d);
    }
    void ResourceManager::renderText(std::string context, glm::vec3 pos, float fontsize, glm::vec4 color, std::string currentRenderPass, TEXT_LAYOUT layout, float xMostLeft)
    {
	    Shader* tmpS = findShader("text", currentRenderPass);

	    std::string uboName = "MODEL_SQUAREtext bitmapfont.png";

        TextUBO ubo;
        ubo.color = color;

        glm::vec3 poses = glm::vec3(pos.x - (fontsize*2.2f), pos.y, pos.z);

        if(layout == CENTERED)
            poses.x -= (fontsize * 2.2f) * ((float)(context.size() - 1) / 2.0f);
        else if(layout == LEFT)
            poses.x -= (fontsize / 2.0f);

        for(int i=0;i<context.size(); i++) {
            int decId = findUBOToUse(uboName, tmpS, {"bitmapfont.png"}, {sizeof(TextUBO)});

            poses.x += fontsize*2.2f;

            if(context[i] == ' ') {
                float xOnward = 0.0f;
                for(int j=i+1; j<context.size(); j++) {
                    if(context[j] == ' ') {
                        break;
                    }
                    else {
                        xOnward += fontsize*2.2f;
                    }
                }

                if(poses.x + xOnward >= xMostLeft) {
                    poses.y += fontsize*4.2f;
                    poses.x = pos.x - (fontsize*2.2f) - (fontsize / 2.0f);
                }
            }

            glm::mat4 model = calcModel(poses, glm::vec3(fontsize, fontsize*2.0f, fontsize), glm::vec3(0.0f));

            ubo.model = model;
            ubo.context = context[i];

            uniformBuffers[decId].bufferdata[0].data = &ubo;
            uniformBuffers[decId].updateUniformBuffer(stock()->devices, stock()->imageIndex);

            renderVBuffer("square", tmpS, decId, proj2d);
        }

    }

    void ResourceManager::deleteResourceManager()
    {
        deleteShaders();
        deleteVertexBuffers();
        deleteTextures();
        deleteUBOs();
    }
    void ResourceManager::deleteShaders()
    {
        for(auto r : shaders)
        {
            for(auto s : r.second)
            {
                s.second.deleteShader(stock()->devices);
            }
        }
    }
    void ResourceManager::deleteTextures()
    {
        for(auto t : textures)
        {
            t.second->deleteImage(stock()->devices);
            delete [] t.second;
        }
    }
    void ResourceManager::deleteUBOs()
    {
        for(int i=0; i<uniformBuffers.size(); i++) {
            uniformBuffers[i].deleteUniformBuffer(stock()->devices);
        }
    }
    void ResourceManager::deleteVertexBuffers()
    {
        for(auto v : vBuffers)
        {
            v.second->deleteVertexBuffer(stock()->devices);
            delete [] v.second;
        }
        vBuffers.clear();
    }
    void ResourceManager::deleteVertexBuffer(std::string name)
    {
        if(vBuffers.find(name) == vBuffers.end()) return;
        stock()->deleter.addToDelete(vBuffers[name][0], stock()->fBuffers["default"].size());
        vBuffers.erase(name);
    }
}
