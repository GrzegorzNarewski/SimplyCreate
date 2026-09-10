#include "Camera.h"

namespace gll
{
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
	public:
		static int findNumberOfFilesInFolder(std::string path);


		static std::vector<std::string> Get_File_Names_In_Folder(std::string path);

		static void createFoldersInPath(std::string path);

		// simple .txt file read
		static std::string findTitle(std::string& text);

		static std::vector<std::string> findData(std::string& text, char whatDivides = '|');

		static std::vector<File_Data> readFile(std::string path);

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
}
