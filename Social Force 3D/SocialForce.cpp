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

    for (int i = 0; i < 20; i++) {
        pedestrian = new Pedestrian(glm::vec3(Utils::Random(-10.0f, -5.0f), 0.0f, Utils::Random(-3.5f, 3.5f)));
        pedestrian->AddCheckpoint(Checkpoint { glm::vec3(Utils::Random(7.5f, 17.5f), 0.0f, Utils::Random(-3.5f, 3.5f)), CHECKPOINT_RADIUS });
        m_pedestrians.push_back(pedestrian);
    }

    for (int i = 0; i < 20; i++) {
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

std::vector<InstanceData> SocialForce::GetPedestrianInstances() {
    std::vector<InstanceData> instances;
    InstanceData instance;
    
    for (Pedestrian* pedestrian : m_pedestrians) {
        instance.transform = glm::translate(Matrix4::identity, pedestrian->GetPosition());
        instance.transform = glm::rotate(instance.transform, pedestrian->GetRadian(), Vector3::up);
        instances.push_back(instance);
    }

    return instances;
}

std::vector<InstanceData> SocialForce::GetBorderInstances() {
    std::vector<InstanceData> instances;
    InstanceData instance;

    for (Border* border : m_borders) {
        instance.transform = glm::translate(Matrix4::identity, border->GetPosition());
        instance.transform = glm::rotate(instance.transform, border->GetRadian(), Vector3::up);
        instance.transform = glm::scale(instance.transform, glm::vec3(border->GetLength(), 1, 0.5));
        instances.push_back(instance);
    }

    return instances;
}
