#include "FrameBuffer.h"

namespace gll
{
    struct KeyFrame
    {
        float time;
        glm::mat4 models;
    };
    struct Bone
    {
        int id;
        std::string name;
        glm::mat4 inverseTransform;
        glm::mat4 poseModels;

        Bone() {}
        Bone(int ID, std::string Name, glm::mat4 InverseTransform = glm::mat4(1.0f)) {
            id = ID;
            name = Name;
            inverseTransform = InverseTransform;
            poseModels = glm::mat4(1.0f);
        }
    };
    struct Heir
    {
        Bone parent;
        std::vector<Heir> child;
        int amountOfBones;
    };

    class Animation
    {
    public:
        std::vector<float>    currentFrameTime;
        std::vector<uint16_t> currentKeyFrame;
        std::vector<uint16_t> NextKeyFrame;
        bool shouldAnimate;

        static std::vector<Bone> readBoneFromNameArray(std::string line, int start);
        static bool findWordFromString(std::string& line, std::string toSep);
        static bool findFirstChars(std::string line, std::string toFind, int& startOfLine);
        static std::string findNextBrackets(std::string line, int& start, bool takeI = false);
        static int whereToBegin(std::string line, char toFind, int start = 0);
        static std::vector<float>     readFloatFromFloatArray(std::string line, int start);
        static std::vector<glm::mat4> readMat4FromFloatArray(std::string line, int start);

        float findProgress(float last, float next, int who);
        static glm::mat4 interpolateMatrix(glm::mat4 Last, glm::mat4 Next, float progress);
        glm::mat4 findMatrix(int who, std::vector<int>& finished);
        void setPoseMatrixes(Heir& heir, glm::mat4 parentMatrix, std::vector<glm::mat4>& poses);
        void updateTime(int who, std::vector<int>& finished);

        std::vector<std::vector<KeyFrame>> keyframes;

        void createAnimation(const char* path);

        std::vector<int> updateAnimation(std::vector<glm::mat4>& toReturn, Heir& heir, float deltaTime, glm::mat4 models = glm::mat4(1.0f), bool refresh = false);
        void resetAnimation();
    };
};