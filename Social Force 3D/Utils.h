#ifndef __UTILS_H__
#define __UTILS_H__

#include <glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>

#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>

#define M_PI 3.14159265358979323846f

#define BACKGROUND_COLOR 49.0f/255.0f, 51.0f/255.0f, 56.0f/255.0f, 1.0f

#define MAX_INSTANCES 40
#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4

//#define CASCADES_NUMS 1 // Cascaded Shadow Mapping
#define MAX_DIRECTION_NUMS 16

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

	// Assimp GLM
	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
		glm::mat4 to;

		// the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

		return to;
	}

	static inline glm::vec3 GetGLMVec(const aiVector3D& vec) { 
		return glm::vec3(vec.x, vec.y, vec.z); 
	}

	static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation) {
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}
};

class Vector2 {
public:
	static const glm::vec2 zero;
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