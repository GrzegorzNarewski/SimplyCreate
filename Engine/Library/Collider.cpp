#include "Collider.h"

namespace gll
{
    collisionStruct Collider::doCollisions(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, glm::mat4 model, glm::vec3 Dir, glm::vec3 Pos)
    {
        double vv_0, vv_1, vv_v, kat_0, kat_1, kat_2;
        glm::vec3 v0, v1, v2, imp_v_0, imp_v_1;
        collisionStruct toReturn;
        glm::vec3 verDir1, verDir2, VNorm;
        float dst = 0.0f;
        for (unsigned int i = 0; i < indices.size(); i += 3)
        {
            v0 = model * glm::vec4(vertices[indices[i]].pos, 1.0f);
            v1 = model * glm::vec4(vertices[indices[i + 1]].pos, 1.0f);
            v2 = model * glm::vec4(vertices[indices[i + 2]].pos, 1.0f);

            verDir1 = v1 - v0;
            verDir2 = v2 - v0;

            // normal vector ?
            VNorm.x = (verDir1.y * verDir2.z) - (verDir1.z * verDir2.y);
            VNorm.y = (verDir1.z * verDir2.x) - (verDir1.x * verDir2.z);
            VNorm.z = (verDir1.x * verDir2.y) - (verDir1.y * verDir2.x);

            vv_0 = (-(VNorm.x * Pos.x) + (VNorm.x * v0.x) - (VNorm.y * Pos.y) +
                (VNorm.y * v0.y) - (VNorm.z * Pos.z) + (VNorm.z * v0.z));
            vv_1 = (VNorm.x * Dir.x + VNorm.y * Dir.y + VNorm.z * Dir.z);
            vv_v = vv_0 / vv_1;

            if (vv_v >= 0.0)
            {
                glm::vec3 imp_0 = glm::dvec3(Pos.x + Dir.x * vv_v, Pos.y + Dir.y * vv_v, Pos.z + Dir.z * vv_v); // impact
                //maths find if impact is inside triangle using angles
                imp_v_0 = glm::normalize(imp_0 - v0);
                imp_v_1 = glm::normalize(imp_0 - v1);
                kat_0 = glm::acos(glm::dot(imp_v_0, imp_v_1));

                imp_v_0 = glm::normalize(imp_0 - v0);
                imp_v_1 = glm::normalize(imp_0 - v2);
                kat_1 = glm::acos(glm::dot(imp_v_0, imp_v_1));

                imp_v_0 = glm::normalize(imp_0 - v1);
                imp_v_1 = glm::normalize(imp_0 - v2);
                kat_2 = glm::acos(glm::dot(imp_v_0, imp_v_1));

                if (glm::abs(kat_0 + kat_1 + kat_2 - 2.0 * Pi) < 0.001)
                {
                    if (!toReturn.isCollision)
                    {
                        toReturn.pos = imp_0;
                        //colid.id = { vex[ind[i]].id, 0 };
                        toReturn.distance = glm::distance(Pos, imp_0);
                        toReturn.isCollision = true;
                        toReturn.normal = VNorm;
                    }
                    else
                    {
                        dst = glm::distance(Pos, imp_0);
                        if (toReturn.distance > dst)
                        {
                            toReturn.pos = imp_0;
                            toReturn.distance = dst;
                            toReturn.normal = VNorm;
                            //colid.id = { vex[ind[i]].id, 0 };
                        }
                    }
                }
            }
        }
        return toReturn;
    }

    bool Collider::findButtonCollisions(glm::vec2 pos, glm::vec2 size, glm::vec2 mousePos)
    {
        if(glfwGetInputMode(window()->handel, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) return false;

        glm::vec4 poses[4] = {
            resources.proj2d * glm::vec4(pos + glm::vec2(-size.x, size.y), 0.0f, 1.0f), resources.proj2d * glm::vec4(pos + glm::vec2(size.x, size.y), 0.0f, 1.0f),
            resources.proj2d * glm::vec4(pos + glm::vec2(-size.x,-size.y), 0.0f, 1.0f), resources.proj2d * glm::vec4(pos + glm::vec2(size.x,-size.y), 0.0f, 1.0f),
        };
        if (mousePos.x > poses[0].x && mousePos.x < poses[1].x &&
            mousePos.y > poses[2].y && mousePos.y < poses[1].y) {
            return true;
        }
        return false;
    }
}
