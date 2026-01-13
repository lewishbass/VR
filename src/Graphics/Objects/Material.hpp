#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>
#include <vector>
#include <fstream>
#include "Utils/Utils.hpp"

#include <filesystem>

#include <vulkan/vulkan.h>


class Material{

public:
    Material(const std::string filePath, const std::string name = "UnnamedMaterial");
    ~Material();

private:
    bool loadFromPath(const std::string &path, const std::string &extension = ".spv");

    bool loadFile(const std::string &filepath, std::vector<char> *&buffer, const std::string &extension);

    std::string filePath;
    std::string name;

    std::vector<char>* vertexShaderCode = nullptr;
    std::vector<char>* fragmentShaderCode = nullptr;
    std::vector<char>* computeShaderCode = nullptr;
    std::vector<char>* geometryShaderCode = nullptr;

    bool vertexShaderLoaded = false;
    bool fragmentShaderLoaded = false;
    bool computeShaderLoaded = false;
    bool geometryShaderLoaded = false;

    //VKShaderModule vertexShaderModule = VK_NULL_HANDLE;
    
};

#endif // MATERIAL_HPP