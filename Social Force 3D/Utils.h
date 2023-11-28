#ifndef __UTILS_H__
#define __UTILS_H__

#include <glm/glm.hpp>

#define M_PI 3.14159265358979323846f

#define BACKGROUND_COLOR 49.0f/255.0f, 51.0f/255.0f, 56.0f/255.0f, 1.0f

#define CASCADES_NUMS 1 // Cascaded Shadow Mapping

// Pedestrian 
#define PEDESTRIAN_RADIUS 0.5f
#define CHECKPOINT_RADIUS 0.03f

#define SAFE_ZONE 1.0f

// Desired Force
#define DESIRED_SPEED 1.34f
#define RELAXATION_TIME 0.5f

// Repulsive Force on Pedestrian
#define CIRCULAR_SPECIFICATION 1
#define ELLIPTICAL_SPECIFICATION_I 2
#define ELLIPTICAL_SPECIFICATION_II 3
#define NEW_ELLIPTICAL_SPECIFICTION 4
//#define COLLISION_PREDICTION 5

//#define CA_A 2000.0f / 80.0f
//#define CA_B 0.08f
#define CA_K 1.2 * 100000.0f / 80.0f		// 80 is the mass value from Helbing, D., Farkas, I., & Vicsek, T. (2000)
#define CA_KAPPA 2.4 * 100000.0f / 80.0f

#define CS_A 10.0f
#define CS_B 0.34f
#define CS_LAMBDA 1.0f

#define ES1_TAU 0.53f
#define ES1_A 9.2f
//#define ES1_B 0.44f
#define ES1_B 1.64f
#define ES1_LAMBDA 0.58f

#define ES2_TAU 1.74f
#define ES2_A 0.8f
#define ES2_B 0.62f
#define ES2_LAMBDA 0.19f

#define NES_TAU 1.78f
#define NES_A 1.33f
#define NES_B 0.34f
#define NES_LAMBDA 0.08f

// Repulsive Force on Border
#define BR_A 2000.0f
#define BR_B 0.05f

class Utils {
public:
	static float Random(float lower, float upper) {
		return (lower + (rand() * 1.0f / RAND_MAX) * (upper - lower));
	}
};

class Vector3 {
public:
	static const glm::vec3 zero;
	static const glm::vec3 ones;

	static const glm::vec3 forward;
	static const glm::vec3 back;
	static const glm::vec3 up;
	static const glm::vec3 down;
	static const glm::vec3 right;
	static const glm::vec3 left;
};

class Matrix4 {
public:
	static const glm::mat4 identity;
};

#endif // !__UTILS_H__