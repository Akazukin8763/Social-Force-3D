#ifndef __PEDESTRIAN_ANIMATOR_H__
#define __PEDESTRIAN_ANIMATOR_H__

#include "Animator.h"
#include "Animation.h"
#include "Utils.h"

enum class PedestrianState {
    UNKNOWN,
    IDLE, WALK, RUN
};

class PedestrianAnimator {
private:
    std::vector<Animator> m_animators;
    std::vector<PedestrianState> m_pedestrianStates;

    Animation m_idleAnimtion;
    Animation m_walkAnimtion;
    Animation m_runAnimtion;

    float m_transitionsTime = 0.5f;

    std::vector<glm::mat4> m_modelMatrices;
    std::vector<std::vector<glm::mat4>> m_finalBoneMatrices;

public:
    PedestrianAnimator(Animation idle, Animation walk, Animation run) {
        m_idleAnimtion = idle;
        m_walkAnimtion = walk;
        m_runAnimtion = run;

        m_animators = std::vector<Animator>(MAX_INSTANCES);
        m_pedestrianStates = std::vector<PedestrianState>(MAX_INSTANCES, PedestrianState::IDLE);
        m_modelMatrices = std::vector<glm::mat4>(MAX_INSTANCES, Matrix4::identity);
        m_finalBoneMatrices = std::vector<std::vector<glm::mat4>>(MAX_INSTANCES, std::vector<glm::mat4>(MAX_BONES, Matrix4::identity));

        for (int i = 0; i < MAX_INSTANCES; i++)
            m_animators[i] = Animator(&m_idleAnimtion);
        UpdateAnimations(0);
    }

    void UpdateStates(std::vector<PedestrianState> states) {
        if (states.size() != MAX_INSTANCES)
            return;

        for (int i = 0; i < MAX_INSTANCES; i++) {
            if (m_pedestrianStates[i] != states[i]) {
                m_pedestrianStates[i] = states[i];

                switch (states[i]) {
                case PedestrianState::UNKNOWN:
                    break;
                case PedestrianState::IDLE:
                    m_animators[i].PlayAnimation(&m_idleAnimtion, m_transitionsTime);
                    break;
                case PedestrianState::WALK:
                    m_animators[i].PlayAnimation(&m_walkAnimtion, m_transitionsTime);
                    break;
                case PedestrianState::RUN:
                    m_animators[i].PlayAnimation(&m_runAnimtion, m_transitionsTime);
                    break;
                default:
                    break;
                }
            }
        }
    }

    void UpdateAnimations(float deltaTime) {
        for (int i = 0; i < MAX_INSTANCES; i++) {
            /*
            if (m_animators[i].IsTransiting())
                m_modelMatrices[i] = glm::scale(Matrix4::identity, glm::vec3(0.01f, 0.01f, 0.01f)); // Due to the mixamo model size, we need to resize the model matrix by 0.01
            else
                m_modelMatrices[i] = Matrix4::identity;
            */

            m_animators[i].UpdateAnimation(deltaTime);
            m_finalBoneMatrices[i] = m_animators[i].GetFinalBoneMatrices();
        }
    }

    std::vector<glm::mat4> GetModelMatrices() { return m_modelMatrices; }
    std::vector<std::vector<glm::mat4>> GetFinalBoneMatrices() { return  m_finalBoneMatrices; }

};

#endif

