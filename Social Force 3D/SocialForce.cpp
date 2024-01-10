#include "SocialForce.h"
#include "HouseParameter.h"
SocialForce::SocialForce(int maxPedestrians) {
	m_currentPedestrians = maxPedestrians;
	m_maxPedestrians = maxPedestrians;
	m_activated = std::vector<bool>(maxPedestrians, true);

	m_pedestrians = std::vector<Pedestrian*>(maxPedestrians);
	m_borders = std::vector<Border*>();

	SetScene();
	SetPedestrianRepulsiveForceType(ELLIPTICAL_SPECIFICATION_I);
	//SetPedestrianRepulsiveForceType(CIRCULAR_SPECIFICATION);
}

void SocialForce::SetScene() {
	m_pedestrians.clear();
	m_borders.clear();

	GeneratePedestrian();
	GenerateBorder();
}

void SocialForce::GeneratePedestrian() {
	// Create pedestrians
	Pedestrian* pedestrian;

	for (int i = 0; i < m_maxPedestrians; i++)
		m_pedestrians.push_back(GeneraePedestrianByType(i % 8));


}

Pedestrian* SocialForce::GeneraePedestrianByType(int type) {
	/*float halfedge = 25.0f;
	float start = halfedge - 5.0f;
	float mid = 7.0f;
	float range = 5.0f;*/

	float roadWidth = 12.0f;
	float roadOffset = roadWidth / 2;

	float halfedge = halfGroundSize;
	float start = halfedge - 5.0f * halfGroundSize / 70;
	float mid = 25.0f * halfGroundSize / 70.0f;
	float range = 5.0f * halfGroundSize / 70.0f;

	Pedestrian* pedestrian;


	float randomFloat1 = Utils::RandomFloat(-range, range);

	/*if (type == 0) {
		pedestrian = new Pedestrian(glm::vec3(start, 0.0f, randomTemp));
		pedestrian->AddCheckpoint(Checkpoint { glm::vec3(mid, 0.0f, randomTemp), CHECKPOINT_RADIUS });
	}
	else if (type == 1) {
		pedestrian = new Pedestrian(glm::vec3(-start, 0.0f, randomTemp));
		pedestrian->AddCheckpoint(Checkpoint { glm::vec3(-mid, 0.0f, randomTemp), CHECKPOINT_RADIUS });
	}
	else if (type == 2) {
		pedestrian = new Pedestrian(glm::vec3(randomTemp, 0.0f, start));
		pedestrian->AddCheckpoint(Checkpoint { glm::vec3(randomTemp, 0.0f, mid), CHECKPOINT_RADIUS });
	}
	else {
		pedestrian = new Pedestrian(glm::vec3(randomTemp, 0.0f, -start));
		pedestrian->AddCheckpoint(Checkpoint { glm::vec3(randomTemp, 0.0f, -mid), CHECKPOINT_RADIUS });
	}*/
	if (type == 0) {
		pedestrian = new Pedestrian(glm::vec3(start, 0.0f, mid + randomFloat1));
		pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(mid,   0.0f, mid + randomFloat1), CHECKPOINT_RADIUS });
	}
	else if (type == 1) {
		pedestrian = new Pedestrian(glm::vec3(mid + randomFloat1, 0.0f, start));
		pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(mid + randomFloat1, 0.0f, mid), CHECKPOINT_RADIUS });
	}
	else if (type == 2) {
		pedestrian = new Pedestrian(glm::vec3(-mid + randomFloat1, 0.0f, start));
		pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(-mid + randomFloat1, 0.0f, mid), CHECKPOINT_RADIUS });
	}
	else if (type == 3) {
		pedestrian = new Pedestrian(glm::vec3(-start, 0.0f, mid + randomFloat1));
		pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(-mid,   0.0f, mid + randomFloat1), CHECKPOINT_RADIUS });
	}
	else if (type == 4) {
		pedestrian = new Pedestrian(glm::vec3(-start, 0.0f, -mid + randomFloat1));
		pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(-mid, 0.0f, -mid + randomFloat1), CHECKPOINT_RADIUS });
	}
	else if (type == 5) {
		pedestrian = new Pedestrian(glm::vec3(-mid + randomFloat1, 0.0f, -start));
		pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(-mid + randomFloat1, 0.0f, -mid), CHECKPOINT_RADIUS });
	}
	else if (type == 6) {
		pedestrian = new Pedestrian(glm::vec3(mid + randomFloat1, 0.0f, -start));
		pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(mid + randomFloat1, 0.0f, -mid), CHECKPOINT_RADIUS });
	}
	else {
		pedestrian = new Pedestrian(glm::vec3(start, 0.0f, -mid + randomFloat1));
		pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(mid, 0.0f, -mid + randomFloat1), CHECKPOINT_RADIUS });
	}

	/*do {
		random = Utils::RandomInt(0, 4);
	} while (random == type);

	if (random == 0) {
		pedestrian->AddCheckpoint(Checkpoint { glm::vec3(mid, 0.0f, Utils::RandomFloat(-range, range)), CHECKPOINT_RADIUS });
		pedestrian->AddCheckpoint(Checkpoint { glm::vec3(start, 0.0f, Utils::RandomFloat(-range, range)), CHECKPOINT_RADIUS });
	}
	else if (random == 1) {
		pedestrian->AddCheckpoint(Checkpoint { glm::vec3(-mid, 0.0f, Utils::RandomFloat(-range, range)), CHECKPOINT_RADIUS });
		pedestrian->AddCheckpoint(Checkpoint { glm::vec3(-start, 0.0f, Utils::RandomFloat(-range, range)), CHECKPOINT_RADIUS });
	}
	else if (random == 2) {
		pedestrian->AddCheckpoint(Checkpoint { glm::vec3(Utils::RandomFloat(-range, range), 0.0f, mid), CHECKPOINT_RADIUS });
		pedestrian->AddCheckpoint(Checkpoint { glm::vec3(Utils::RandomFloat(-range, range), 0.0f, start), CHECKPOINT_RADIUS });
	}
	else {
		pedestrian->AddCheckpoint(Checkpoint { glm::vec3(Utils::RandomFloat(-range, range), 0.0f, -mid), CHECKPOINT_RADIUS });
		pedestrian->AddCheckpoint(Checkpoint { glm::vec3(Utils::RandomFloat(-range, range), 0.0f, -start), CHECKPOINT_RADIUS });
	}*/

	int randomInt1, randomInt2;
	float randomFloat2;

	randomInt2 = Utils::RandomInt(0, 2);

	randomFloat2 = Utils::RandomFloat(-range, range);

	do {
		randomInt1 = Utils::RandomInt(0, 4);
	} while (randomInt1 == type / 2);

	if (randomInt1 == 0) {
		pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(mid + randomFloat2, 0.0f, mid + randomFloat2), CHECKPOINT_RADIUS });
		if (randomInt2 == 0)
			pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(start, 0.0f, mid + randomFloat2), CHECKPOINT_RADIUS });
		else
			pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(mid + randomFloat2, 0.0f, start), CHECKPOINT_RADIUS });
	}
	else if (randomInt1 == 1) {
		pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(-mid + randomFloat2, 0.0f, mid + randomFloat2), CHECKPOINT_RADIUS });
		if (randomInt2 == 0)
			pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(-mid + randomFloat2, 0.0f, start), CHECKPOINT_RADIUS });
		else
			pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(-start, 0.0f, mid + randomFloat2), CHECKPOINT_RADIUS });
	}
	else if (randomInt1 == 2) {
		pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(-mid + randomFloat2, 0.0f, -mid + randomFloat2), CHECKPOINT_RADIUS });
		if (randomInt2 == 0)
			pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(-start, 0.0f, -mid + randomFloat2), CHECKPOINT_RADIUS });
		else
			pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(-mid + randomFloat2, 0.0f, -start), CHECKPOINT_RADIUS });
	}
	else {
		pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(mid + randomFloat2, 0.0f, -mid + randomFloat2), CHECKPOINT_RADIUS });
		if (randomInt2 == 0)
			pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(mid + randomFloat2, 0.0f, -start), CHECKPOINT_RADIUS });
		else
			pedestrian->AddCheckpoint(Checkpoint{ glm::vec3(start, 0.0f, -mid + randomFloat2), CHECKPOINT_RADIUS });
	}
	return pedestrian;
}

void SocialForce::GenerateBorder() {
	float halfedge = 70.0f * halfGroundSize / 70.0f;     //25
	float roadWidth1 = 60.0f * halfGroundSize / 70.0f;    //12
	float roadOffset1 = roadWidth1 / 2;

	float roadWidth2 = 40.0f * halfGroundSize / 70.0f;    //12
	float roadOffset2 = roadWidth2 / 2;

	// Create borders
	Border* border;

	// Bound
	border = new Border(glm::vec3(halfedge, 0.0f, -halfedge), glm::vec3(halfedge, 0.0f, halfedge));
	m_borders.push_back(border);
	border = new Border(glm::vec3(halfedge, 0.0f, -halfedge), glm::vec3(-halfedge, 0.0f, -halfedge));
	m_borders.push_back(border);
	border = new Border(glm::vec3(-halfedge, 0.0f, halfedge), glm::vec3(halfedge, 0.0f, halfedge));
	m_borders.push_back(border);
	border = new Border(glm::vec3(-halfedge, 0.0f, halfedge), glm::vec3(-halfedge, 0.0f, -halfedge));
	m_borders.push_back(border);

	// Intersection
	border = new Border(glm::vec3(-roadOffset1, 0.0f, -halfedge), glm::vec3(-roadOffset1, 0.0f, -roadOffset1));
	m_borders.push_back(border);
	border = new Border(glm::vec3(-halfedge, 0.0f, -roadOffset1), glm::vec3(-roadOffset1, 0.0f, -roadOffset1));
	m_borders.push_back(border);

	border = new Border(glm::vec3(roadOffset1, 0.0f, -halfedge), glm::vec3(roadOffset1, 0.0f, -roadOffset1));
	m_borders.push_back(border);
	border = new Border(glm::vec3(halfedge, 0.0f, -roadOffset1), glm::vec3(roadOffset1, 0.0f, -roadOffset1));
	m_borders.push_back(border);

	border = new Border(glm::vec3(-roadOffset1, 0.0f, halfedge), glm::vec3(-roadOffset1, 0.0f, roadOffset1));
	m_borders.push_back(border);
	border = new Border(glm::vec3(-halfedge, 0.0f, roadOffset1), glm::vec3(-roadOffset1, 0.0f, roadOffset1));
	m_borders.push_back(border);

	border = new Border(glm::vec3(roadOffset1, 0.0f, halfedge), glm::vec3(roadOffset1, 0.0f, roadOffset1));
	m_borders.push_back(border);
	border = new Border(glm::vec3(halfedge, 0.0f, roadOffset1), glm::vec3(roadOffset1, 0.0f, roadOffset1));
	m_borders.push_back(border);


	/*border = new Border(glm::vec3(-roadOffset2, 0.0f, -halfedge), glm::vec3(-roadOffset2, 0.0f, -roadOffset2));
	m_borders.push_back(border);
	border = new Border(glm::vec3(-halfedge, 0.0f, -roadOffset2), glm::vec3(-roadOffset2, 0.0f, -roadOffset2));
	m_borders.push_back(border);

	border = new Border(glm::vec3(roadOffset2, 0.0f, -halfedge), glm::vec3(roadOffset2, 0.0f, -roadOffset2));
	m_borders.push_back(border);
	border = new Border(glm::vec3(halfedge, 0.0f, -roadOffset2), glm::vec3(roadOffset2, 0.0f, -roadOffset2));
	m_borders.push_back(border);

	border = new Border(glm::vec3(-roadOffset2, 0.0f, halfedge), glm::vec3(-roadOffset2, 0.0f, roadOffset2));
	m_borders.push_back(border);
	border = new Border(glm::vec3(-halfedge, 0.0f, roadOffset2), glm::vec3(-roadOffset2, 0.0f, roadOffset2));
	m_borders.push_back(border);

	border = new Border(glm::vec3(roadOffset2, 0.0f, halfedge), glm::vec3(roadOffset2, 0.0f, roadOffset2));
	m_borders.push_back(border);
	border = new Border(glm::vec3(halfedge, 0.0f, roadOffset2), glm::vec3(roadOffset2, 0.0f, roadOffset2));
	m_borders.push_back(border);*/
}

void SocialForce::UpdatePedestrianNums(int nums) {
	if (nums == m_currentPedestrians || nums < 0 || nums > m_maxPedestrians)
		return;

	bool newActivatedState = (nums > m_currentPedestrians ? true : false);
	int updateNums = (nums > m_currentPedestrians ? nums - m_currentPedestrians : m_currentPedestrians - nums);

	m_currentPedestrians = nums;

	for (int i = 0; i < updateNums; i++) {
		int pick = Utils::RandomInt(0, m_maxPedestrians);

		// iter until find the different activated state target
		while (m_pedestrians[pick]->IsActivated() == newActivatedState)
			pick = (pick + 1) % m_maxPedestrians;

		if (!newActivatedState)
			m_pedestrians[pick] = GeneraePedestrianByType(pick % 8);
		m_pedestrians[pick]->SetActivated(newActivatedState);
	}
}

void SocialForce::Simulate(float dt) {
	for (int i = 0; i < m_maxPedestrians; i++) {
		Pedestrian* pedestrian = m_pedestrians[i];
		pedestrian->Simulate(m_pedestrians, m_borders, dt);

		if (pedestrian->IsFinished())
			m_pedestrians[i] = GeneraePedestrianByType(i % 8);
	}
}

/*
 *  Instances
 */
std::vector<glm::mat4> SocialForce::GetPedestrianInstanceTransforms() {
	std::vector<glm::mat4> transforms;
	glm::mat4 transform;

	for (Pedestrian* pedestrian : m_pedestrians) {
		if (!pedestrian->IsActivated())
			continue;

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
		if (pedestrian->IsActivated()) {
			velocity = glm::length(pedestrian->GetVelocity());

			if (velocity < 0.2f) state = PedestrianState::IDLE;
			else if (0.2f <= velocity && velocity < 1.5f) state = PedestrianState::WALK;
			else state = PedestrianState::RUN;

			states.push_back(state);
		}
		else {
			states.push_back(PedestrianState::UNKNOWN);
		}
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
