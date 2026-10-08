#include "Animation.h"

namespace gll
{
    void Animation::createAnimation(const char* path)
    {
        shouldAnimate = true;
        std::vector<std::vector<float>>     tmpTime;
        std::vector<std::vector<glm::mat4>> tmpTransforms;

        std::string line;
        std::string full_path = path; full_path = "Resources/Meshes/" + full_path;
        std::ifstream file(full_path);

        if (!file.good()) std::cout << "cant open file : " << path << std::endl;

        while (getline(file, line))
        {
            int startOfLine = 0;
            if (Animation::findFirstChars(line, "<float_array", startOfLine)) {
                std::string toFind = Animation::findNextBrackets(line, startOfLine);
                if (findWordFromString(toFind, "pose_matrix-input-array")) {
                    tmpTime.push_back(readFloatFromFloatArray(line, startOfLine));
                }
                else if (findWordFromString(toFind, "pose_matrix-output-array")) {
                    tmpTransforms.push_back(readMat4FromFloatArray(line, startOfLine));
                }
            }
        }
        keyframes.resize(tmpTime.size());
        for (uint32_t i = 0; i < tmpTime.size(); i++) {
            keyframes[i].clear();
            for (uint32_t j = 0; j < tmpTime[i].size(); j++) {
                keyframes[i].push_back({ tmpTime[i][j], tmpTransforms[i][j] });
                currentKeyFrame.push_back(0);
                NextKeyFrame.push_back(1);
                currentFrameTime.push_back(0.0f);
            }
        }
    }
    std::vector<int> Animation::updateAnimation(std::vector<glm::mat4>& toReturn, Heir& heir, float deltaTime, glm::mat4 models, bool refresh)
    {
        toReturn.clear();
        if (refresh) resetAnimation();
        // start by increasing the time
        for (uint32_t i = 0;i < currentFrameTime.size();i++) {
            currentFrameTime[i] += deltaTime;
        }
        // get the pose models
        std::vector<int> finished = {};
        for (int i = 0; i < heir.amountOfBones; i++) {
            toReturn.push_back(findMatrix(i, finished));
        }
        setPoseMatrixes(heir, models, toReturn);
        return finished;
    }
    void Animation::resetAnimation()
    {
        for (uint32_t i = 0;i < currentFrameTime.size();i++) {
            currentFrameTime[i] = 0.0f;
            currentKeyFrame[i] = 0;
            NextKeyFrame[i] = 1;
        }
    }

    float Animation::findProgress(float last, float next, int who)
    {
        float timeTaken = next - last;
        return (currentFrameTime[who] - last) / timeTaken;
    }
    glm::mat4 Animation::interpolateMatrix(glm::mat4 Last, glm::mat4 Next, float progress)
    {
        glm::mat4 toReturn = glm::mat4(1.0f);
        for (int y = 0;y < 4;y++) {
            for (int x = 0;x < 4;x++) {
                toReturn[y][x] = Last[y][x] + (Next[y][x] - Last[y][x]) * progress;
            }
        }
        return toReturn;
    }
    glm::mat4 Animation::findMatrix(int who, std::vector<int>& finished)
    {
        while(currentFrameTime[who] >= keyframes[who][NextKeyFrame[who]].time)
            updateTime(who, finished);

        glm::mat4 last = keyframes[who][currentKeyFrame[who]].models;
        glm::mat4 next = keyframes[who][NextKeyFrame[who]].models;
        float prog = findProgress(keyframes[who][currentKeyFrame[who]].time, keyframes[who][NextKeyFrame[who]].time, who);
        return interpolateMatrix(last, next, prog);
    }
    void Animation::updateTime(int who, std::vector<int>& finished)
    {
        currentKeyFrame[who] += 1; NextKeyFrame[who] += 1;

        if (NextKeyFrame[who] == keyframes[who].size()) {
            currentFrameTime[who] -= keyframes[who][keyframes[who].size() - 1].time;
            currentKeyFrame[who] = 0;
            NextKeyFrame[who] = 1;
        }

        finished.push_back(currentKeyFrame[who]);
    }
    void Animation::setPoseMatrixes(Heir& heir, glm::mat4 parentMatrix, std::vector<glm::mat4>& poses)
    {
        glm::mat4 currentTransform = parentMatrix * poses[heir.parent.id];
        for (uint32_t i = 0;i < heir.child.size();i++) {
            setPoseMatrixes(heir.child[i], currentTransform, poses);
        }
        currentTransform = currentTransform * heir.parent.inverseTransform;
        heir.parent.poseModels = currentTransform;
        poses[heir.parent.id] = currentTransform;
    }

    bool Animation::findWordFromString(std::string& line, std::string toSep)
    {
        uint32_t found = 0;
        for (uint32_t i = 0;i < line.size();i++) {
            if (line[i] == toSep[found]) {
                found++;
                if (found == toSep.size())
                    return true;
            }
            else if (line[i] != toSep[found] && found > 1) return false;
        }
        return false;
    }
    bool Animation::findFirstChars(std::string line, std::string toFind, int& startOfLine)
    {
        int toStart;
        uint32_t tofindSize = 0;
        for (uint32_t i = 0; i < line.size(); i++) {
            if (line[i] != ' ') {
                toStart = i; break;
            }
        }
        for (uint32_t i = toStart;i < line.size();i++) {
            if (line[i] == toFind[tofindSize]) {
                tofindSize++;
                if (tofindSize == toFind.size())
                    return true;
            }
            else return false;
        }
        return false;
    }
    std::string Animation::findNextBrackets(std::string line, int& start, bool takeI)
    {
        int toStart = Animation::whereToBegin(line, '"', start);
        std::string toReturn;

        for (uint32_t i = toStart + 1; i < line.size(); i++) {
            if (line[i] == '"') {
                if (takeI)start = i + 1;
                else start = toStart;
                return toReturn;
            }
            toReturn += line[i];
        }

        return "";
    }
    int Animation::whereToBegin(std::string line, char toFind, int start)
    {
        for (uint32_t i = start; i < line.size(); i++) {
            if (line[i] == toFind)
                return i;
        }
        return -1;
    }
    std::vector<Bone> Animation::readBoneFromNameArray(std::string line, int start)
    {
        std::vector<Bone> toReturn;
        int beginRead = Animation::whereToBegin(line, '>', start) + 1;
        Bone tmp; int tmp2 = 0;
        std::string read;
        for (uint32_t i = beginRead;i < line.size();i++)
        {
            if (line[i] == ' ' || line[i] == '<') {
                tmp = { tmp2, read };
                toReturn.push_back(tmp);
                read = ""; tmp2++;
                if (line[i] == '<') break;
            }
            else {
                read += line[i];
            }
        }
        return toReturn;
    }
    std::vector<float> Animation::readFloatFromFloatArray(std::string line, int start)
    {
        std::vector<float> toReturn;
        int beginRead = whereToBegin(line, '>', start) + 1;
        float tmp;
        std::string read;
        for (uint32_t i = beginRead;i < line.size();i++)
        {
            if (line[i] == ' ' || line[i] == '<') {
                tmp = atof(read.c_str());
                toReturn.push_back(tmp);
                read = "";
                if (line[i] == '<') break;
            }
            else {
                read += line[i];
            }
        }
        return toReturn;
    }
    std::vector<glm::mat4> Animation::readMat4FromFloatArray(std::string line, int start)
    {
        std::vector<glm::mat4> toReturn;
        int beginRead = whereToBegin(line, '>', start) + 1;
        int offsetX = 0, offsetY = 0; glm::mat4 tmp;
        std::string read;
        for (uint32_t i = beginRead;i < line.size();i++)
        {
            if (line[i] == ' ' || line[i] == '<') {
                tmp[offsetX][offsetY] = atof(read.c_str());
                read = "";
                offsetX++;
                if (offsetX == 4) {
                    offsetX = 0;
                    offsetY++;
                    if (offsetY == 4) {
                        toReturn.push_back(tmp);
                        offsetY = 0;
                    }
                }
                if (line[i] == '<') break;
            }
            else {
                read += line[i];
            }
        }
        return toReturn;
    }
};