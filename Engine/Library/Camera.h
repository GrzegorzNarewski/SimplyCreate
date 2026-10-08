#include "Stock.h"

namespace gll
{
    const float YAW =   0.0f;
    const float PITCH = 0.0f;
    const float SPEED = 8.0f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 110.0f;
    const float GRAVITY = 5.0f;

    class Camera
    {
    public:
        glm::vec3 Position;
        glm::vec3 Front, Ahead;
        glm::vec3 Up;
        glm::vec3 Right, RightAhead;
        glm::vec3 WorldUp;

        float Yaw;
        float Pitch;

        bool firstMouse = true;
        float lastX = 800.0f / 2.0;
        float lastY = 600.0 / 2.0;

        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;

        bool freezeCameraMouseMovement = false;

        void createCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

        glm::mat4 GetViewMatrix();
        glm::mat4 GetGunMatrix();

        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

        void updateCameraVectors();
    };

    Camera* activeCamera();
    void setActiveCamera(Camera* camera);
}
