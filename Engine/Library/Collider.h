#include "ResourceManager.h"

namespace gll
{
    struct collisionStruct
    {
        glm::vec3 pos;
        glm::vec3 normal;
        bool isCollision;
        float distance;
        int objectID;

        collisionStruct()
        {
            pos = glm::vec3(0.0f);
            normal = glm::vec3(0.0f);
            isCollision = false;
            distance = 0.0f;
            objectID = 0;
        }
    };

    class Collider
    {
    public:
        static bool findButtonCollisions(glm::vec2 pos, glm::vec2 size, glm::vec2 mousePos);
        static collisionStruct doCollisions(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, glm::mat4 model, glm::vec3 Dir, glm::vec3 Pos);
    };
}

