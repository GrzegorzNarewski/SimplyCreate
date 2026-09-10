#include "FrameBuffer.h"

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
    std::map<std::string, float>    currentFrameTime;
    std::map<std::string, int32_t> currentKeyFrame;
    std::map<std::string, int32_t> NextKeyFrame;
    std::vector<Bone>      Bones;
    bool shouldAnimate;

    static std::vector<Bone> readBoneFromNameArray(std::string line, int start);
    static bool findWordFromString(std::string &line, std::string toSep);
    static bool findFirstChars(std::string line, std::string toFind, int &startOfLine);
    static std::string findNextBrackets(std::string line, int &start, bool takeI = false);
    static int whereToBegin(std::string line, char toFind, int start = 0);
    static std::vector<float>     readFloatFromFloatArray(std::string line, int start);
    static std::vector<glm::mat4> readMat4FromFloatArray(std::string line, int start);
    bool findNameFromString(std::string &line, std::string Name);

    float findProgress(float last, float next, std::string who);
    static glm::mat4 interpolateMatrix(glm::mat4 Last, glm::mat4 Next, float progress);
    glm::mat4 findMatrix(int who, int &finished, int dir);
    void setPoseMatrixes(Heir &heir, glm::mat4 parentMatrix, std::vector<glm::mat4> &poses);
    void updateTime(std::string who, int &finished, int dir);

    std::map<std::string, std::vector<KeyFrame>> keyframes;
    int prevDir = 1;

    void createAnimation(const char* path);

    int updateAnimation(std::vector<glm::mat4> &toReturn, Heir &heir, float& deltaTime, glm::mat4 models, bool refresh, float dir);
    void resetAnimation();
};

