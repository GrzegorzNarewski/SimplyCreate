#include "Mesh.h"

namespace gll
{
	void Mesh::createMesh(std::string p_obj_name, std::string p_shader, std::string p_vBuffer, std::vector<std::string> p_texts, glm::vec3 p_pos, glm::vec3 p_size, glm::vec3 p_rot, glm::vec4 p_color)
	{
		obj_name = p_obj_name;
		shader = p_shader;
		vertexBuffer = p_vBuffer;
		textures = p_texts;
		transform.position = p_pos;
		transform.size = p_size;
		transform.rotation = p_rot;
		color = p_color;
	}
	void Mesh::updateObject(std::string renderPass)
	{
		gll::rManager()->renderMesh(shader, vertexBuffer, textures, transform, color, renderPass, ubo_addons);
	}
	std::string Mesh::type()
	{
		return "Mesh";
	}
	std::string Mesh::name()
	{
		return obj_name;
	}
	float Mesh::getRenderPriority()
	{
		return 1.0f;
	}
}