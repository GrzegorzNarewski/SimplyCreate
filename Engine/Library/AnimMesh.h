#include "Mesh.h"

namespace gll
{
	class AnimMesh :public Object
	{
		std::vector<int> framesFinished;
	public:
		Transform transform;
		std::string shader;
		std::string vertexBuffer;
		std::vector<std::string> textures;
		glm::vec4 color;

		std::map<std::string, gll::Animation> animes;
		std::string activeAnime;
		std::vector<glm::mat4>* models;
		bool animationPaused;
		float animationSpeed;

		void createAnimMesh(std::string p_obj_name, std::string p_shader, std::string p_vBuffer, std::vector<std::string> p_texts, std::map<std::string, std::string> p_animes, glm::vec3 p_pos = glm::vec3(0.0f), glm::vec3 p_size = glm::vec3(1.0f), glm::vec3 p_rot = glm::vec3(0.0f), glm::vec4 p_color = glm::vec4(1.0f));

		void changeAnimation(std::string animation);
		void pauseAnimation(bool should);
		void inheritAnimation(std::vector<glm::mat4>* modelsToInherit);
		bool isAnimationFinished();
		bool isKeyFrameFinished(int keyFrame);
		void setAnimationSpeed(float aSpeed);
			
		void updateObject(std::string renderPass);
		std::string type();
		std::string name();
		float getRenderPriority();
	};
}