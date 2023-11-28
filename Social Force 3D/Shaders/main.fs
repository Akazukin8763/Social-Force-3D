#version 460 core

struct Material {
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
}; 

struct DirectionLight {
    bool enable;
    bool enableShadow;

    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VertexData {
    vec3 position;
    vec2 texCoord; 
    vec3 normal;

    vec4 positionDirectionLightSpace[16];
} fs_in;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;

uniform DirectionLight dirLight;
uniform sampler2D DirectionLightShadowMap[16];
uniform int DirectionLightShadowMapNums;

vec3 CalcDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction); // direction from the position to the light source
    vec3 reflectRay = reflect(-lightDir, normal);

    // Ambient = I * K
    vec3 ambient = light.ambient * texture(material.ambient, fs_in.texCoord).rgb * material.Ka;
  	
    // Diffuse  = I * <N, L> * K
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, fs_in.texCoord).rgb * material.Kd;

    // Specular = I * (<V, R>)^n * K
    float spec = pow(max(dot(viewDir, reflectRay), 0.0f), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, fs_in.texCoord).rgb * material.Ks;

    // Shadow value
	float shadow = 0.0f;

    if (light.enableShadow) {
        for (int i = 0; i < DirectionLightShadowMapNums; i++) {
             // Sets lightCoord to cull space
	        vec3 lightCoord = fs_in.positionDirectionLightSpace[i].xyz / fs_in.positionDirectionLightSpace[i].w;
	
            if (lightCoord.z <= 1.0f) {
                // Get from [-1, 1] range to [0, 1] range just like the shadow map
		        lightCoord = (lightCoord + 1.0f) / 2.0f;
		        float currentDepth = lightCoord.z;

		        // Prevents shadow acne
		        float bias = max(0.005f * (1.0f - dot(normal, lightDir)), 0.0001f);

		        // Smoothens out the shadows
		        int sampleRadius = 2;
		        vec2 pixelSize = 1.0 / textureSize(DirectionLightShadowMap[i], 0);

		        for (int y = -sampleRadius; y <= sampleRadius; y++) {
		            for (int x = -sampleRadius; x <= sampleRadius; x++) {
		                float closestDepth = texture(DirectionLightShadowMap[i], lightCoord.xy + vec2(x, y) * pixelSize).r;

				        if (currentDepth > closestDepth + bias)
					        shadow += 1.0f;     
		            }    
		        }

		        // Get average shadow
		        shadow /= pow((sampleRadius * 2 + 1), 2);
	        }
        }
    }
    
    // Reflection effect = Ambient + Diffuse + Specular
    return ambient + (diffuse + specular) * (1.0 - shadow);
}

void main() {
    if (texture(material.diffuse, fs_in.texCoord).a == 0.0f) // Discard when alpha = 0
        discard;
        
    vec4 renderColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Light
    vec3 normal = normalize(fs_in.normal) * (gl_FrontFacing ? 1 : -1);
    vec3 viewDir = normalize(viewPos - fs_in.position);

    if (dirLight.enable) renderColor += vec4(CalcDirectionLight(dirLight, normal, viewDir), 0.0f);

    // Render Color
    FragColor = renderColor;
}