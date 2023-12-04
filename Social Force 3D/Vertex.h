#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "Utils.h"

struct InstanceData {
	glm::mat4 transform;
};

struct Vertex {
    // position
    glm::vec3 Position;
    // texCoords
    glm::vec2 TexCoord;
    // normal
    glm::vec3 Normal;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    // bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

#endif