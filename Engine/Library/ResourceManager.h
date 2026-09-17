#include "Input.h"

namespace gll
{
    struct Model2DUBO
    {
        glm::mat4 model;
        glm::vec4 color;
    };
    struct ModelUBO
    {
        glm::mat4 model;
        glm::mat4 view;
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
        glm::mat4 view;
    };
    struct TextUBO
    {
        glm::mat4 model;
        glm::vec4 color;
        int context;
        int texturesizex;
        int texturesizey;
        float LetterWidth;
    };

    struct Transform
    {
        glm::vec3 position;
        glm::vec3 size;
        glm::vec3 rotation;

        glm::mat4 outerModel;
        glm::mat4 model;

        Transform();
        glm::mat4 getModel();
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
            20 * sizeof( float )             // uint32_t               size
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
        std::vector<Data> fontData;

        std::vector<LightUBO> lightinfos;

        glm::mat4 proj2d, proj, view;
        glm::vec2 mousePos;

        float maxX;

        void createResourceManager();
        void deleteResourceManager();

        void restartUBOs();
        void updateViewMatrices(Camera* camera, int width, int height);

        void renderSprite(std::string shader, std::string vBuffer, std::string texture, glm::vec3 pos, glm::vec3 size, glm::vec3 rot, glm::vec4 color, std::string currentRenderPass = "default");
        void renderMesh(std::string shader, std::string vBuffer, std::vector<std::string> texts, glm::vec3 pos, glm::vec3 size, glm::vec3 rot, glm::vec4 color, std::string currentRenderPass = "default", glm::mat4 outerModel = glm::mat4(1.0f));
        void renderAnimMesh(std::string shader, std::string vBuffer, std::vector<std::string> texts, std::vector<glm::mat4>& models, std::string currentRenderPass);
        void renderText(std::string context, Transform transform, glm::vec4 color, TEXT_LAYOUT layout, bool displaceXMax, std::string currentRenderPass = "default");

        Shader* findShader(std::string name, std::string renderPass);
        VertexBuffer* findVertexBuffer(std::string name); void deleteVertexBuffer(std::string name);
        Animation* findAnimation(std::string name);
        Images* findTexture(std::string name);
        int findUBO(std::string name);
    };

    ResourceManager* rManager();
}
