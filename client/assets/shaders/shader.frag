#version 330

out vec4 finalColor;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform float ambient;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;

void main()
{
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(diff);

    vec3 result = (vec3(ambient) + diffuse) * colDiffuse.xyz;
    finalColor = vec4(result, 1.0);
}
