#include "Engine.h"

namespace gll
{
    struct ModelUBO
    {
        glm::mat4 model;
        glm::vec4 color;
    };
    struct LightUBO
    {
        glm::vec4 position;
        glm::vec4 color;
    };
    struct AnimModelUBO
    {
        glm::mat4 model[25];
    };
    struct TextUBO
    {
        glm::mat4 model;
        glm::vec4 color;
        int context;
    };

    enum TEXT_LAYOUT
    {
        CENTERED,
        LEFT
    };

    class ResourceManager
    {
        std::map<std::string, VertexInput>  vertexInputs = {
            {"Vertex", {Vertex::getBindingDescription(), Vertex::getAttributeDescriptions() }}, {"AnimVertex", { AnimVertex::getBindingDescription(), AnimVertex::getAttributeDescriptions()} }
        };

        VkPushConstantRange viewMatricesPushConstants = {
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,   // VkShaderStageFlags     stageFlags
            0,                              // uint32_t               offset
            36 * sizeof( float )             // uint32_t               size
        };

        void deleteShaders();
        void deleteVertexBuffers();
        void deleteTextures();
        void deleteUBOs();

        int findUBOToUse(std::string& name, Shader* tmpS, std::vector<std::string> texts, std::vector<size_t> ubufferSizes);
        void renderVBuffer(std::string vBuffer, Shader* tmpS, int& decId, glm::mat4& proj);
    public:
        // renderpass name,   map of shaders
        std::map<std::string, std::map<std::string, Shader>> shaders;

        std::map<std::string, VertexBuffer*> vBuffers;
        std::map<std::string, Animation*> animations;
        std::map<std::string, Images*> textures;
        std::vector<Uniform> uniformBuffers;

        std::vector<LightUBO> lightinfos;

        glm::mat4 proj2d, proj, view;
        glm::vec3 viewPos;
        glm::vec2 mousePos;

        float maxX;
        float lastFrame, deltaTime;

        void createResourceManager();
        void deleteResourceManager();

        void restartUBOs();
        glm::mat4 calcModel(glm::vec3 pos, glm::vec3 size, glm::vec3 rot);
        void updateViewMatrices(Camera* camera, int width, int height);

        void renderSprite(std::string shader, std::string vBuffer, std::string texture, glm::vec3 pos, glm::vec3 size, glm::vec3 rot, glm::vec4 color, std::string currentRenderPass = "default");
        void renderMesh(std::string shader, std::string vBuffer, std::vector<std::string> texts, glm::vec3 pos, glm::vec3 size, glm::vec3 rot, glm::vec4 color, std::string currentRenderPass = "default", glm::mat4 outerModel = glm::mat4(1.0f));
        void renderAnimMesh(std::string shader, std::string vBuffer, std::vector<std::string> texts, std::vector<glm::mat4>& models, std::string currentRenderPass);
        void renderText(std::string context, glm::vec3 pos, float fontsize, glm::vec4 color, std::string currentRenderPass, TEXT_LAYOUT layout, float xMostLeft);

        Shader* findShader(std::string name, std::string renderPass);
        VertexBuffer* findVertexBuffer(std::string name); void deleteVertexBuffer(std::string name);
        Animation* findAnimation(std::string name);
        Images* findTexture(std::string name);
        int findUBO(std::string name);
    };

    extern ResourceManager resources;
}
