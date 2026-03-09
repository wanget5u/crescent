#version 330

in vec3 fragWorldPos;
out vec4 finalColor;

uniform vec3 camPos;

float getGrid(vec2 coord, float size) {
    vec2 c = coord / size;
    vec2 derivative = fwidth(c);
    vec2 grid = abs(fract(c - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    return 1.0 - min(line, 1.0);
}

void main() {
    float thinAlpha = getGrid(fragWorldPos.xz, 1.0);
    float thickAlpha = getGrid(fragWorldPos.xz, 10.0);
    float alpha = max(thinAlpha * 0.2, thickAlpha);
    float dist = length(fragWorldPos - camPos);
    float fade = 1.0 - clamp(dist / 100.0, 0.0, 1.0);
    float finalAlpha = alpha * fade;
    vec3 gridColor = vec3(1.0);
    finalColor = vec4(gridColor, finalAlpha);
}