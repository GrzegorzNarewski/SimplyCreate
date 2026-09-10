#include "Animation.h"

void Animation::createAnimation(const char* path)
{
    shouldAnimate = true;
    std::map<std::string, std::vector<float>>     tmpTime;
    std::map<std::string, std::vector<glm::mat4>> tmpTransforms;

    std::string line;
    std::string paths = "Resources/Meshes/";
    paths += path;
    std::ifstream file(paths.c_str());

    if(!file.good()) std::cout<<"cant open file : " << path << std::endl;

    while (getline (file, line))
    {
        int startOfLine = 0;
        if(Animation::findFirstChars(line, "<float_array", startOfLine)) {
            std::string toFind = Animation::findNextBrackets(line, startOfLine);
            if(findWordFromString(toFind, "pose_matrix-input-array")) {
                for(int i=0; i<Bones.size(); i++) {
                    if(findNameFromString(toFind, Bones[i].name)) {
                        tmpTime[Bones[i].name] = readFloatFromFloatArray(line, startOfLine);
                    }
                }
            }
            else if(findWordFromString(toFind, "pose_matrix-output-array")) {
                for(int i=0; i<Bones.size(); i++) {
                    if(findNameFromString(toFind, Bones[i].name)) {
                        tmpTransforms[Bones[i].name] = readMat4FromFloatArray(line, startOfLine);
                    }
                }
            }
        }
        else if(Animation::findFirstChars(line, "<Name_array ", startOfLine)) {
            std::string whatThe = Animation::findNextBrackets(line, startOfLine);
            if(Animation::findWordFromString(whatThe, "skin-joints-array")) {
                Bones = Animation::readBoneFromNameArray(line, startOfLine);
            }
        }
    }

    for(auto frame : tmpTime)
    {
        keyframes[frame.first] = {};
        for(uint32_t j=0; j<frame.second.size(); j++) {
            keyframes[frame.first].push_back({frame.second[j], tmpTransforms[frame.first][j]});
        }
        currentKeyFrame[frame.first] = 0;
        NextKeyFrame[frame.first] = 1;
        currentFrameTime[frame.first] = 0.0f;
    }
}
int Animation::updateAnimation(std::vector<glm::mat4> &toReturn, Heir &heir, float& deltaTime, glm::mat4 models, bool refresh, float dir)
{
    toReturn.clear();

    // reset the animation if needed
    if(refresh) resetAnimation();

    // check if the direction changed
    if(prevDir == 1 && dir == -1) {
        for(auto &timez : currentFrameTime) {
            int tmp = NextKeyFrame[timez.first];
            NextKeyFrame[timez.first] = currentKeyFrame[timez.first];
            currentKeyFrame[timez.first] = tmp;
        }
        prevDir = -1;
    }
    if(prevDir == -1 && dir == 1) {
        for(auto &timez : currentFrameTime) {
            int tmp = NextKeyFrame[timez.first];
            NextKeyFrame[timez.first] = currentKeyFrame[timez.first];
            currentKeyFrame[timez.first] = tmp;
        }
        prevDir = 1;
    }

    // get the pose models
    int finished = -1;
    toReturn.resize(heir.amountOfBones);
    for(int i=0; i<heir.amountOfBones; i++) {
        toReturn[i] = findMatrix(i, finished, dir);
    }
    setPoseMatrixes(heir, models, toReturn);

    // end by increasing the time
    for(auto &time : currentFrameTime) {
        time.second += deltaTime * dir;
    }

    return finished;
}
void Animation::resetAnimation()
{
    for(auto time : currentFrameTime) {
        time.second = 0.0f;
        currentKeyFrame[time.first] = 0;
        NextKeyFrame[time.first] = 1;
    }
}

float Animation::findProgress(float last, float next, std::string who)
{
    float timeTaken = next - last;
    if(timeTaken == 0.0f) timeTaken = 0.001f;

    float diff = currentFrameTime[who] - last;
    if(diff == 0.0f) diff = 0.001f;

    float prog = (diff) / timeTaken;
    return prog;
}
glm::mat4 Animation::interpolateMatrix(glm::mat4 Last, glm::mat4 Next, float progress)
{
    glm::mat4 toReturn = glm::mat4(1.0f);
    for(int y=0;y<4;y++) {
        for(int x=0;x<4;x++) {
            float diff = Next[y][x] - Last[y][x];
            if(diff == 0.0f) diff = 0.001f;
            toReturn[y][x] = Last[y][x] + (diff) * progress;
        }
    }
    return toReturn;
}
glm::mat4 Animation::findMatrix(int whos, int &finished, int dir)
{
    std::string who = "";
    for(int i = 0; i<Bones.size(); i++)
    {
        if(Bones[i].id == whos && currentFrameTime.find(Bones[i].name) == currentFrameTime.end()) {
            return glm::mat4(1.0f);
        }
        else if(Bones[i].id == whos && currentFrameTime.find(Bones[i].name) != currentFrameTime.end()) {
            who = Bones[i].name;
            break;
        }
    }

    if(currentFrameTime[who] >= keyframes[who][NextKeyFrame[who]].time && dir == 1)
        updateTime(who, finished, 1);
    if(currentFrameTime[who] <= keyframes[who][NextKeyFrame[who]].time && dir == -1)
        updateTime(who, finished, -1);

    glm::mat4 next = keyframes[who][NextKeyFrame[who]].models;
    glm::mat4 last = keyframes[who][currentKeyFrame[who]].models;

    float prog = findProgress(keyframes[who][currentKeyFrame[who]].time, keyframes[who][NextKeyFrame[who]].time, who);
    return interpolateMatrix(last, next, prog);
}
void Animation::updateTime(std::string who, int &finished, int dir)
{
    currentKeyFrame[who] += dir; NextKeyFrame[who] += dir;

    if(NextKeyFrame[who] == keyframes[who].size()) {
        currentFrameTime[who] = 0.0f;
        currentKeyFrame[who] = 0;
        NextKeyFrame[who] = 1;
        finished = 1;
    }
    else if(NextKeyFrame[who] == -1) {
        currentFrameTime[who] = keyframes[who].back().time;
        currentKeyFrame[who] = keyframes[who].size() - 1;
        NextKeyFrame[who] = keyframes[who].size() - 2;
        finished = 1;
    }
}
void Animation::setPoseMatrixes(Heir &heir, glm::mat4 parentMatrix, std::vector<glm::mat4> &poses)
{
    glm::mat4 currentTransform = parentMatrix * poses[heir.parent.id];
    for(uint32_t i=0;i<heir.child.size();i++) {
        setPoseMatrixes(heir.child[i], currentTransform, poses);
    }
    currentTransform = currentTransform * heir.parent.inverseTransform;
    heir.parent.poseModels = currentTransform;
    poses[heir.parent.id] = currentTransform;
}

bool Animation::findNameFromString(std::string &line, std::string Name)
{
    std::string word = "";
    bool writing = false;
    for(int i=0; i<line.size(); i++) {
        if(!writing) {
            if(line[i] == '_') {
                writing = true;
            }
        }
        else {
            if(line[i] == '_') {
                if(word == Name) {
                    return true;
                }
                word = "";
            }
            else {
                word += line[i];
            }
        }
    }
    if(word == Name) {
        return true;
    }
}
bool Animation::findWordFromString(std::string &line, std::string toSep)
{
    uint32_t found = 0;
    for(uint32_t i=0;i<line.size();i++) {
        if(line[i] == toSep[found]) {
            found++;
            if(found == toSep.size())
                return true;
        }
        else if(line[i] != toSep[found] && found > 1) return false;
    }
    return false;
}
bool Animation::findFirstChars(std::string line, std::string toFind, int &startOfLine)
{
    int toStart;
    uint32_t tofindSize = 0;
    for(uint32_t i=0; i<line.size(); i++) {
        if(line[i] != ' ') {
            toStart = i; break;
        }
    }
    for(uint32_t i=toStart;i<line.size();i++){
        if(line[i] == toFind[tofindSize]) {
            tofindSize++;
            if(tofindSize == toFind.size())
                return true;
        }
        else return false;
    }
    return false;
}
std::string Animation::findNextBrackets(std::string line, int &start, bool takeI)
{
    int toStart = Animation::whereToBegin(line, '"', start);
    std::string toReturn;

    for(uint32_t i=toStart + 1; i<line.size(); i++) {
        if(line[i] == '"') {
            if(takeI)start = i+1;
            else start = toStart;
            return toReturn;
        }
        toReturn+=line[i];
    }
    return "";
}
int Animation::whereToBegin(std::string line, char toFind, int start)
{
    for(uint32_t i=start; i<line.size(); i++) {
        if(line[i] == toFind)
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
    for(uint32_t i=beginRead;i<line.size();i++)
    {
        if(line[i] == ' ' || line[i] == '<') {
            tmp = {tmp2, read};
            toReturn.push_back(tmp);
            read = ""; tmp2++;
            if(line[i] == '<') break;
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
    for(uint32_t i=beginRead;i<line.size();i++)
    {
        if(line[i] == ' ' || line[i] == '<') {
            tmp = atof(read.c_str());
            toReturn.push_back(tmp);
            read = "";
            if(line[i] == '<') break;
        }
        else {
            read+= line[i];
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
    for(uint32_t i=beginRead;i<line.size();i++)
    {
        if(line[i] == ' ' || line[i] == '<') {
            tmp[offsetX][offsetY] = atof(read.c_str());
            read = "";
            offsetX++;
            if(offsetX == 4) {
                offsetX = 0;
                offsetY++;
                if(offsetY == 4) {
                    toReturn.push_back(tmp);
                    offsetY = 0;
                }
            }
            if(line[i] == '<') break;
        }
        else {
            read += line[i];
        }
    }
    return toReturn;
}
