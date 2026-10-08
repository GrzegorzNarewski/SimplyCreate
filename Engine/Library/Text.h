#include "Sprite.h"

namespace gll
{
	class Text :public Object
	{
	public:

		Transform transform;
		std::string shader;
		std::string vertexBuffer;
		std::string texture;
		glm::vec4 color;

		float alignX;
		std::string context;
		TEXT_LAYOUT text_layout;

		void createText(std::string p_obj_name, std::string p_context, std::string p_shader, std::string p_text, glm::vec3 p_pos = glm::vec3(0.0f), glm::vec3 p_size = glm::vec3(1.0f), glm::vec3 p_rot = glm::vec3(0.0f), glm::vec4 p_color = glm::vec4(1.0f));

		float getTextSize();
		void AlignX(float direction);
		void AlignType(TEXT_LAYOUT layoutAlign);

		void updateObject(std::string renderPass);
		std::string type();
		std::string name();
		float getRenderPriority();
	};
}