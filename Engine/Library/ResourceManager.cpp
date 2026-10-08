#include "ResourceManager.h"

namespace gll
{
    glm::mat4 calcModel(glm::vec3 pos, glm::vec3 size, glm::vec3 rot)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(model, glm::vec3(size.x, size.y, size.z));
        model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
        return model;
    }
    Transform::Transform()
    {
        position = glm::vec3(0.0f);
        size = glm::vec3(1.0f);
        rotation = glm::vec3(0.0f);
        outerModel = glm::mat4(1.0f);
        model = glm::mat4(1.0f);
    }
    glm::mat4 Transform::getModel()
    {
        model = calcModel(position, size, rotation);
        model *= outerModel;
        return model;
    }

    ResourceManager resources;
    ResourceManager* rManager()
    {
        return &resources;
    }

    void framebuffer_size_callback(GLFWwindow* p_window, int width, int height)
    {
        window()->width = width;
        window()->height = height;
        stock()->framebufferResized = true;
    }
    void mouse_callback(GLFWwindow* p_window, double xpos, double ypos)
    {
        window()->mousePos.x = (((xpos * 2.0f) / window()->width) - 1.0f) * ((float)window()->width / (float)window()->height);
        window()->mousePos.y = -(((ypos * 2.0f) / window()->height) - 1.0f);

        if (activeCamera() != nullptr) {
            if (!activeCamera()->freezeCameraMouseMovement) {
                activeCamera()->ProcessMouseMovement(xpos, ypos);
            }
        }
    }

    void ResourceManager::createResourceManager()
    {
        lightinfos.clear();
        lightinfos.push_back({});
        lightinfos[0].position = glm::vec4(2.5f, 3.0f, 0.1f, 1.0f);
        lightinfos[0].color = glm::vec4(1.0f);

        fontData = gll::FileManager::readFile("Textures/Fonts/Font1Data.csv");
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

        glm::vec4 camPos;
        if (camera != nullptr) {
            view = camera->GetViewMatrix();
            proj = glm::perspective(glm::radians(camera->Zoom), maxX, 0.001f, 1000.0f);
            camPos = glm::vec4(camera->Position, 1.0f);
        }
        else {
            camPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }

        proj2d = glm::ortho(-maxX, maxX, -1.0f, 1.0f, -100.0f, 100.0f);

        glm::vec2 tmpMP = window()->mousePos;
        mousePos.x = ((tmpMP.x / window()->width) * 2.0f) - 1.0f;
        mousePos.y = ((tmpMP.y / window()->height) * 2.0f) - 1.0f;

        for(auto r : shaders) {
            for(auto s : r.second) {
                //vkCmdPushConstants( stock()->cBuffers["default"].commandBuffers[stock()->currentFrame], s.second.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, 16 * sizeof(float), &view );
                vkCmdPushConstants( stock()->cBuffers["default"].commandBuffers[stock()->currentFrame], s.second.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 16 * sizeof(float), 4 * sizeof(float), &camPos);
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

        std::string path1 = "Resources/Shaders/compiled/" + name + "_vert.spv";
        std::string path2 = "Resources/Shaders/compiled/" + name + "_frag.spv";
        std::string path3 = "Resources/Shaders/compiled/" + name + "_geom.spv";

        std::string descript = "Buffer-Image", vInp = "Vertex";

        std::vector<File_Data> data = FileManager::readFilez("Shaders/details/" + name + ".save");

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

        vkCmdPushConstants(cbuff->commandBuffers[stock()->currentFrame], tmpS->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, 16 * sizeof(float), &proj);

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

    void ResourceManager::renderMesh(std::string shader, std::string vBuffer, std::vector<std::string> texts, Transform transform, glm::vec4 color, std::string currentRenderPass, std::map<std::string, void*> ubo_addons)
    {
	    Shader* tmpS = findShader(shader, currentRenderPass);

	    std::string uboName = "MODEL_SQUARE" + shader + " ";
	    for(int i=0; i<texts.size(); i++) { uboName += texts[i]; }
        std::vector<std::size_t> ubo_sizes = { sizeof(ModelUBO), sizeof(LightUBO) };
        for (auto& ubo_addon : ubo_addons) {
            ubo_sizes.push_back(sizeof(ubo_addon.second));
        }
        int decId = findUBOToUse(uboName, tmpS, texts, ubo_sizes);

        glm::mat4 model = transform.getModel();

        ModelUBO ubo;
        ubo.model = model;
        ubo.view = view;
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

        int kk = 2;
        for (auto& ubo_addon : ubo_addons) {
            uniformBuffers[decId].bufferdata[kk].data = &ubo_addon.second;
            kk++;
        }
        uniformBuffers[decId].updateUniformBuffer(stock()->devices, stock()->imageIndex);

        renderVBuffer(vBuffer, tmpS, decId, proj);
    }
    void ResourceManager::renderAnimMesh(std::string shader, std::string vBuffer, std::vector<std::string> texts, std::vector<glm::mat4>& models, glm::vec4 color, std::string currentRenderPass, std::map<std::string, void*> ubo_addons)
    {
	    Shader* tmpS = findShader(shader, currentRenderPass);

	    std::string uboName = "MODEL_SQUARE" + shader + " ";
	    for(int i=0; i<texts.size(); i++) { uboName += texts[i]; }
        std::vector<std::size_t> ubo_sizes = { sizeof(AnimModelUBO), sizeof(LightUBO) };
        for (auto& ubo_addon : ubo_addons) {
            ubo_sizes.push_back(sizeof(ubo_addon.second));
        }
        int decId = findUBOToUse(uboName, tmpS, texts, ubo_sizes);

        AnimModelUBO ubo;
        for(int i=0; i<models.size(); i++) {
            ubo.model[i] = models[i];
        }
        ubo.view = view;
        ubo.color = color;
        uniformBuffers[decId].bufferdata[0].data = &ubo;

        LightUBO ubo2;
        if(lightinfos.size() > 0) {
            ubo2 = lightinfos[0];
        }
        uniformBuffers[decId].bufferdata[1].data = &ubo2;

        int kk = 2;
        for (auto& ubo_addon : ubo_addons) {
            uniformBuffers[decId].bufferdata[kk].data = &ubo_addon.second;
            kk++;
        }
        uniformBuffers[decId].updateUniformBuffer(stock()->devices, stock()->imageIndex);

        renderVBuffer(vBuffer, tmpS, decId, proj);
    }
    void ResourceManager::renderSprite(std::string shader, std::string vBuffer, std::string texture, Transform& transform, glm::vec4 color, std::string currentRenderPass, std::map<std::string, void*> ubo_addons)
    {
	    Shader* tmpS = findShader(shader, currentRenderPass);

	    std::string uboName = "MODEL_SQUARE" + shader + " " + texture;
        std::vector<std::size_t> ubo_sizes = { sizeof(Model2DUBO) };
        for (auto& ubo_addon : ubo_addons) {
            ubo_sizes.push_back(sizeof(ubo_addon.second));
        }
        int decId = findUBOToUse(uboName, tmpS, { texture }, ubo_sizes);

        glm::mat4 model = transform.getModel();

        Model2DUBO ubo;
        ubo.model = model;
        ubo.color = color;

        uniformBuffers[decId].bufferdata[0].data = &ubo;
        int kk = 1;
        for (auto& ubo_addon : ubo_addons) {
            uniformBuffers[decId].bufferdata[kk].data = ubo_addon.second;
            kk++;
        }
        uniformBuffers[decId].updateUniformBuffer(stock()->devices, stock()->imageIndex);

        renderVBuffer(vBuffer, tmpS, decId, proj2d);
    }
    Data* findData(std::string name)
    {
        for (int i = 0; i < rManager()->fontData.size(); i++) {
            if (rManager()->fontData[i].name == name) {
                return &rManager()->fontData[i];
            }
        }
        return &rManager()->fontData[0];
    }
    float ResourceManager::findTextSize(Transform transform, std::string context)
    {
        Transform tmp = transform;
        float leftMost = 0.0f;

        float cellSizex = std::atof(findData("Cell Width")->data.c_str());
        float previousSize = 0.0f;
        float fullSize = 0.0f;
        for (int i = 0; i < context.size(); i++) {
            float letterWidth = std::atof(findData("Char " + std::to_string((int)context[i]) + " Base Width")->data.c_str());

            transform.size.x = tmp.size.x * (letterWidth / cellSizex);
            transform.position.x += (previousSize + transform.size.x);
            if (i == 0) leftMost = transform.position.x - (transform.size.x);
            previousSize = transform.size.x;

            fullSize += transform.size.x;
        }
        float rightMost = transform.position.x + (transform.size.x);
        float toReturn = (rightMost - leftMost) / 2.0f;

        return toReturn;
    }
    void ResourceManager::renderText(std::string context, Transform transform, glm::vec4 color, TEXT_LAYOUT layout, std::string currentRenderPass, std::map<std::string, void*> ubo_addons)
    {
	    Shader* tmpS = findShader("text", currentRenderPass);

	    std::string uboName = "MODEL_SQUAREtext bitmapfont.png";

        TextUBO ubo;

        float cellSizex = std::atof(findData("Cell Width")->data.c_str());

        if (layout == CENTERED) {
            float textcenterpos = findTextSize(transform, context);
            transform.position.x -= textcenterpos;
        }
        float previousSize = 0.0f;
        float originalSize = transform.size.x;
        for (int i = 0; i < context.size(); i++) {
            std::vector<std::size_t> ubo_sizes = { sizeof(Model2DUBO) };
            for (auto& ubo_addon : ubo_addons) {
                ubo_sizes.push_back(sizeof(ubo_addon.second));
            }
            int decId = findUBOToUse(uboName, tmpS, { "Fonts/Font1.png" }, { sizeof(TextUBO) });

            float letterWidth = std::atof(findData("Char " + std::to_string((int)context[i]) + " Base Width")->data.c_str());

            transform.size.x = originalSize * (letterWidth / cellSizex);
            transform.position.x += (previousSize + transform.size.x);
            previousSize = transform.size.x;

            ubo.model = transform.getModel();
            ubo.context = context[i];
            ubo.color = color;
            ubo.texturesizex = std::atof(findData("Image Width")->data.c_str());
            ubo.texturesizey = std::atof(findData("Image Height")->data.c_str());
            ubo.LetterWidth = letterWidth;

            uniformBuffers[decId].bufferdata[0].data = &ubo;
            int kk = 1;
            for (auto& ubo_addon : ubo_addons) {
                uniformBuffers[decId].bufferdata[kk].data = &ubo_addon.second;
                kk++;
            }
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
