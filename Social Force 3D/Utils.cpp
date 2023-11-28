#include "Utils.h"

const glm::vec3 Vector3::zero = glm::vec3(0, 0, 0);
const glm::vec3 Vector3::ones = glm::vec3(1, 1, 1);
const glm::vec3 Vector3::forward = glm::vec3(0, 0, 1);
const glm::vec3 Vector3::back = glm::vec3(0, 0, -1);
const glm::vec3 Vector3::up = glm::vec3(0, 1, 0);
const glm::vec3 Vector3::down = glm::vec3(0, -1, 0);
const glm::vec3 Vector3::right = glm::vec3(1, 0, 0);
const glm::vec3 Vector3::left = glm::vec3(-1, 0, 0);

const glm::mat4 Matrix4::identity = glm::mat4(1);