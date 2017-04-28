#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

uniform Material material;

struct DirectionLight {
    vec4 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionLight directionLight[1]; 

out vec4 color;

uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos; 
in vec2 TexCoords;

vec3 calculateDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-(light.direction.xyz));
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular); 
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    color = vec4(calculateDirectionLight(directionLight[0], norm, viewDir), 1.0f);
}

