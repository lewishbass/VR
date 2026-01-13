#ifndef MODEL_HPP
#define MODEL_HPP



#include <tiny_obj_loader.h>
#include <vector>
#include <glm/glm.hpp>
#include <functional>

#include "Utils/Utils.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 color;

    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && texCoord == other.texCoord && color == other.color;
    }
};

namespace std
{
    template <>
    struct hash<Vertex>
    {
        size_t operator()(Vertex const &v) const noexcept
        {
            auto h = size_t{0};
            auto mix = [](size_t seed, size_t v) noexcept
            {
                seed ^= v + 0x9e3779b97f4a7c15ull + (seed << 6) + (seed >> 2);
                return seed;
            };
            // hash floats from position, normal, texCoord
            h = mix(h, std::hash<float>{}(v.position.x));
            h = mix(h, std::hash<float>{}(v.position.y));
            h = mix(h, std::hash<float>{}(v.position.z));
            h = mix(h, std::hash<float>{}(v.normal.x));
            h = mix(h, std::hash<float>{}(v.normal.y));
            h = mix(h, std::hash<float>{}(v.normal.z));
            h = mix(h, std::hash<float>{}(v.texCoord.x));
            h = mix(h, std::hash<float>{}(v.texCoord.y));
            h = mix(h, std::hash<float>{}(v.color.x));
            h = mix(h, std::hash<float>{}(v.color.y));
            h = mix(h, std::hash<float>{}(v.color.z));
            return h;
        }
    };
}

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

class Model {
public:
    Model(const std::string& filepath);
    const std::vector<Mesh>& getMeshes() const { return meshes; }
    const tinyobj::attrib_t& getAttrib() const { return attrib; }

private:
    std::vector<Mesh> meshes;
    tinyobj::attrib_t attrib;
    bool loadOBJ(const std::string& filepath);
};

#endif // MODEL_HPP
