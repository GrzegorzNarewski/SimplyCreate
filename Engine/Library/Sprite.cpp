#include "Sprite.h"

namespace gll
{
	void Sprite::createSprite(std::string p_obj_name, std::string p_shader, std::string p_texts, glm::vec3 p_pos, glm::vec3 p_size, glm::vec3 p_rot, glm::vec4 p_color)
	{
		obj_name = p_obj_name;
		shader = p_shader;
		vertexBuffer = "square";
		texture = p_texts;
		transform.position = p_pos;
		transform.size = p_size;
		transform.rotation = p_rot;
		color = p_color;
		alignX = 0.0f;
	}
	void Sprite::updateObject(std::string renderPass)
	{
		transform.position.x = transform.position.x + (alignX * (gll::rManager()->maxX - 1.0f));
		gll::rManager()->renderSprite(shader, vertexBuffer, texture, transform, color, renderPass, ubo_addons);
		transform.position.x = transform.position.x - (alignX * (gll::rManager()->maxX - 1.0f));
	}

	void Sprite::AlignX(float direction)
	{
		alignX = direction;
	}

	std::string Sprite::type()
	{
		return "Sprite";
	}
	std::string Sprite::name()
	{
		return obj_name;
	}
	float Sprite::getRenderPriority()
	{
		return 2.0f;
	}
}