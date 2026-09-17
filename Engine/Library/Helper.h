#include "vulkan/vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include <filesystem>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <future>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <cstdint>
#include <limits>
#include <optional>
#include <fstream>
#include <set>
#include <map>
#include <list>
#include <sys/types.h>
#include <direct.h>
#include <sys/stat.h>
#include <stdio.h>

namespace gll
{
    float randomiseFloat(float LO, float HI);
    int randomiseInt(int LO, int HI);

    class DeltaTimer
    {
    public:
        float deltaTime, currentTime, LastFrame;
        void refresh(); // used to fix large lag spikes 
        void updateDeltaTime();
    };
    DeltaTimer* dTime();

    struct Data
    {
        std::string name;
        std::string data;
    };
    struct File_Data
    {
        std::string title;
        std::vector<std::string> content;
    };
    struct path_leaf_string
    {
        std::string operator()(const std::filesystem::directory_entry& entry) const
        {
            return entry.path().string();
        }
    };

    class FileManager
    {
        static Data getDataFromLine(std::string line);

        static std::string getDirectory(std::string line);

    public:

        static std::vector<Data> readFile(std::string path);
        static std::vector<std::string> readFileRaw(std::string path);

        static std::ifstream openFileToRead(std::string path);
        static void closeFile(std::ifstream& file);

        static bool doesFileExist(std::string path);
        static std::ifstream* openBinaryFileRead(std::string path);
        template<typename T>
        static void readBinaryFile(std::ifstream* file, T& data)
        {
            file->read((char*)&data, sizeof(data));
        }
        static std::ofstream* openBinaryFileWrite(std::string path);
        template<typename T>
        static void writeBinaryFile(std::ofstream* file, T& data)
        {
            file->write((char*)&data, sizeof(data));
        }
        static void closeBinaryFile(std::ofstream* file);
        static void closeBinaryFile(std::ifstream* file);

        ///////

        static int findNumberOfFilesInFolder(std::string path);


        static std::vector<std::string> Get_File_Names_In_Folder(std::string path);

        static void createFoldersInPath(std::string path);

        // simple .txt file read
        static std::string findTitle(std::string& text);

        static std::vector<std::string> findData(std::string& text, char whatDivides = '|');

        static std::vector<File_Data> readFilez(std::string path);

        // binary file save and read
        static void writeInt(std::ofstream& file, int what);
        static void writeFloat(std::ofstream& file, float what);
        static void writeString(std::ofstream& file, std::string what);

        static void readInt(std::ifstream& file, int& what);
        static void readUInt(std::ifstream& file, unsigned int& what);
        static void readFloat(std::ifstream& file, float& what);
        static void readString(std::ifstream& file, std::string& what);

        static std::ifstream openFileForRead(std::string path);

        static std::ofstream openFileForWrite(std::string path);

        static void close(std::ifstream& f);
        static void close(std::ofstream& f);

        static bool File_Exists(std::string name);

        static std::string Get_Relevant_Float_To_String(float x);

        static glm::vec2 Get_Vec2(std::vector<std::string> datas, int start = 0);
        static glm::vec3 Get_Vec3(std::vector<std::string> datas, int start = 0);
        static glm::vec4 Get_Vec4(std::vector<std::string> datas, int start = 0);
        static float Get_Float(std::vector<std::string> datas, int start = 0);

        static glm::vec4 getVec4(std::string line, int lineOffset);
        static glm::vec3 Get_Vec3(std::string line, int lineOffset);
        static glm::vec2 getVec2(std::string& line, int lineOffset);
        static bool findWordFromString(std::string& line, std::string toSep);
        static bool findFirstChars(std::string line, std::string toFind, int& startOfLine);

        static std::string findNameFromPath(std::string path);
    };

    std::string getRelaventFloatString(float xx);
};
