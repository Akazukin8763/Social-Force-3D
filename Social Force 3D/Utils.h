#ifndef __UTILS_H__
#define __UTILS_H__

#include <glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>

#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>

#define M_PI 3.14159265358979323846f

#define BACKGROUND_COLOR 49.0f/255.0f, 51.0f/255.0f, 56.0f/255.0f, 1.0f

#define MAX_INSTANCES 100
#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4

//#define CASCADES_NUMS 1 // Cascaded Shadow Mapping
#define MAX_DIRECTION_NUMS 16

// Pedestrian 
#define PEDESTRIAN_RADIUS 0.5f
#define CHECKPOINT_RADIUS 1.2f

#define SAFE_ZONE 1.0f

// Desired Force
#define DESIRED_SPEED 2.34f
#define RELAXATION_TIME 0.5f

// Repulsive Force on Pedestrian
#define CIRCULAR_SPECIFICATION 1
#define ELLIPTICAL_SPECIFICATION_I 2
#define ELLIPTICAL_SPECIFICATION_II 3
#define NEW_ELLIPTICAL_SPECIFICTION 4
//#define COLLISION_PREDICTION 5






class Utils {
public:
	static int RandomInt(int lower, int upper) {
		return (lower + (rand() * 1.0f / RAND_MAX) * (upper - lower));
	}

	static float RandomFloat(float lower, float upper) {
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