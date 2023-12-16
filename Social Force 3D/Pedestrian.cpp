#include "Pedestrian.h"

Pedestrian::Pedestrian(glm::vec3 origin) {
    m_position = origin;

    this->PTR_GetPedestrianRepulsiveForce = &Pedestrian::GetPedestrianRepulsiveForceWithCS; // Default
}

bool Pedestrian::IsAllCheckpointsArrived() {
    return m_checkpoints.size() == 0;
}

void Pedestrian::CheckCheckpoints() {
    if (IsAllCheckpointsArrived())
        return;

    //if (m_checkpoints.size() == 1) // make those who has already arrived goal not to block others like a fixed obstacle
    //    return;

    Checkpoint point = m_checkpoints.front();

    // If the position is inside the checkpoint area, change the checkpoint to next one
    if (glm::length(point.goal - m_position) < point.radius)
        m_checkpoints.pop();
}

glm::vec3 Pedestrian::GetDesiredDirection() {
    if (IsAllCheckpointsArrived())
        return Vector3::zero;

    Checkpoint point = m_checkpoints.front();

    // Compute desired direction
    // r_k: destination
    // r_i: current position
    // => e_i = (r_k - r_i) / || r_k - r_i ||
    glm::vec3 desiredDirection = point.goal - m_position;
    desiredDirection = glm::normalize(desiredDirection);

    if (isnan(desiredDirection.x) || isnan(desiredDirection.y) || isnan(desiredDirection.z)) // this means target is so close to the checkpoint (the distance tends to 0)
        desiredDirection = glm::vec3(0.0f);

    return desiredDirection;
}

glm::vec3 Pedestrian::GetCollisionAvoidForce(Pedestrian* opponent) {
    //const float A = CA_A;
    //const float B = CA_B;
    const float K = CA_K;
    const float KAPPA = CA_KAPPA;

    // Assign values
    float r_ij = m_radius + opponent->m_radius;
    float d_ij = glm::length(m_position - opponent->m_position);
    glm::vec3 n_ij = glm::normalize(m_position - opponent->m_position);
    glm::vec3 t_ij = glm::vec3(-n_ij.y, n_ij.x, 0.0f);
    float dv_ji = glm::dot((opponent->m_velocity - m_velocity), t_ij);

    if (d_ij > r_ij) // No Collision
        return Vector3::zero;

    // Compute the repulsive force with Collision avoidance (CA)
    // A: const
    // r_ij: sum of the radius of each other
    // d_ij: vector from opponent to self
    // B: const
    // k: const
    // n_ij: normalized vector from opponent to self
    // κ: const
    // t_ij: tangential direction of the vector which is from self to opponent
    // dv_ji: tangential velocity difference
    // => f_ij = { A * exp[(r_ij - d_ij) / B] + k * g(r_ij - d_ij) } * n_ij + κ * g(r_ij - d_ij) * dv_ji * t_ij
    //    g(x) = 0 if d_ij > r_ij,
    //           x otherwise
    //glm::vec3 repulsiveInteractionForce = A * exp((r_ij - d_ij) / B) * n_ij; // it has been use in Circular specification
    glm::vec3 bodyForce = K * (r_ij - d_ij) * n_ij;
    glm::vec3 slidingFrictionForce = KAPPA * (r_ij - d_ij) * dv_ji * t_ij;

    return (bodyForce + slidingFrictionForce);
}

glm::vec3 Pedestrian::GetDrivingForce() {
    const float k = 1 / RELAXATION_TIME;

    // Compute desired direction
    glm::vec3 e_i = GetDesiredDirection();

    // Compute driving force
    // k: 1 / relaxation time
    // v: desired speed
    // e_i: desired direction
    // v_i: current velocity
    // => f_i = k * (v * e_i - v_i)
    glm::vec3 force = k * (DESIRED_SPEED * e_i - m_velocity);

    return force;
}

glm::vec3 Pedestrian::GetPedestrianRepulsiveForceWithCS(std::vector<Pedestrian*> others) {
    glm::vec3 force = glm::vec3(0.0f);

    const float A = CS_A;
    const float B = CS_B;
    const float LAMBDA = CS_LAMBDA;

    glm::vec3 e_i = GetDesiredDirection();

    for (Pedestrian* opponent : others) {
        // Exclude itself or not activared opponent
        if (opponent == this || !opponent->m_isActivated)
            continue;

        // Ignore if opponent is too far away
        glm::vec3 vector = m_position - opponent->m_position;
        if (glm::length(vector) > m_radius + opponent->m_radius + SAFE_ZONE)
            continue;

        // Assign values
        float r_ij = m_radius + opponent->m_radius;
        glm::vec3 d_ij = m_position - opponent->m_position;

        // Compute the repulsive force
        // A: const
        // B: const
        // r_ij: sum of the radius of each other
        // d_ij: vector from opponent to self
        // => f_ij = { A * e ^ [(r_ij - || d_ij ||) / B] } * (d_ij / || d_ij ||)
        glm::vec3 f_ij = A * exp((r_ij - glm::length(d_ij)) / B) * glm::normalize(d_ij);

        // Compute the weight
        // λ: const
        // e_i: self desired direction
        // d_ij: vector from opponent to self
        // ϕ: angle between e_i and d_ij
        // => w = λ + (1 - λ) * (1 + cos(ϕ)) / 2
        if (enableWeight)
            f_ij *= LAMBDA + (1 - LAMBDA) * (1 + glm::dot(e_i, glm::normalize(d_ij))) / 2.0f;

        // Compute the collision force
        if (enableCollision)
            f_ij += GetCollisionAvoidForce(opponent);

        // Add force
        force += f_ij;
    }

    return force;
}

glm::vec3 Pedestrian::GetPedestrianRepulsiveForceWithES1(std::vector<Pedestrian*> others) {
    glm::vec3 force = glm::vec3(0.0f);

    const float TAU = ES1_TAU;
    const float A = ES1_A;
    const float B = ES1_B;
    const float LAMBDA = ES1_LAMBDA;

    glm::vec3 e_i = GetDesiredDirection();

    for (Pedestrian* opponent : others) {
        // Exclude itself or not activared opponent
        if (opponent == this || !opponent->m_isActivated)
            continue;

        // Ignore if opponent is too far away
        glm::vec3 vector = m_position - opponent->m_position;
        if (glm::length(vector) > m_radius + opponent->m_radius + SAFE_ZONE)
            continue;

        // Assign values
        glm::vec3 d_ij = m_position - opponent->m_position;
        glm::vec3 v_j = glm::length(opponent->m_velocity) * opponent->GetDesiredDirection();
        glm::vec3 y_ij = v_j * TAU;

        // Compute semi-minor axis of the ellipse with Elliptical specification I (ES1)
        // d_ij: vector from opponent to self
        // v_j: opponent desired velocity
        // τ: time of pedestrian stride
        // => 2b = sqrt[ (|| d_ij ||  + || d_ij - v_j * τ ||) ^ 2 - (|| v_j * τ ||) ^ 2 ]
        float param1 = glm::length(d_ij) + glm::length(d_ij - v_j * TAU);
        float param2 = glm::length(v_j * TAU);
        float inSqrt = param1 * param1 - param2 * param2;

        float b = inSqrt > 0 ? sqrt(inSqrt) / 2.0f : 0.01f; // avoid negative inside the root

        // Compute the repulsive force
        // A: const
        // B: const
        // b: semi-minor axis
        // d_ij: vector from opponent to self
        // y_ij: v_j * τ
        // => f_ij = A * e ^ (-b / B) * 
        //           [ (|| d_ij || + || d_ij - y_ij ||) / (2 * b) ] * 
        //           { (d_ij / || d_ij ||) + [ (d_ij - y_ij) / || d_ij - y_ij || ] } / 2
        float param3 = A * exp(-b / B);
        float param4 = (glm::length(d_ij) + glm::length(d_ij - y_ij)) / (2.0f * b);
        glm::vec3 param5 = (glm::normalize(d_ij) + glm::normalize(d_ij - y_ij)) / 2.0f;

        glm::vec3 f_ij = param3 * param4 * param5;

        // Compute the weight
        // λ: const
        // e_i: self desired direction
        // d_ij: vector from opponent to self
        // ϕ: angle between e_i and d_ij
        // => w = λ + (1 - λ) * (1 + cos(ϕ)) / 2
        if (enableWeight)
            f_ij *= LAMBDA + (1 - LAMBDA) * (1 + glm::dot(e_i, glm::normalize(d_ij))) / 2.0f;

        // Compute the collision force
        if (enableCollision)
            f_ij += GetCollisionAvoidForce(opponent);

        // Add force
        force += f_ij;

        /* Or you can compute the gradient of repulsive potential function
        // Repulsive Potential with ES1
        float GetRepulsivePotential(vec3 d_ij, vec3 v_j) {
            // 2b = sqrt[ (|| d_ij ||  + || d_ij - v_j * τ ||) ^ 2 - (|| v_j * τ ||) ^ 2 ]
            ...
            // V = A * B * e ^ (-b / B)
            ...
            return V
        }

        float v = GetRepulsivePotential(d_ij, v_j);

        vec3 dx = glm::vec3(dt, 0.0f, 0.0f);
        vec3 dy = glm::vec3(0.0f, dt, 0.0f);

        float dvdx = (GetRepulsivePotential(d_ij + dx, v_j) - v) / dt;
        float dvdy = (GetRepulsivePotential(d_ij + dy, v_j) - v) / dt;

        vec3 gradient = glm::vec3(dvdx, dvdy, 0.0f);

        vec3 f_ij = -gradient;
        */
    }

    return force;
}

glm::vec3 Pedestrian::GetPedestrianRepulsiveForceWithES2(std::vector<Pedestrian*> others) {
    glm::vec3 force = glm::vec3(0.0f);

    const float TAU = ES2_TAU;
    const float A = ES2_A;
    const float B = ES2_B;
    const float LAMBDA = ES2_LAMBDA;

    glm::vec3 e_i = GetDesiredDirection();

    for (Pedestrian* opponent : others) {
        // Exclude itself or not activared opponent
        if (opponent == this || !opponent->m_isActivated)
            continue;

        // Ignore if opponent is too far away
        glm::vec3 vector = m_position - opponent->m_position;
        if (glm::length(vector) > m_radius + opponent->m_radius + SAFE_ZONE)
            continue;

        // Assign values
        glm::vec3 d_ij = m_position - opponent->m_position;
        glm::vec3 v_i = m_velocity;
        glm::vec3 v_j = opponent->m_velocity;
        glm::vec3 y_ij = (v_j - v_i) * TAU;

        // Compute semi-minor axis of the ellipse with Elliptical specification II (ES2)
        // d_ij: vector from opponent to self
        // v_i: self velocity
        // v_j: opponent velocity
        // τ: time of pedestrian stride
        // => 2b = sqrt[ (|| d_ij ||  + || d_ij - (v_j - v_i) * τ ||) ^ 2 - (|| (v_j - v_i) * τ ||) ^ 2 ]
        float param1 = glm::length(d_ij) + glm::length(d_ij - y_ij);
        float param2 = glm::length(y_ij);
        float inSqrt = param1 * param1 - param2 * param2;

        float b = inSqrt > 0 ? sqrt(inSqrt) / 2.0f : 0.01f; // avoid negative inside the root

        // Compute the repulsive force
        // A: const
        // B: const
        // b: semi-minor axis
        // d_ij: vector from opponent to self
        // y_ij: (v_j - v_i) * τ
        // => f_ij = A * e ^ (-b / B) * 
        //           [ (|| d_ij || + || d_ij - y_ij ||) / (2 * b) ] * 
        //           { (d_ij / || d_ij ||) + [ (d_ij - y_ij) / || d_ij - y_ij || ] } / 2
        float param3 = A * exp(-b / B);
        float param4 = (glm::length(d_ij) + glm::length(d_ij - y_ij)) / (2.0f * b);
        glm::vec3 param5 = (glm::normalize(d_ij) + glm::normalize(d_ij - y_ij)) / 2.0f;

        glm::vec3 f_ij = param3 * param4 * param5;

        // Compute the weight
        // λ: const
        // e_i: self desired direction
        // d_ij: vector from opponent to self
        // ϕ: angle between e_i and d_ij
        // => w = λ + (1 - λ) * (1 + cos(ϕ)) / 2
        if (enableWeight)
            f_ij *= LAMBDA + (1 - LAMBDA) * (1 + glm::dot(e_i, glm::normalize(d_ij))) / 2.0f;

        // Compute the collision force
        if (enableCollision)
            f_ij += GetCollisionAvoidForce(opponent);

        // Add force
        force += f_ij;
    }

    return force;
}

glm::vec3 Pedestrian::GetPedestrianRepulsiveForceWithNES(std::vector<Pedestrian*> others) {
    glm::vec3 force = glm::vec3(0.0f);

    const float TAU = NES_TAU;
    const float A = NES_A;
    const float B = NES_B;
    const float LAMBDA = NES_LAMBDA;

    glm::vec3 e_i = GetDesiredDirection();

    for (Pedestrian* opponent : others) {
        // Exclude itself or not activared opponent
        if (opponent == this || !opponent->m_isActivated)
            continue;

        // Ignore if opponent is too far away
        glm::vec3 vector = m_position - opponent->m_position;
        if (glm::length(vector) > m_radius + opponent->m_radius + SAFE_ZONE)
            continue;

        // Assign values
        glm::vec3 d_ij = m_position - opponent->m_position;
        glm::vec3 v_i = m_velocity;
        glm::vec3 v_j = opponent->m_velocity;
        glm::vec3 y_ij = (v_j - v_i) * TAU;

        // Compute semi-minor axis of the ellipse with New elliptical specification (NES)
        // d_ij: vector from opponent to self
        // v_i: self velocity
        // v_j: opponent velocity
        // τ: time of pedestrian stride
        // => 2b = sqrt{ [ (|| d_ij ||  + || d_ij - (v_j - v_i) * τ ||) ^ 2 - (|| (v_j - v_i) * τ ||) ^ 2 ] / (1 + || v_i || * τ) }
        float param1 = glm::length(d_ij) + glm::length(d_ij - y_ij);
        float param2 = glm::length(y_ij);
        float param3 = 1 + glm::length(v_i) * TAU;
        float inSqrt = (param1 * param1 - param2 * param2) / param3;

        float b = inSqrt > 0 ? sqrt(inSqrt) / 2.0f : 0.01f; // avoid negative inside the root

        // Compute the repulsive force
        // A: const
        // B: const
        // b: semi-minor axis
        // d_ij: vector from opponent to self
        // y_ij: (v_j - v_i) * τ
        // => f_ij = [ A * e ^ (-b / B) ] / sqrt(1 + || v_i || * τ) * 
        //           [ (|| d_ij || + || d_ij - y_ij ||) / (2 * b) ] * 
        //           { (d_ij / || d_ij ||) + [ (d_ij - y_ij) / || d_ij - y_ij || ] } / 2
        float param4 = A * exp(-b / B) / sqrt(param3);
        float param5 = (glm::length(d_ij) + glm::length(d_ij - y_ij)) / (2.0f * b);
        glm::vec3 param6 = (glm::normalize(d_ij) + glm::normalize(d_ij - y_ij)) / 2.0f;

        glm::vec3 f_ij = param4 * param5 * param6;

        // Compute the weight
        // λ: const
        // e_i: self desired direction
        // d_ij: vector from opponent to self
        // ϕ: angle between e_i and d_ij
        // => w = λ + (1 - λ) * (1 + cos(ϕ)) / 2
        if (enableWeight)
            f_ij *= LAMBDA + (1 - LAMBDA) * (1 + glm::dot(e_i, glm::normalize(d_ij))) / 2.0f;

        // Compute the collision force
        if (enableCollision)
            f_ij += GetCollisionAvoidForce(opponent);

        // Add force
        force += f_ij;
    }

    return force;
}

glm::vec3 Pedestrian::GetBorderRepulsiveForce(std::vector<Border*> borders, float dt) {
    glm::vec3 force = glm::vec3(0.0f);

    const float A = BR_A;
    const float B = BR_B;

    for (Border* border : borders) {
        // Compute the nearest point to border
        glm::vec3 nearest = border->GetNearestPoint(m_position);

        // Assign values
        float r_i = m_radius;
        glm::vec3 d_iW = m_position - nearest;

        // Compute the repulsive force
        // A: const
        // B: const
        // r_i: self radius
        // d_iW: vector from border to self
        // => f_iW = { A * e ^ [(r_i - || d_iW || ) / B] } * (d_iW / || d_iW ||)
        glm::vec3 f_iW = A * exp((r_i - glm::length(d_iW)) / B) * glm::normalize(d_iW);

        // Add force
        force += f_iW;
    }

    return force;
}

glm::vec3 Pedestrian::GetSocialForce(std::vector<Pedestrian*> others, std::vector<Border*> borders, float dt) {
    // Social Force = Driving Force + Repulsive Force (Pedestrian) + Repulsive Force (Border) + Attractive Force + Fluctuations
    glm::vec3 f_i = GetDrivingForce();
    glm::vec3 f_ij = (this->*PTR_GetPedestrianRepulsiveForce)(others);
    glm::vec3 f_iW = GetBorderRepulsiveForce(borders, dt);
    //glm::vec3 f_iA;
    //glm::vec3 fluctuations;

    return f_i + f_ij + f_iW;
}

void Pedestrian::SetPedestrianRepulsiveForceType(int type) {
    if (type == CIRCULAR_SPECIFICATION)
        PTR_GetPedestrianRepulsiveForce = &Pedestrian::GetPedestrianRepulsiveForceWithCS;
    else if (type == ELLIPTICAL_SPECIFICATION_I)
        PTR_GetPedestrianRepulsiveForce = &Pedestrian::GetPedestrianRepulsiveForceWithES1;
    else if (type == ELLIPTICAL_SPECIFICATION_II)
        PTR_GetPedestrianRepulsiveForce = &Pedestrian::GetPedestrianRepulsiveForceWithES2;
    else if (type == NEW_ELLIPTICAL_SPECIFICTION)
        PTR_GetPedestrianRepulsiveForce = &Pedestrian::GetPedestrianRepulsiveForceWithNES;
    //else if (COLLISION_PREDICTION)
    //    PTR_GetPedestrianRepulsiveForce = &Pedestrian::GetPedestrianRepulsiveForceWithCP;
}

void Pedestrian::Simulate(std::vector<Pedestrian*> others, std::vector<Border*> borders, float dt) {
    // Return if isn't activated
    if (!m_isActivated)
        return;

    // Check position
    CheckCheckpoints();

    // Return if arrive all checkpoints
    if (IsAllCheckpointsArrived())  {
        m_velocity = glm::vec3(0.0f); // Clear velocity
        return;
    }

    // Compute social force
    glm::vec3 force = GetSocialForce(others, borders, dt);

    // Calc velocity and position
    m_velocity += force * dt; // Cuz we don't consider mass, the force is equal to acceleration
    
    if (glm::length(m_velocity) > DESIRED_SPEED) // Limit velocity if exceed desired speed
        m_velocity = glm::normalize(m_velocity) * DESIRED_SPEED;

    m_position += m_velocity * dt;
}
