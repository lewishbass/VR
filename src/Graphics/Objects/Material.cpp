#include "Material.hpp"

Material::Material(std::string filePath, std::string name) : filePath(filePath), name(name) {
    if(loadFromPath(filePath)){
        logMessage(3, "Material loaded successfully from path: " + filePath, {"Graphics", "Material", "Slang"});
    }else{
        logMessage(2, "Failed to load material from path: " + filePath, {"Graphics", "Material", "Slang"});
    }


}

Material::~Material() {
    if (vertexShaderCode != nullptr) {
        delete vertexShaderCode;
    }
    if (fragmentShaderCode != nullptr) {
        delete fragmentShaderCode;
    }
    if (computeShaderCode != nullptr) {
        delete computeShaderCode;
    }
    if (geometryShaderCode != nullptr) {
        delete geometryShaderCode;
    }
    logMessage(3, "Material destroyed: " + name, {"Graphics", "Material", "Slang"});

}

bool Material::loadFromPath(const std::string &path, const std::string &extension) {
    // list files in the directory
    std::filesystem::path dirPath = std::filesystem::path(SHADER_BINARY_DIR) / path;
    if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) {
        logMessage(2, "Material path does not exist or is not a directory: " + path, {"Graphics", "Material", "Slang"});
        return false;
    }
    int loadedShaderCount = 0;
    for (const auto &entry : std::filesystem::directory_iterator(dirPath)) {
        const auto &filePath = entry.path();
        if (filePath.extension() == extension) {
            if (filePath.filename().string().find(".vertex.") != std::string::npos) {
                if (!loadFile(filePath.string(), vertexShaderCode, extension)) {
                    logMessage(2, "Failed to load vertex shader file: " + filePath.string(), {"Graphics", "Material", "Slang"});
                    return false;
                }else{
                    logMessage(3, "Loaded vertex shader file: " + filePath.string(), {"Graphics", "Material", "Slang"});
                    loadedShaderCount++;
                    vertexShaderLoaded = true;
                }
            } else if (filePath.filename().string().find(".fragment.") != std::string::npos) {
                if (!loadFile(filePath.string(), fragmentShaderCode, extension)) {
                    logMessage(2, "Failed to load fragment shader file: " + filePath.string(), {"Graphics", "Material", "Slang"});
                    return false;
                }else{
                    logMessage(3, "Loaded fragment shader file: " + filePath.string(), {"Graphics", "Material", "Slang"});
                    loadedShaderCount++;
                    fragmentShaderLoaded = true;
                }
            } else if (filePath.filename().string().find(".geometry.") != std::string::npos) {
                if (!loadFile(filePath.string(), geometryShaderCode, extension)) {
                    logMessage(2, "Failed to load geometry shader file: " + filePath.string(), {"Graphics", "Material", "Slang"});
                    return false;
                }else{
                    logMessage(3, "Loaded geometry shader file: " + filePath.string(), {"Graphics", "Material", "Slang"});
                    loadedShaderCount++;
                    geometryShaderLoaded = true;
                }
            } else if (filePath.filename().string().find(".compute.") != std::string::npos) {
                if (!loadFile(filePath.string(), computeShaderCode, extension)) {
                    logMessage(2, "Failed to load compute shader file: " + filePath.string(), {"Graphics", "Material", "Slang"});
                    return false;
                }else{
                    logMessage(3, "Loaded compute shader file: " + filePath.string(), {"Graphics", "Material", "Slang"});
                    loadedShaderCount++;
                    computeShaderLoaded = true;
                }
            }
        }
    }
    return loadedShaderCount > 0;
}

bool Material::loadFile(const std::string &filepath, std::vector<char>*& buffer, const std::string &extension) {

    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        logMessage(2, "Failed to open " + extension + " file: " + filepath, {"Graphics", "Material", "Slang"});
        return false;
    }

    size_t fileSize = (size_t)file.tellg();

    if (fileSize == 0) {
        logMessage(2, extension + " file is empty: " + filepath, {"Graphics", "Material", "Slang"});
        file.close();
        return false;
    }

    if(buffer != nullptr){
        // delete existing buffer
        logMessage(3, "Deleting existing buffer for file: " + filepath, {"Graphics", "Material", "Slang"});
        delete buffer;
    }
    buffer = new std::vector<char>(fileSize);

    file.seekg(0);
    file.read(buffer->data(), fileSize);
    file.close();

    return true;
}