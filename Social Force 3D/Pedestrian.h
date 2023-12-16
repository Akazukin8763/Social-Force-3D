#ifndef __PEDESTRIAN_H__
#define __PEDESTRIAN_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Model.h"
#include "Checkpoint.h"
#include "Border.h"
#include "Utils.h"

#include <vector>
#include <queue>
#include <functional>

class Pedestrian {
private:
    bool m_isActivated = true;

    glm::vec3 m_position;
    glm::vec3 m_velocity = Vector3::zero;
    float m_radius = PEDESTRIAN_RADIUS;

    std::queue<Checkpoint> m_checkpoints;

    bool IsAllCheckpointsArrived();
    void CheckCheckpoints();

    glm::vec3 GetDesiredDirection();

    bool enableWeight = false;
    bool enableCollision = false;
    glm::vec3 GetCollisionAvoidForce(Pedestrian* opponent);

    // Driving Force
    glm::vec3 GetDrivingForce();

    // Repulsive Force on Pedestrian
    glm::vec3 (Pedestrian::*PTR_GetPedestrianRepulsiveForce)(std::vector<Pedestrian*>);
    glm::vec3 GetPedestrianRepulsiveForceWithCS(std::vector<Pedestrian*> others); // Circular specification
    glm::vec3 GetPedestrianRepulsiveForceWithES1(std::vector<Pedestrian*> others); // Elliptical specification I
    glm::vec3 GetPedestrianRepulsiveForceWithES2(std::vector<Pedestrian*> others); // Elliptical specification II
    glm::vec3 GetPedestrianRepulsiveForceWithNES(std::vector<Pedestrian*> others); // New elliptical specification
    //glm::vec3 GetPedestrianRepulsiveForceWithCP(std::vector<Pedestrian*> others, float dt); // Collision prediction
    
    // Repulsive Force on Border
    glm::vec3 GetBorderRepulsiveForce(std::vector<Border*> borders, float dt);

    // Other Force
    //glm::vec3 GetAttractiveForce();
    //glm::vec3 GetFluctuations();

    // All of the force
    glm::vec3 GetSocialForce(std::vector<Pedestrian*> others, std::vector<Border*> borders, float dt);

public:
    Pedestrian(glm::vec3 origin);

    bool IsActivated() { return m_isActivated; }
    void SetActivated(bool value) { m_isActivated = value; }

    glm::vec3 GetPosition() { return m_position; }
    float GetRadian() { return glm::orientedAngle(Vector3::forward, GetDesiredDirection(), Vector3::up); }
    glm::vec3 GetVelocity() { return m_velocity; }

    void SetPedestrianRepulsiveForceType(int type);
    void EnableWeight(bool value) { this->enableWeight = value; }
    void EnableCollision(bool value) { this->enableCollision = value; }

    void AddCheckpoint(Checkpoint checkpoint) { m_checkpoints.push(checkpoint); }

    void Simulate(std::vector<Pedestrian*> others, std::vector<Border*> borders, float dt);
};

#endif // !__PEDESTRIAN_H__