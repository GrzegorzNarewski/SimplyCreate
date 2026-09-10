#include "Animation.h"

namespace gll
{
    struct pathVertices
    {
        glm::vec3 Pos; int id;

        float globalGoal; // the distance to the goal from this point
        float localGoal; // the amount of steps it took to get to this point
        int parentID; // the id of the parent that got here

        bool visited; // has already been checked
        bool isWall; // can it be walked through

        std::vector<int> neighbours;

        pathVertices()
        {
            Pos = glm::vec3(0.0f);
            id = 0;
            globalGoal = 100000000;
            localGoal = 100000000;
            parentID = 0;
            visited = false;
            isWall = false;
            neighbours.clear();
        }
    };

    struct tmpVertexAtt
    {
        glm::vec3 Pos;
        float wei[10];
        int   joi[10];
    };
    struct IndicesFind
    {
        Vertex v;
        int id;
    };
    struct IndicesAnimFind
    {
        AnimVertex v;
        int id;
    };

    class VertexBuffer
    {
        bool loadOBJ(std::string);
        void findindices(bool anim = false);
        void calculateTangents(bool anim = false);
        // -- anim loader
        int findLineSpaces(std::string line);
        std::vector<glm::vec3> readVec3FromFloatArray(std::string line, int start);
        std::vector<glm::vec2> readVec2FromFloatArray(std::string line, int start);
        std::vector<int>       readIntFromIntArray(std::string line, int start);
        Heir createHier(Heir &hier, std::vector<int> &spaces, std::vector<Bone> &Bones, int ii);
        void debugHier(Heir hier, std::string spaces);
        void loadColladaOBJ(std::string path);
    public:
        Buffer vertexBuffer;
        std::vector<Vertex> vertices = {
            {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        };
        std::vector<AnimVertex> animVertices;
        std::vector<uint32_t> indices = {
            0, 1, 2, 2, 3, 0
        };
        std::vector<pathVertices> pathsVertices;

        std::string name;
        std::string path;

        Heir heir;

        VertexBuffer() {};
        VertexBuffer(std::string Name, Devices &devices, VkCommandPool &commandPool);
        VertexBuffer(std::string name, Devices &devices, VkCommandPool &commandPool, std::vector<Vertex>& points);
        VertexBuffer(std::string name, Devices &devices, VkCommandPool &commandPool, std::vector<Vertex>& points, std::vector<uint32_t>& indices);
        VertexBuffer(std::string Name, std::string path, Devices &devices, VkCommandPool &commandPool, bool collada = false);
        void createVertexBuffer(Devices &devices, VkCommandPool &commandPool);

        void deleteVertexBuffer(Devices &devices);

        void renderVertexBuffer(VkCommandBuffer &commandbuffer);

        glm::vec3 findObjectSize();
        glm::vec3 findObjectPos();

        void bindVertexBuffer(VkCommandBuffer &commandbuffer);
    };
}
