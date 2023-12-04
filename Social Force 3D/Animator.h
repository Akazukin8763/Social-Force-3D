#ifndef __ANIMATOR_H__
#define __ANIMATOR_H__

#include <glm/glm.hpp>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "Animation.h"
#include "Bone.h"

#include <map>
#include <vector>

class Animator {
private:
	std::vector<glm::mat4> m_finalBoneMatrices;
	Animation* m_currentAnimation;

	float m_currentTime;
	float m_deltaTime;

public:
	Animator(Animation* animation) {
		m_currentTime = 0.0;
		m_currentAnimation = animation;

		m_finalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_finalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	void UpdateAnimation(float dt) {
		m_deltaTime = dt;

		if (m_currentAnimation) {
			m_currentTime += m_currentAnimation->GetTicksPerSecond() * dt;
			m_currentTime = fmod(m_currentTime, m_currentAnimation->GetDuration());
			CalculateBoneTransform(&m_currentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void PlayAnimation(Animation* pAnimation) {
		m_currentAnimation = pAnimation;
		m_currentTime = 0.0f;
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = m_currentAnimation->FindBone(nodeName);

		if (Bone) {
			Bone->Update(m_currentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_currentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_finalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices() {
		return m_finalBoneMatrices;
	}

};

#endif