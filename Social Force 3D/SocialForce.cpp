#include "SocialForce.h"

SocialForce::SocialForce() {
    SetScene();
    SetPedestrianRepulsiveForceType(ELLIPTICAL_SPECIFICATION_I);
    //SetPedestrianRepulsiveForceType(CIRCULAR_SPECIFICATION);
}

void SocialForce::SetScene() {
    m_pedestrians.clear();
    m_borders.clear();

    // Create pedestrians
    Pedestrian* pedestrian;

    for (int i = 0; i < MAX_INSTANCES / 2; i++) {
        pedestrian = new Pedestrian(glm::vec3(Utils::Random(-10.0f, -5.0f), 0.0f, Utils::Random(-3.5f, 3.5f)));
        pedestrian->AddCheckpoint(Checkpoint { glm::vec3(Utils::Random(7.5f, 17.5f), 0.0f, Utils::Random(-3.5f, 3.5f)), CHECKPOINT_RADIUS });
        m_pedestrians.push_back(pedestrian);
    }

    for (int i = 0; i < MAX_INSTANCES / 2; i++) {
        pedestrian = new Pedestrian(glm::vec3(Utils::Random(5.0f, 10.0f), 0.0f, Utils::Random(-3.5f, 3.5f)));
        pedestrian->AddCheckpoint(Checkpoint { glm::vec3(Utils::Random(-17.5f, -7.5f), 0.0f, Utils::Random(-3.5f, 3.5f)), CHECKPOINT_RADIUS });
        m_pedestrians.push_back(pedestrian);
    }

    // Create borders
    Border* border;

    border = new Border(glm::vec3(-20.0f, 0.0f, -10.0f), glm::vec3(20.0f, 0.0f, -10.0f));
    m_borders.push_back(border);
    border = new Border(glm::vec3(-20.0f, 0.0f, 10.0f), glm::vec3(20.0f, 0.0f, 10.0f));
    m_borders.push_back(border);

    border = new Border(glm::vec3(-1.5f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 1.5f));
    m_borders.push_back(border);
    border = new Border(glm::vec3(1.5f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 1.5f));
    m_borders.push_back(border);
    border = new Border(glm::vec3(-1.5f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, -1.5f));
    m_borders.push_back(border);
    border = new Border(glm::vec3(1.5f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, -1.5f));
    m_borders.push_back(border);

    border = new Border(glm::vec3(-20.0f, 0.0f, -10.0f), glm::vec3(-20.0f, 0.0f, 10.0f));
    m_borders.push_back(border);
    border = new Border(glm::vec3(20.0f, 0.0f, -10.0f), glm::vec3(20.0f, 0.0f, 10.0f));
    m_borders.push_back(border);
}

void SocialForce::Simulate(float dt) {
    for (Pedestrian* pedestrian : m_pedestrians)
        pedestrian->Simulate(m_pedestrians, m_borders, dt);
}

/*
 *  Instances
 */
std::vector<glm::mat4> SocialForce::GetPedestrianInstanceTransforms() {
    std::vector<glm::mat4> transforms;
    glm::mat4 transform;
    
    for (Pedestrian* pedestrian : m_pedestrians) {
        transform = glm::translate(Matrix4::identity, pedestrian->GetPosition());
        transform = glm::rotate(transform, pedestrian->GetRadian(), Vector3::up);
        transforms.push_back(transform);
    }

    return transforms;
}

std::vector<PedestrianState> SocialForce::GetPedestrianInstanceStates() {
    std::vector<PedestrianState> states;
    PedestrianState state;
    float velocity;

    for (Pedestrian* pedestrian : m_pedestrians) {
        velocity = glm::length(pedestrian->GetVelocity());

        if (velocity < 0.1f) state = PedestrianState::IDLE;
        else if (0.1f <= velocity && velocity < 1.0f) state = PedestrianState::WALK;
        else state = PedestrianState::RUN;

        states.push_back(state);
    }

    return states;
}

std::vector<glm::mat4> SocialForce::GetBorderInstanceTransforms() {
    std::vector<glm::mat4> transforms;
    glm::mat4 transform;

    for (Border* border : m_borders) {
        transform = glm::translate(Matrix4::identity, border->GetPosition());
        transform = glm::rotate(transform, border->GetRadian(), Vector3::up);
        transform = glm::scale(transform, glm::vec3(border->GetLength(), 1, 0.5));
        transforms.push_back(transform);
    }

    return transforms;
}
