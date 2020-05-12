#include "PSIMPCH.h"
#include "VulkanModelLoad.h"

#include "Platform/Vk/FrameWork/VulkanFrameWork.h"
#include <tinyObjLoader/tiny_obj_loader.h>
#include <glm/gtx/hash.hpp>

namespace std {
	template<> struct hash<VulkanBuffer::Vertex> {
		size_t operator()(VulkanBuffer::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

VulkanModelLoad::VulkanModelLoad()
{
}


VulkanModelLoad::~VulkanModelLoad()
{
}

void VulkanModelLoad::loadModel(const std::string MODEL_PATH)
{
	PSIM_PROFILE_FUNCTION();
	VulkanFrameWork *framework = VulkanFrameWork::getFramework();
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	//load model
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
		PSIM_CORE_ERROR(warn + err);
	}

	//ensure that each vertex is unique and parsed according to obj rulesu
	std::unordered_map<VulkanBuffer::Vertex, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			VulkanBuffer::Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(framework->vertices.size());
				framework->vertices.push_back(vertex);
			}

			framework->indices.push_back(uniqueVertices[vertex]);
		}
	}
}