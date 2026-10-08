#include "Text.h"

namespace gll
{
	class Mesh :public Object
	{
	public:
		
		Transform transform;
		std::string shader;
		std::string vertexBuffer;
		std::vector<std::string> textures;
		glm::vec4 color;

		void createMesh(std::string p_obj_name, std::string p_shader, std::string p_vBuffer, std::vector<std::string> p_texts, glm::vec3 p_pos = glm::vec3(0.0f), glm::vec3 p_size = glm::vec3(1.0f), glm::vec3 p_rot = glm::vec3(0.0f), glm::vec4 p_color = glm::vec4(1.0f));

		void updateObject(std::string renderPass);
		std::string type();
		std::string name();
		float getRenderPriority();
	};
}