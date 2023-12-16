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
    int m_maxPedestrians;

    std::vector<Animator> m_animators;
    std::vector<PedestrianState> m_pedestrianStates;

    Animation m_idleAnimtion;
    Animation m_walkAnimtion;
    Animation m_runAnimtion;

    float m_transitionsTime = 0.5f;

    std::vector<glm::mat4> m_modelMatrices;
    std::vector<std::vector<glm::mat4>> m_finalBoneMatrices;

public:
    PedestrianAnimator(int maxPedestrians, Animation idle, Animation walk, Animation run) {
        m_maxPedestrians = maxPedestrians;

        m_idleAnimtion = idle;
        m_walkAnimtion = walk;
        m_runAnimtion = run;

        m_animators = std::vector<Animator>(maxPedestrians);
        m_pedestrianStates = std::vector<PedestrianState>(maxPedestrians, PedestrianState::IDLE);
        m_modelMatrices = std::vector<glm::mat4>(maxPedestrians, Matrix4::identity);
        m_finalBoneMatrices = std::vector<std::vector<glm::mat4>>(maxPedestrians, std::vector<glm::mat4>(MAX_BONES, Matrix4::identity));

        for (int i = 0; i < maxPedestrians; i++)
            m_animators[i] = Animator(&m_idleAnimtion);
        UpdateAnimations(0);
    }

    void UpdateStates(std::vector<PedestrianState> states) {
        if (states.size() != m_maxPedestrians)
            return;

        for (int i = 0; i < m_maxPedestrians; i++) {
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
        for (int i = 0; i < m_maxPedestrians; i++) {
            if (m_pedestrianStates[i] == PedestrianState::UNKNOWN)
                continue;

            m_animators[i].UpdateAnimation(deltaTime);
            m_finalBoneMatrices[i] = m_animators[i].GetFinalBoneMatrices();
        }
    }

    std::vector<glm::mat4> GetModelMatrices() { 
        std::vector<glm::mat4> modelMatrices;

        for (int i = 0; i < m_maxPedestrians; i++) {
            if (m_pedestrianStates[i] == PedestrianState::UNKNOWN)
                continue;
            
            modelMatrices.push_back(m_modelMatrices[i]);
        }

        return modelMatrices;
        //return m_modelMatrices; 
    }

    std::vector<std::vector<glm::mat4>> GetFinalBoneMatrices() {
        std::vector<std::vector<glm::mat4>> finalBoneMatrices;

        for (int i = 0; i < m_maxPedestrians; i++) {
            if (m_pedestrianStates[i] == PedestrianState::UNKNOWN)
                continue;

            finalBoneMatrices.push_back(m_finalBoneMatrices[i]);
        }

        return finalBoneMatrices;
        //return m_finalBoneMatrices; 
    }

};

#endif

