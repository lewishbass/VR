#include "Model.hpp"

Model::Model(const std::string& filepath)
{
    loadOBJ(filepath);
}

bool Model::loadOBJ(const std::string& filepath){
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str());
    if (!warn.empty()) {
        logMessage(2, "TinyObjLoader warning: " + warn, {"Graphics", "Model"});
    }
    if (!err.empty()) {
        logMessage(1, "TinyObjLoader error: " + err, {"Graphics", "Model"});
    }
    if (!ret) {
        logMessage(1, "Failed to load/parse .obj file: " + filepath, {"Graphics", "Model"});
        return false;
    }
    
    for (const auto& shape : shapes) {
        Mesh mesh;
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            if (index.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }

            if (index.texcoord_index >= 0) {
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }

            // Load vertex colors if available (RGB format)
            if (!attrib.colors.empty() && index.vertex_index * 3 + 2 < attrib.colors.size()) {
                vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2]
                };
            } else {
                // Default to white if no color is specified
                vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);
            }

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
                mesh.vertices.push_back(vertex);
            }

            mesh.indices.push_back(uniqueVertices[vertex]);
        }
        meshes.push_back(mesh);
    }
    int total_faces = 0;
    for (const auto& shape : shapes) {
        total_faces += shape.mesh.num_face_vertices.size();
    }
    std::stringstream ss;
    ss << "     Total vertices: " << attrib.vertices.size() / 3 << "\n";
    ss << "     Total faces: " << total_faces << "\n";
    ss << "     Has vertex colors: " << (!attrib.colors.empty() ? "Yes" : "No") << "\n";
    ss << "     Has texture coordinates: " << (!attrib.texcoords.empty() ? "Yes" : "No");
    
    logMessage(3, "Successfully loaded model: " + filepath, {"Graphics", "Model"});
    logMessage(4, ss.str(), {"Graphics", "Model"});
    return true;
}
