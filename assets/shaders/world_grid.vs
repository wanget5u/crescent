#version 330

in vec3 vertexPosition;

uniform mat4 mvp;
uniform mat4 matModel;

out vec3 fragWorldPos;

void main() {
    fragWorldPos = vertexPosition;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}