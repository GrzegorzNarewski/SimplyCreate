#include "VertexBuffer.h"

#pragma warning(disable : 4996)

namespace gll
{
    VertexBuffer::VertexBuffer(std::string Name, Devices &devices, VkCommandPool &commandPool)
    {
        name = Name;
        createVertexBuffer(devices, commandPool);
    }
    VertexBuffer::VertexBuffer(std::string Name, Devices &devices, VkCommandPool &commandPool, std::vector<Vertex>& points)
    {
        name = Name;
        vertices = points;
        indices.clear();
        for(int i=0; i<points.size(); i++) {
            indices.push_back(i);
        }
        createVertexBuffer(devices, commandPool);
    }
    VertexBuffer::VertexBuffer(std::string Name, Devices &devices, VkCommandPool &commandPool, std::vector<Vertex>& vetricesz, std::vector<uint32_t>& indicesz)
    {
        name = Name;
        vertices = vetricesz;
        indices = indicesz;
        if(vertices.size() != 0) {
            animVertices.clear();
            createVertexBuffer(devices, commandPool);
        }
    }
    VertexBuffer::VertexBuffer(std::string Name, std::string paths, Devices &devices, VkCommandPool &commandPool, bool collada)
    {
        name = Name;
        path = paths;
        if(!collada)
        {
            loadOBJ(paths);
            if(vertices.size() != 0) {
                calculateTangents(false); animVertices.clear();
                findindices();
                createVertexBuffer(devices, commandPool);
            }
        }
        else
        {
            loadColladaOBJ(paths); vertices.clear();
            createVertexBuffer(devices, commandPool);
        }
    }
    glm::vec3 VertexBuffer::findObjectSize()
    {
        glm::vec3 toReturn;
        glm::vec3 largest = glm::vec3(0.0f), smallest = glm::vec3(0.0f);
        for(int i=0;i<vertices.size();i++){
            if(largest.x < vertices[i].pos.x)
                largest.x = vertices[i].pos.x;
            else if(smallest.x > vertices[i].pos.x)
                smallest.x = vertices[i].pos.x;

            if(largest.y < vertices[i].pos.y)
                largest.y = vertices[i].pos.y;
            else if(smallest.y > vertices[i].pos.y)
                smallest.y = vertices[i].pos.y;

            if(largest.z < vertices[i].pos.z)
                largest.z = vertices[i].pos.z;
            else if(smallest.z > vertices[i].pos.z)
                smallest.z = vertices[i].pos.z;
        }
        toReturn.x = largest.x - smallest.x;
        toReturn.y = largest.y - smallest.y;
        toReturn.z = largest.z - smallest.z;
        return toReturn;
    }
    glm::vec3 VertexBuffer::findObjectPos()
    {
        glm::vec3 toReturn;
        glm::vec3 largest = glm::vec3(0.0f), smallest = glm::vec3(0.0f);
        for(int i=0;i<vertices.size();i++){
            if(largest.x < vertices[i].pos.x)
                largest.x = vertices[i].pos.x;
            else if(smallest.x > vertices[i].pos.x)
                smallest.x = vertices[i].pos.x;

            if(largest.y < vertices[i].pos.y)
                largest.y = vertices[i].pos.y;
            else if(smallest.y > vertices[i].pos.y)
                smallest.y = vertices[i].pos.y;

            if(largest.z < vertices[i].pos.z)
                largest.z = vertices[i].pos.z;
            else if(smallest.z > vertices[i].pos.z)
                smallest.z = vertices[i].pos.z;
        }
        toReturn.x = (largest.x + smallest.x) / 2.0f;
        toReturn.y = (largest.y + smallest.y) / 2.0f;
        toReturn.z = (largest.z + smallest.z) / 2.0f;
        return toReturn;
    }
    void VertexBuffer::deleteVertexBuffer(Devices &devices)
    {
        vertexBuffer.deleteBuffer(devices);
    }
    void VertexBuffer::createVertexBuffer(Devices &devices, VkCommandPool &commandPool)
    {
        VkDeviceSize bufferSize = 0;
        if(vertices.size() > 0) bufferSize = (sizeof(Vertex) * vertices.size()) + (sizeof(uint32_t) * indices.size());
        else if(animVertices.size() > 0) bufferSize = (sizeof(AnimVertex) * animVertices.size()) + (sizeof(uint32_t) * indices.size());

        vertexBuffer.createBuffer(devices, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT| VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if(vertices.size() > 0) {
            toSend tosent;
            tosent.vertices = vertices;
            tosent.indices = indices;
            useStagingBufferToCopyData(devices, vertexBuffer, tosent, commandPool, bufferSize, 0);
            //useStagingBufferToCopyData(devices, vertexBuffer, vertices, commandPool, bufferSize, 0);
            //useStagingBufferToCopyData(devices, vertexBuffer, indices, commandPool, (sizeof(uint16_t) * indices.size()), sizeof(Vertex) * vertices.size());
        }
        else if(animVertices.size() > 0) {
            toSendAnim tosent;
            tosent.vertices = animVertices;
            tosent.indices = indices;
            useStagingBufferToCopyData(devices, vertexBuffer, tosent, commandPool, bufferSize, 0);
            //useStagingBufferToCopyData(devices, vertexBuffer, animVertices, commandPool, bufferSize, 0);
            //useStagingBufferToCopyData(devices, vertexBuffer, indices, commandPool, (sizeof(uint16_t) * indices.size()), sizeof(AnimVertex) * animVertices.size());
        }
    }
    void VertexBuffer::bindVertexBuffer(VkCommandBuffer &commandbuffer)
    {
        VkDeviceSize offsets = 0;
        vkCmdBindVertexBuffers(commandbuffer, 0, 1, vertexBuffer.handel, &offsets);
    }
    void VertexBuffer::renderVertexBuffer(VkCommandBuffer &commandBuffer)
    {
        if(vertices.size() > 0) {
            vkCmdBindIndexBuffer(commandBuffer, vertexBuffer.handel[0], sizeof(Vertex) * vertices.size(), VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);
        }
        else if(animVertices.size() > 0) {
            vkCmdBindIndexBuffer(commandBuffer, vertexBuffer.handel[0], (sizeof(AnimVertex) * animVertices.size()), VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);
        }
    }
    bool VertexBuffer::loadOBJ(std::string paths)
    {
        path = paths;

        vertices.clear(); animVertices.clear();
        std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec2> temp_uvs;
        std::vector<glm::vec3> temp_normals;
        std::vector<glm::ivec2> temp_lineData;

        FILE* file = fopen(paths.c_str(), "r");
        if (file == NULL) {
            std::string err = "cant open file in path: " + path + " ?\n";
            printf(err.c_str() );
            getchar();
            return false;
        }

        while (1) {

            char lineHeader[128];
            int res = fscanf(file, "%s", lineHeader);
            if (res == EOF)
                break;

            if (strcmp(lineHeader, "v") == 0) {
                glm::vec3 vertex;
                fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
                temp_vertices.push_back(vertex);
            }
            else if (strcmp(lineHeader, "vt") == 0) {
                glm::vec2 uv;
                fscanf(file, "%f %f\n", &uv.x, &uv.y);
                uv.y = -uv.y;
                temp_uvs.push_back(uv);
            }
            else if (strcmp(lineHeader, "vn") == 0) {
                glm::vec3 normal;
                fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
                temp_normals.push_back(normal);
            }
            else if (strcmp(lineHeader, "l") == 0) {
                glm::ivec2 tmpss;
                fscanf(file, "%d %d\n", &tmpss.x, &tmpss.y);
                tmpss.x--; tmpss.y--;
                temp_lineData.push_back(tmpss);
                vertices.push_back({temp_vertices[tmpss.x], temp_vertices[tmpss.y], {0.0f, 0.0f}});
                vertices.push_back({temp_vertices[tmpss.y], temp_vertices[tmpss.y], {0.0f, 0.0f}});
            }
            else if (strcmp(lineHeader, "f") == 0) {
                std::string vertex1, vertex2, vertex3;
                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
                if (matches != 9) {
                    printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                    fclose(file);
                    return false;
                }
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
                uvIndices.push_back(uvIndex[0]);
                uvIndices.push_back(uvIndex[1]);
                uvIndices.push_back(uvIndex[2]);
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[1]);
                normalIndices.push_back(normalIndex[2]);
            }
            else {
                char stupidBuffer[1000];
                fgets(stupidBuffer, 1000, file);
            }

        }

        for (unsigned int i = 0; i < vertexIndices.size(); i++) {
            unsigned int vertexIndex = vertexIndices[i];
            unsigned int uvIndex = uvIndices[i];
            unsigned int normalIndex = normalIndices[i];

            glm::vec3 vertex = temp_vertices[vertexIndex - 1];
            glm::vec2 uv = temp_uvs[uvIndex - 1];
            glm::vec3 normal = temp_normals[normalIndex - 1];

            vertices.push_back({vertex, normal, uv});

        }
        fclose(file);

        return true;
    }
    void VertexBuffer::findindices(bool anim)
    {
        if(!anim)
        {
            indices.clear(); int id = -1;
            std::vector<IndicesFind> tmp_vertices;
            for(int i=0; i<vertices.size(); i++)
            {
                bool ifCounted = false;
                for(int j=0; j<tmp_vertices.size(); j++)
                {
                    if(tmp_vertices[j].v.pos == vertices[i].pos && tmp_vertices[j].v.uvs == vertices[i].uvs &&
                       tmp_vertices[j].v.nor == vertices[i].nor && tmp_vertices[j].v.tan == vertices[i].tan) {
                        indices.push_back(tmp_vertices[j].id);
                        ifCounted = true;
                        break;
                    }
                }
                if(!ifCounted) {
                    id++;
                    tmp_vertices.push_back({vertices[i], id});
                    indices.push_back(id);
                }
            }
            vertices.clear();
            for(int j=0; j<tmp_vertices.size(); j++)
            {
                vertices.push_back(tmp_vertices[j].v);
            }
        }
        else
        {
            indices.clear(); int id = -1;
            std::vector<IndicesAnimFind> tmp_vertices;
            for(int i=0; i<animVertices.size(); i++)
            {
                bool ifCounted = false;
                for(int j=0; j<tmp_vertices.size(); j++)
                {
                    if(tmp_vertices[j].v.pos == animVertices[i].pos && tmp_vertices[j].v.uvs == animVertices[i].uvs &&
                       tmp_vertices[j].v.nor == animVertices[i].nor && tmp_vertices[j].v.wei == animVertices[i].wei && tmp_vertices[j].v.joi == animVertices[i].joi) {
                        indices.push_back(tmp_vertices[j].id);
                        ifCounted = true;
                        break;
                    }
                }
                if(!ifCounted) {
                    id++;
                    tmp_vertices.push_back({animVertices[i], id});
                    indices.push_back(id);
                }
            }
            animVertices.clear();
            for(int j=0; j<tmp_vertices.size(); j++)
            {
                animVertices.push_back(tmp_vertices[j].v);
            }
        }
    }

    std::vector<glm::vec3> VertexBuffer::readVec3FromFloatArray(std::string line, int start)
    {
        std::vector<glm::vec3> toReturn;
        int beginRead = Animation::whereToBegin(line, '>', start) + 1;
        int offset = 0; glm::vec3 tmp;
        std::string read;
        for(int i=beginRead;i<line.size();i++)
        {
            if(line[i] == ' ' || line[i] == '<') {
                tmp[offset] = atof(read.c_str());
                offset++;
                if(offset == 3) {
                    toReturn.push_back(tmp);
                    offset = 0;
                }
                read = "";
                if(line[i] == '<') break;
            }
            else {
                read+= line[i];
            }
        }
        return toReturn;
    }
    std::vector<glm::vec2> VertexBuffer::readVec2FromFloatArray(std::string line, int start)
    {
        std::vector<glm::vec2> toReturn;
        int beginRead = Animation::whereToBegin(line, '>', start) + 1;
        int offset = 0; glm::vec2 tmp;
        std::string read;
        for(int i=beginRead;i<line.size();i++)
        {
            if(line[i] == ' ' || line[i] == '<') {
                tmp[offset] = atof(read.c_str());
                offset++;
                if(offset == 2) {
                    toReturn.push_back(tmp);
                    offset = 0;
                }
                read = "";
                if(line[i] == '<') break;
            }
            else {
                read+= line[i];
            }
        }
        return toReturn;
    }
    std::vector<int> VertexBuffer::readIntFromIntArray(std::string line, int start)
    {
        std::vector<int> toReturn;
        int beginRead = Animation::whereToBegin(line, '>', start) + 1;
        int tmp;
        std::string read;
        for(int i=beginRead;i<line.size();i++)
        {
            if(line[i] == ' ' || line[i] == '<') {
                tmp = atoi(read.c_str());
                toReturn.push_back(tmp);
                read = "";
                if(line[i] == '<') break;
            }
            else {
                read+= line[i];
            }
        }
        return toReturn;
    }
    int VertexBuffer::findLineSpaces(std::string line)
    {
        for(int i=0;i<line.size();i++) {
            if(line[i] != ' ') return i - 1;
        }
    }
    Heir VertexBuffer::createHier(Heir &hier, std::vector<int> &spaces, std::vector<Bone> &Bones, int ii)
    {
        hier.parent = Bones[ii];
        if(ii+1 >= Bones.size()) return hier;
        for(int i=ii+1; i<Bones.size(); i++) {
            if(spaces[i] == spaces[ii] + 2) {
                hier.child.push_back({Bones[i]});
                createHier(hier.child.back(), spaces, Bones, i);
            }
            if(spaces[i] <= spaces[ii]) break;
        }

        return hier;
    }
    void VertexBuffer::debugHier(Heir hier, std::string spaces)
    {
        std::cout<<spaces<<hier.parent.name<<" "<<hier.parent.id<<std::endl;
        for(int i=0; i<hier.child.size(); i++) {
            debugHier(hier.child[i], spaces + '|');
        }
    }
    void VertexBuffer::loadColladaOBJ(std::string paths)
    {
        path = paths;

        vertices.clear();
        animVertices.clear();
        indices.clear();

        std::vector<glm::vec3> tmpPositions;
        std::vector<glm::vec3> tmpNormals;
        std::vector<glm::vec2> tmpUvs;
        std::vector<glm::mat4> tmpInverseTransforms;
        std::vector<int>       tmpIndices;
        std::vector<float>     tmpWeights;
        std::vector<int>       tmpVCount;
        std::vector<int>       tmpBoneIndices;
        std::vector<int>       tmpHier;
        std::vector<Bone>      tmpBones;

        std::string line;
        std::ifstream file(path);

        int inGeom = 3;

        while (getline (file, line))
        {
            int startOfLine = 0;
            if(Animation::findFirstChars(line, "<float_array", startOfLine)) {
                std::string whatThe = Animation::findNextBrackets(line, startOfLine);
                if(Animation::findWordFromString(whatThe, "mesh-positions-array")) {
                    tmpPositions = readVec3FromFloatArray(line, startOfLine);
                }
                else if(Animation::findWordFromString(whatThe, "mesh-normals-array")) {
                    tmpNormals = readVec3FromFloatArray(line, startOfLine);
                }
                else if(Animation::findWordFromString(whatThe, "mesh-map-0-array")) {
                    tmpUvs = readVec2FromFloatArray(line, startOfLine);
                }
                else if(Animation::findWordFromString(whatThe, "skin-weights-array")) {
                    tmpWeights = Animation::readFloatFromFloatArray(line, startOfLine);
                }
                else if(Animation::findWordFromString(whatThe, "skin-bind_poses-array")) {
                    tmpInverseTransforms = Animation::readMat4FromFloatArray(line, startOfLine);
                }
            }
            else if(Animation::findFirstChars(line, "<p>", startOfLine)) {
                tmpIndices = readIntFromIntArray(line, startOfLine);
            }
            else if(Animation::findFirstChars(line, "<input ", startOfLine)) {
                if(Animation::findNextBrackets(line, startOfLine) == "COLOR") {
                    inGeom++;
                }
            }
            else if(Animation::findFirstChars(line, "<v>", startOfLine)) {
                tmpBoneIndices = readIntFromIntArray(line, startOfLine);
            }
            else if(Animation::findFirstChars(line, "<vcount>", startOfLine)) {
                tmpVCount = readIntFromIntArray(line, startOfLine);
            }
            else if(Animation::findFirstChars(line, "<Name_array ", startOfLine)) {
                std::string whatThe = Animation::findNextBrackets(line, startOfLine);
                if(Animation::findWordFromString(whatThe, "skin-joints-array")) {
                    tmpBones = Animation::readBoneFromNameArray(line, startOfLine);
                }
            }
            else if(Animation::findFirstChars(line, "<node id=", startOfLine)) {
                Animation::findNextBrackets(line, startOfLine, true); Animation::findNextBrackets(line, startOfLine, true); Animation::findNextBrackets(line, startOfLine, true);
                if(Animation::findNextBrackets(line, startOfLine) == "JOINT") {
                    tmpHier.push_back(findLineSpaces(line));
                }
            }
        }

        std::vector<tmpVertexAtt> vertexs; vertexs.resize(tmpPositions.size());
        for(int i=0; i<tmpPositions.size(); i++) {
            vertexs[i].Pos = tmpPositions[i];
            for(int j=0;j<10;j++) {
                vertexs[i].joi[j] = -1;
                vertexs[i].wei[j] = 0.0f;
            }
        }

        int where = 0;
        for(int i=0;i<tmpVCount.size();i++) {
            int howMany = tmpVCount[i]; int ii = 0, j = where;
            howMany *= 2;
            for(j;j<where+howMany;j+=2) {
                vertexs[i].joi[ii] = tmpBoneIndices[j];
                vertexs[i].wei[ii] = tmpWeights[tmpBoneIndices[j + 1]]; ii++;
            }
            where = j;
        }

        for(int i=0; i<tmpIndices.size(); i+=inGeom) {
            tmpUvs[tmpIndices[i + 2]].y = -tmpUvs[tmpIndices[i + 2]].y;
            animVertices.push_back({vertexs[tmpIndices[i]].Pos, tmpNormals[tmpIndices[i + 1]], tmpUvs[tmpIndices[i + 2]], vertexs[tmpIndices[i]].wei, vertexs[tmpIndices[i]].joi});
        }

        for(int i=0;i<tmpBones.size();i++) {
            tmpBones[i].inverseTransform = tmpInverseTransforms[i];
        }

        heir = createHier(heir, tmpHier, tmpBones, 0);
        heir.amountOfBones = tmpBones.size();
        //debugHier(heir, "");
        calculateTangents(true);
        findindices(true);

        file.close();
    }
    void VertexBuffer::calculateTangents(bool anim)
    {
        if(anim)
            for (int i = 0; i < animVertices.size() - 1; i += 3) {
                glm::vec3 tangent1, bitangent1;

                glm::vec3 edge1 = animVertices[i + 1].pos - animVertices[i].pos;
                glm::vec3 edge2 = animVertices[i + 2].pos - animVertices[i].pos;
                glm::vec2 deltaUV1 = animVertices[i + 1].uvs - animVertices[i].uvs;
                glm::vec2 deltaUV2 = animVertices[i + 2].uvs - animVertices[i].uvs;
                float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

                animVertices[i].tan = tangent1; //animVertices[i].bit = bitangent1;
                animVertices[i + 1].tan = tangent1; //animVertices[i + 1].bit = bitangent1;
                animVertices[i + 2].tan = tangent1; //animVertices[i + 2].bit = bitangent1;
            }
        else
            for (int i = 0; i < vertices.size() - 1; i += 3) {
                glm::vec3 tangent1, bitangent1;

                glm::vec3 edge1 = vertices[i + 1].pos - vertices[i].pos;
                glm::vec3 edge2 = vertices[i + 2].pos - vertices[i].pos;
                glm::vec2 deltaUV1 = vertices[i + 1].uvs - vertices[i].uvs;
                glm::vec2 deltaUV2 = vertices[i + 2].uvs - vertices[i].uvs;
                float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

                vertices[i].tan = tangent1; //vertices[i].bit = bitangent1;
                vertices[i + 1].tan = tangent1; //vertices[i + 1].bit = bitangent1;
                vertices[i + 2].tan = tangent1; //vertices[i + 2].bit = bitangent1;

            }
    }

    VkVertexInputBindingDescription Vertex::getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(float) * 11;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
    std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = 0;

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = sizeof(float) * 3;

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = sizeof(float) * 6;

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = sizeof(float) * 8;
        return attributeDescriptions;
    }

    VkVertexInputBindingDescription AnimVertex::getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(float) * 15 + sizeof(int) * 4;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
    std::vector<VkVertexInputAttributeDescription> AnimVertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(6);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = 0; //offsetof(AnimVertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = sizeof(float) * 3; //offsetof(AnimVertex, nor);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset =  sizeof(float) * 6; //offsetof(AnimVertex, uvs);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[3].offset =  sizeof(float) * 8; //offsetof(AnimVertex, wei);

        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SINT;
        attributeDescriptions[4].offset =  sizeof(float) * 12; // offsetof(AnimVertex, joi);

        attributeDescriptions[5].binding = 0;
        attributeDescriptions[5].location = 5;
        attributeDescriptions[5].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[5].offset =  sizeof(float) * 16;// offsetof(AnimVertex, tan);
        return attributeDescriptions;
    }
}
