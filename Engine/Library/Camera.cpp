#include "Camera.h"

namespace gll
{
    void Camera::createCamera(glm::vec3 position, float yaw, float pitch)
    {
        Position = position;
        WorldUp = glm::vec3(0.0f, 0.0f, -1.0f);
        Yaw = yaw;
        Pitch = pitch;
        MovementSpeed = SPEED;
        MouseSensitivity = SENSITIVITY;
        Zoom = ZOOM;
        updateCameraVectors();

        stamina = 0.0f;
        maxStamina = 100.0f;
        staminaRegen = 10.0f;
        staminaUsage = 15.0f;
    }

    glm::mat4 Camera::GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }
    glm::mat4 Camera::GetGunMatrix()
    {
        return glm::mat4(
            glm::vec4(-Front, 0.0f),
            glm::vec4(Up, 0.0f),
            glm::vec4(Right, 0.0f),
            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
        );
    }

    void Camera::ProcessMouseMovement(float xposIn, float yposIn, GLboolean constrainPitch)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        lastX = xpos;
        lastY = ypos;

        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }
        updateCameraVectors();
    }

    void Camera::ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 120.0f)
            Zoom = 120.0f;
    }

    void Camera::updateCameraVectors()
    {
        glm::vec3 front;
        front.x = (cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)));
        front.y = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.z = sin(glm::radians(Pitch));
        Front = glm::normalize(front);

        Ahead.x = (cos(glm::radians(Yaw)) * cos(glm::radians(0.0f)));
        Ahead.y = sin(glm::radians(Yaw)) * cos(glm::radians(0.0f));
        Ahead.z = sin(glm::radians(0.0f));
        Ahead = glm::normalize(Ahead);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        RightAhead = glm::normalize(glm::cross(Ahead, WorldUp));

        Up = glm::normalize(glm::cross(Right, Front));
    }

    void Camera::ProcessWalking(Window* window, float deltaTime)
    {
        /*float speed = MovementSpeed;
        int sprinting = 0;
        if(window->getKey(GLFW_KEY_LEFT_CONTROL) == KEY_PRESS) {
            sprinting = 1;
        }
        else if(window->getKey(GLFW_KEY_LEFT_SHIFT) == KEY_PRESS) {
            speed /= 2.0f;
        }

        if(window->getKey(GLFW_KEY_W) == KEY_PRESS) {
            if(sprinting == 1 && stamina > 0.0f) {
                stamina -= staminaUsage * deltaTime;
                speed *= 2.0f;
                sprinting = 2;
                staminaRegenTimer = 2.0f;
            }
            Position += Ahead * speed * deltaTime;
        }
        if(window->getKey(GLFW_KEY_S) == KEY_PRESS) {
            Position -= Ahead * speed * deltaTime;
        }
        if(window->getKey(GLFW_KEY_A) == KEY_PRESS) {
            Position -= RightAhead * speed * deltaTime;
        }
        if(window->getKey(GLFW_KEY_D) == KEY_PRESS) {
            Position += RightAhead * speed * deltaTime;
        }

        if(sprinting != 2) {
            if(stamina < maxStamina && staminaRegenTimer <= 0.0f) {
                stamina += staminaRegen * deltaTime;
                if(stamina > maxStamina) stamina = maxStamina;
            }
            else {
                staminaRegenTimer -= deltaTime;
            }
        }

        Position.z -= GRAVITY * deltaTime;*/
    }
    void Camera::ProcessTopDownWalking(Window* window, float deltaTime)
    {
     /*   if (window->getKey(GLFW_KEY_W) == KEY_PRESS) {
            Position += Ahead * MovementSpeed * deltaTime;
        }
        if(window->getKey(GLFW_KEY_S) == KEY_PRESS) {
            Position -= Ahead * MovementSpeed * deltaTime;
        }
        if(window->getKey(GLFW_KEY_D) == KEY_PRESS) {
            Position += RightAhead * MovementSpeed * deltaTime;
        }
        if(window->getKey(GLFW_KEY_A) == KEY_PRESS) {
            Position -= RightAhead * MovementSpeed * deltaTime;
        }
*/
        /*int count;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);

        if(count >= 2) {
            if(glm::round(axes[1]) == -1.0f) {
                Position += Ahead * MovementSpeed * deltaTime;
            }
            if(glm::round(axes[1]) == 1.0f) {
                Position -= Ahead * MovementSpeed * deltaTime;
            }
            if(glm::round(axes[0]) == 1.0f) {
                Position += RightAhead * MovementSpeed * deltaTime;
            }
            if(glm::round(axes[0]) == -1.0f) {
                Position -= RightAhead * MovementSpeed * deltaTime;
            }
        }*/
    }
}
