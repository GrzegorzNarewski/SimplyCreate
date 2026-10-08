#include "Text.h"

namespace gll
{
	void Text::createText(std::string p_obj_name, std::string p_context, std::string p_shader, std::string p_text, glm::vec3 p_pos, glm::vec3 p_size, glm::vec3 p_rot, glm::vec4 p_color)
	{
		obj_name = p_obj_name;
		shader = p_shader;
		vertexBuffer = "square";
		texture = p_text;
		transform.position = p_pos;
		transform.size = p_size;
		transform.rotation = p_rot;
		color = p_color;
		context = p_context;
		alignX = 0.0f;
		text_layout = gll::CENTERED;
	}

	float Text::getTextSize()
	{
		return rManager()->findTextSize(transform, context);
	}
	void Text::AlignX(float direction)
	{
		alignX = direction;
	}
	void Text::AlignType(TEXT_LAYOUT layoutAlign)
	{
		text_layout = layoutAlign;
	}

	void Text::updateObject(std::string renderPass)
	{
		transform.position.x = transform.position.x + (alignX * (gll::rManager()->maxX - 1.0f));
		gll::rManager()->renderText(context, transform, color, text_layout, renderPass, ubo_addons);
		transform.position.x = transform.position.x - (alignX * (gll::rManager()->maxX - 1.0f));
	}
	std::string Text::type()
	{
		return "Text";
	}
	std::string Text::name()
	{
		return obj_name;
	}
	float Text::getRenderPriority()
	{
		return 2.0f;
	}
}