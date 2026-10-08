#include "AnimMesh.h"

namespace gll
{
	void AnimMesh::createAnimMesh(std::string p_obj_name, std::string p_shader, std::string p_vBuffer, std::vector<std::string> p_texts, std::map<std::string, std::string> p_animes, glm::vec3 p_pos, glm::vec3 p_size, glm::vec3 p_rot, glm::vec4 p_color)
	{
		obj_name = p_obj_name;
		shader = p_shader;
		vertexBuffer = p_vBuffer;
		textures = p_texts;
		transform.position = p_pos;
		transform.size = p_size;
		transform.rotation = p_rot;
		color = p_color;
		animationPaused = false;
		models = new std::vector<glm::mat4>[1];
		animationSpeed = 1.0f;

		for (auto& an : p_animes) {
			animes[an.first].createAnimation(an.second.c_str());
			activeAnime = an.first;
		}
	}
	void AnimMesh::changeAnimation(std::string animation)
	{
		activeAnime = animation;
	}
	void AnimMesh::pauseAnimation(bool should)
	{
		animationPaused = should;
	}
	void AnimMesh::inheritAnimation(std::vector<glm::mat4>* modelsToInherit)
	{
		pauseAnimation(true);
		delete[] models;
		models = modelsToInherit;
	}
	bool AnimMesh::isAnimationFinished()
	{
		for (int i = 0; i < framesFinished.size(); i++)
			if (framesFinished[i] == 0) return true;
		return false;
	}
	bool AnimMesh::isKeyFrameFinished(int keyFrame)
	{
		for (int i = 0; i < framesFinished.size(); i++)
			if (framesFinished[i] == keyFrame) return true;
		return false;
	}
	void AnimMesh::setAnimationSpeed(float aSpeed)
	{
		animationSpeed = aSpeed;
	}
	void AnimMesh::updateObject(std::string renderPass)
	{
		if (!animationPaused) {
			framesFinished = animes[activeAnime].updateAnimation(models[0], gll::rManager()->findVertexBuffer(vertexBuffer)->heir, gll::dTime()->deltaTime * animationSpeed, transform.getModel(), false);
		}
		gll::rManager()->renderAnimMesh(shader, vertexBuffer, textures, models[0], color, renderPass, ubo_addons);
	}
	std::string AnimMesh::type()
	{
		return "AnimMesh";
	}
	std::string AnimMesh::name()
	{
		return obj_name;
	}
	float AnimMesh::getRenderPriority()
	{
		return 1.0f - (color.a / 2.0f);
	}
}