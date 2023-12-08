#ifndef __SOCIAL_FORCE_H__
#define __SOCIAL_FORCE_H__

#include <glm/glm.hpp>

#include "Shader.h"
#include "Pedestrian.h"
#include "Border.h"
#include "PedestrianAnimator.h"

#include <vector>

class SocialForce {
private:
    std::vector<Pedestrian*> m_pedestrians;
    std::vector<Border*> m_borders;

public:
    SocialForce();

    void AddPedestrian(Pedestrian* pedestrian) { m_pedestrians.push_back(pedestrian); }
    void AddBorder(Border* border) { m_borders.push_back(border); }

    void SetScene();

    void SetPedestrianRepulsiveForceType(int type) { 
        for (Pedestrian* pedestrian : m_pedestrians)
            pedestrian->SetPedestrianRepulsiveForceType(type);
    }

    void EnableWeight(bool value) { 
        for (Pedestrian* pedestrian : m_pedestrians)
            pedestrian->EnableWeight(value);
    }

    void EnableCollision(bool value) { 
        for (Pedestrian* pedestrian : m_pedestrians)
            pedestrian->EnableCollision(value);
    }

    void Simulate(float dt);

    std::vector<glm::mat4> GetPedestrianInstanceTransforms();
    std::vector<PedestrianState> GetPedestrianInstanceStates();
    std::vector<glm::mat4> GetBorderInstanceTransforms();

};

#endif // !__SOCIAL_FORCE_H__