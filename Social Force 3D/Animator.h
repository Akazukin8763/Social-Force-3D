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

	// Current
	Animation* m_currentAnimation;
	float m_currentTime;

	// Target
	Animation* m_targetAnimation;
	float m_targetTime;

	// Transitions
	bool m_isTransiting;
	float m_transitionTime;
	float m_transitionsProgress;
	Animation* m_nextTransitAnimation = nullptr;

public:
	Animator() = default;
	Animator(Animation* animation) {
		m_currentTime = 0.0f;
		m_currentAnimation = animation;

		m_targetTime = 0.0f;
		m_targetAnimation = nullptr;

		m_isTransiting = false;
		m_transitionTime = 0.0f;
		m_transitionsProgress = 0.0f;

		// Setup Bone Matrices
		m_finalBoneMatrices.reserve(MAX_BONES);

		for (int i = 0; i < MAX_BONES; i++)
			m_finalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	void UpdateAnimation(float dt) {
		// Need to transit animations
		if (m_isTransiting && m_transitionsProgress < m_transitionTime) {
			m_transitionsProgress = std::min(m_transitionsProgress + dt, m_transitionTime);

			// Interpolate the Animations
			float ratio = m_transitionsProgress / m_transitionTime;
			
			m_currentTime += m_currentAnimation->GetTicksPerSecond() * dt;
			m_currentTime = fmod(m_currentTime, m_currentAnimation->GetDuration());

			m_targetTime += m_targetAnimation->GetTicksPerSecond() * dt;
			m_targetTime = fmod(m_targetTime, m_targetAnimation->GetDuration());

			CalculateBlendedBoneTransform(&m_currentAnimation->GetRootNode(), &m_targetAnimation->GetRootNode(), Matrix4::identity, ratio);

			// Finish the animation transition
			if (m_transitionsProgress == m_transitionTime) {
				m_isTransiting = false;
				m_currentTime = m_targetTime;
				m_currentAnimation = m_targetAnimation;

				if (m_nextTransitAnimation != nullptr) {
					PlayAnimation(m_nextTransitAnimation, m_transitionTime);
					m_nextTransitAnimation = nullptr;
				}
			}
		}
		// Loop the same animation
		else {
			m_currentTime += m_currentAnimation->GetTicksPerSecond() * dt;
			m_currentTime = fmod(m_currentTime, m_currentAnimation->GetDuration());

			CalculateBoneTransform(&m_currentAnimation->GetRootNode(), Matrix4::identity);
		}
	}

	void PlayAnimation(Animation* animation, float transitions) {
		if (m_isTransiting) {
			m_nextTransitAnimation = animation;
			return;
		}

		m_targetAnimation = animation;
		m_targetTime = 0.0f;

		m_isTransiting = true;
		m_transitionTime = transitions;
		m_transitionsProgress = 0.0f;
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
		const std::string& nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = m_currentAnimation->FindBone(nodeName);

		if (Bone) {
			Bone->Update(m_currentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		const auto& boneInfoMap = m_currentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
			const int index = boneInfoMap.at(nodeName).id;
			const glm::mat4& offset = boneInfoMap.at(nodeName).offset;
			m_finalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	void CalculateBlendedBoneTransform(const AssimpNodeData* currentNode, const AssimpNodeData* targetNode, const glm::mat4& parentTransform, const float ratio) {
		const std::string& nodeName = currentNode->name;
		glm::mat4 currentNodeTransform = currentNode->transformation;
		glm::mat4 targetNodeTransform = targetNode->transformation;

		Bone* bone = nullptr;
			
		bone = m_currentAnimation->FindBone(nodeName);
		if (bone) {
			bone->Update(m_currentTime);
			currentNodeTransform = bone->GetLocalTransform();
		}

		bone = m_targetAnimation->FindBone(nodeName);
		if (bone) {
			bone->Update(m_targetTime);
			targetNodeTransform = bone->GetLocalTransform();
		}

		// Blend two matrices
		const glm::quat rot0 = glm::quat_cast(currentNodeTransform);
		const glm::quat rot1 = glm::quat_cast(targetNodeTransform);
		const glm::quat finalRotation = glm::slerp(rot0, rot1, ratio);
		glm::mat4 blendedMatrix = glm::mat4_cast(finalRotation);
		blendedMatrix[3] = (1.0f - ratio) * currentNodeTransform[3] + targetNodeTransform[3] * ratio;

		glm::mat4 globalTransformation = parentTransform * blendedMatrix;

		const auto& boneInfoMap = m_currentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
			const int index = boneInfoMap.at(nodeName).id;
			const glm::mat4& offset = boneInfoMap.at(nodeName).offset;
			m_finalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < currentNode->childrenCount; i++)
			CalculateBlendedBoneTransform(&currentNode->children[i], &targetNode->children[i], globalTransformation, ratio);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices() {
		return m_finalBoneMatrices;
	}

	bool IsTransiting() {
		return m_isTransiting;
	}

};

#endif