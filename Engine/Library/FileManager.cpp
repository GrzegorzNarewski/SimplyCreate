#include "FileManager.h"

namespace gll
{
    int FileManager::findNumberOfFilesInFolder(std::string path)
    {
        return std::distance(std::filesystem::directory_iterator(path.c_str()), std::filesystem::directory_iterator());
    }

    std::vector<std::string> FileManager::Get_File_Names_In_Folder(std::string path)
    {
        std::vector<std::string> v;
        std::filesystem::path p(path);
        std::filesystem::directory_iterator start(p);
        std::filesystem::directory_iterator end;
        std::transform(start, end, std::back_inserter(v), path_leaf_string());
        return v;
    }

    void FileManager::createFoldersInPath(std::string path)
    {
        std::string tmp = "";
        for (int i = 0; i < path.size(); i++) {
            if (path[i] == '/') {
                if (!std::filesystem::is_directory(tmp) || !std::filesystem::exists(tmp)) {
                    std::filesystem::create_directory(tmp);
                }
                tmp += path[i];
            }
            else {
                tmp += path[i];
            }
        }
    }

    // simple .txt file read
    std::string FileManager::findTitle(std::string& text)
    {
        std::string toReturn = "";

        while (text.size() > 0) {
            if (text[0] == ':') {
                text.erase(text.begin());
                return toReturn;
            }
            else {
                toReturn += text[0];
                text.erase(text.begin());
            }
        }

        return "error";
    }

    char FirstChar(std::string str) {
        for(int i=0; i<str.size(); i++) {
            if(str[i] != ' ') {
                return str[i];
            }
        }
    }

    void DeleteFirstChar(std::string& str) {
        for(int i=0; i<str.size(); i++) {
            if(str[i] != ' ') {
                str.erase(str.begin(), str.begin() + i + 1);
                break;
            }
        }
    }
    void DeleteLastChar(std::string& str) {
        for(int i=str.size()-1; i>= 0; i--) {
            if(str[i] != ' ') {
                str.erase(str.begin() + i, str.end());
                break;
            }
        }
    }

    std::vector<std::string> FileManager::findData(std::string& text, char whatDivides)
    {
        std::vector<std::string> toReturn;
        toReturn.clear();
        toReturn.push_back("");

        for (int i = 0; i < text.size(); i++) {
            if (text[i] == whatDivides) {
                toReturn.push_back("");
            }
            else {
                toReturn.back() += text[i];
            }
        }

        return toReturn;
    }

    std::vector<File_Data> FileManager::readFile(std::string path)
    {
        std::vector<File_Data> data;
        std::string text = "";
        path = "Resources/" + path;
        std::ifstream file(path);

        if(!file.good()) {
            throw std::runtime_error("Cannot read file at location: " + path);
            return {};
        }

        while (getline(file, text)) {
            data.push_back({});

            data.back().title = findTitle(text);
            data.back().title.erase(std::remove(data.back().title.begin(), data.back().title.end(), ' '), data.back().title.end());
            std::transform(data.back().title.begin(), data.back().title.end(), data.back().title.begin(), [](unsigned char c){ return std::tolower(c); });

            data.back().content = findData(text);
            for(int i=0; i<data.back().content.size(); i++) {
                if(FirstChar(data.back().content[i]) != '"') {
                    data.back().content[i].erase(std::remove(data.back().content[i].begin(), data.back().content[i].end(), ' '), data.back().content[i].end());
                }
                else {
                    DeleteFirstChar(data.back().content[i]);
                    DeleteLastChar(data.back().content[i]);
                }
            }
        }

        file.close();

        return data;
    }

    // binary file save and read
    void FileManager::writeInt(std::ofstream& file, int what)
    {
        file.write(reinterpret_cast<char*>(&what), sizeof(int));
    }
    void FileManager::writeFloat(std::ofstream& file, float what)
    {
        file.write(reinterpret_cast<char*>(&what), sizeof(float));
    }
    void FileManager::writeString(std::ofstream& file, std::string what)
    {
        writeInt(file, what.size());
        file.write(what.c_str(), what.size());
    }

    void FileManager::readInt(std::ifstream& file, int& what)
    {
        file.read(reinterpret_cast<char*>(&what), sizeof(what));
    }
    void FileManager::readUInt(std::ifstream& file, unsigned int& what)
    {
        file.read(reinterpret_cast<char*>(&what), sizeof(what));
    }
    void FileManager::readFloat(std::ifstream& file, float& what)
    {
        file.read(reinterpret_cast<char*>(&what), sizeof(what));
    }
    void FileManager::readString(std::ifstream& file, std::string& what)
    {
        int size = 0; what.clear();
        readInt(file, size);
        what.resize(size);
        file.read(&what[0], what.size());
    }

    std::ifstream FileManager::openFileForRead(std::string path)
    {
        return std::ifstream(path, std::ifstream::binary);
    }

    std::ofstream FileManager::openFileForWrite(std::string path)
    {
        createFoldersInPath(path);
        return std::ofstream(path, std::ofstream::binary);
    }

    void FileManager::close(std::ifstream& f)
    {
        f.close();
    }
    void FileManager::close(std::ofstream& f)
    {
        f.close();
    }

    bool FileManager::File_Exists(std::string name) {
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }

    std::string FileManager::Get_Relevant_Float_To_String(float x)
    {
        std::string xx = std::to_string(x);

        while (true)
        {
            if (xx.back() == '.')
            {
                xx.pop_back();
                return xx;
            }
            else if (xx.back() != '0') {
                return xx;
            }
            else {
                xx.pop_back();
            }
        }
    }

    glm::vec2 FileManager::Get_Vec2(std::vector<std::string> datas, int start)
    {
        glm::vec2 toReturn = glm::vec2(0.0f);

        int endz = start + 2;
        if(datas.size() <= endz) {
            endz = datas.size();
        }

        for (int k = start; k < endz; k++) {
            toReturn[k - start] = atof(datas[k].c_str());
        }
        return toReturn;
    }
    glm::vec3 FileManager::Get_Vec3(std::vector<std::string> datas, int start)
    {
        glm::vec3 toReturn = glm::vec3(0.0f);

        int endz = start + 3;
        if(datas.size() <= endz) {
            endz = datas.size();
        }

        for (int k = start; k < endz; k++) {
            toReturn[k - start] = atof(datas[k].c_str());
        }
        return toReturn;
    }
    glm::vec4 FileManager::Get_Vec4(std::vector<std::string> datas, int start)
    {
        glm::vec4 toReturn = glm::vec4(0.0f);

        int endz = start + 4;
        if(datas.size() <= endz) {
            endz = datas.size();
        }

        for (int k = start; k < endz; k++) {
            toReturn[k - start] = atof(datas[k].c_str());
        }
        return toReturn;
    }
    float FileManager::Get_Float(std::vector<std::string> datas, int start)
    {
        int endz = start + 1;
        if(datas.size() <= endz) {
            endz = datas.size();
        }

        for (int k = start; k < endz; k++) {
            return atof(datas[k].c_str());
        }
        return 0.0f;
    }

    glm::vec4 FileManager::getVec4(std::string line, int lineOffset)
    {
        std::string tmp = ""; int id = 0; glm::vec4 toReturn;
        for (int i = lineOffset + 1; i < line.size(); i++) {
            if (line[i] != ' ' && line[i] != ':') {
                tmp += line[i];
            }
            else {
                toReturn[id] = atof(tmp.c_str());
                id++; tmp = "";
            }
        }
        toReturn[id] = atof(tmp.c_str());
        return toReturn;
    }
    glm::vec3 FileManager::Get_Vec3(std::string line, int lineOffset)
    {
        std::string tmp = ""; int id = 0; glm::vec3 toReturn;
        for (int i = lineOffset + 1; i < line.size(); i++) {
            if (line[i] != ' ' && line[i] != ':') {
                tmp += line[i];
            }
            else {
                toReturn[id] = atof(tmp.c_str());
                id++; tmp = "";
            }
        }
        toReturn[id] = atof(tmp.c_str());
        return toReturn;
    }
    glm::vec2 FileManager::getVec2(std::string& line, int lineOffset)
    {
        std::string tmp = ""; int id = 0; glm::vec2 toReturn;
        for (int i = lineOffset + 1; i < line.size(); i++) {
            if (line[i] != ' ' && line[i] != ':') {
                tmp += line[i];
            }
            else {
                toReturn[id] = atof(tmp.c_str());
                id++; tmp = "";
                if (id == 2) {
                    line.erase(line.begin(), line.begin() + i + 1);
                    return toReturn;
                }
            }
        }
        toReturn[id] = atof(tmp.c_str());
        return toReturn;
    }
    bool FileManager::findWordFromString(std::string& line, std::string toSep)
    {
        uint32_t found = 0;
        for (uint32_t i = 0; i < line.size(); i++) {
            if (line[i] == toSep[found]) {
                found++;
                if (found == toSep.size())
                    return true;
            }
            else if (line[i] != toSep[found] && found > 1) return false;
        }
        return false;
    }
    bool FileManager::findFirstChars(std::string line, std::string toFind, int& startOfLine)
    {
        int toStart = 0;
        uint32_t tofindSize = 0;
        for (uint32_t i = startOfLine; i < line.size(); i++) {
            if (line[i] != ' ') {
                toStart = i; break;
            }
        }
        for (uint32_t i = toStart; i < line.size(); i++) {
            if (line[i] == toFind[tofindSize]) {
                tofindSize++;
                if (tofindSize == toFind.size()) {
                    startOfLine = i;
                    return true;
                }
            }
            else return false;
        }
        return false;
    }

    std::string FileManager::findNameFromPath(std::string path)
    {
        int eraseBack = 0, eraseFront = 0;
        for (int i = 0; i < path.size(); i++) {
            if (path[i] == '/') {
                eraseBack = i;
            }
            else if (path[i] == '.') {
                eraseFront = i;
            }
        }
        path.erase(path.begin() + eraseFront, path.end());
        path.erase(path.begin(), path.begin() + eraseBack + 1);
        return path;
    }
}
