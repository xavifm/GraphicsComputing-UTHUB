#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float shininess;

void main()
{
    vec4 texColor = texture(texture1, TexCoords);
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.005 * (distance * distance)); 

    vec3 ambient = ambientStrength * lightColor * texColor.rgb;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor * texColor.rgb;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 lighting = (ambient + diffuse + specular) * attenuation;
    
    FragColor = vec4(lighting, texColor.a);
}
