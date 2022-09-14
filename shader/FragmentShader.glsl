#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;

uniform sampler2D textureSampler;
uniform float lightPower;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

void main() {
    float distance = length(lightPosition - vertexPosition);

    vec3 ambientColor = 0.1 * vec3(texture(textureSampler, vertexUV));
    vec3 diffuseColor = 0.6 * vec3(texture(textureSampler, vertexUV));
    vec3 specularColor = 0.3 * vec3(1, 1, 1);

    vec3 ambient = ambientColor;

    vec3 N = normalize(vertexNormal);
    vec3 L = normalize(lightPosition - vertexPosition);
    vec3 diffuse =  diffuseColor * abs(dot(N, L)) * lightPower / (distance * distance);

    vec3 V = normalize(cameraPosition - vertexPosition);
    vec3 H = normalize(L + V);
    vec3 specular = specularColor * pow(abs(dot(N, H)), 10) * lightPower / (distance * distance);

    gl_FragColor = vec4(ambient + diffuse + specular, 1);
}