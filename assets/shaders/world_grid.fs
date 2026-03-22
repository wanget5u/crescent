#version 330

in vec3 fragWorldPos;
out vec4 finalColor;

uniform vec3 camPos;

vec2 trueFwidth(vec2 p) {
    vec2 dx = dFdx(p);
    vec2 dy = dFdy(p);
    return vec2(length(vec2(dx.x, dy.x)), length(vec2(dx.y, dy.y)));
}

float getGrid(vec2 coord, float size) {
	vec2 c = coord / size;
	vec2 derivative = trueFwidth(c);
	vec2 grid = abs(fract(c - 0.5) - 0.5) / derivative;
	float line = min(grid.x, grid.y);
	return 1.0 - min(line, 1.0);
}

void main() {
	float baseSize = 1.0;
	float minPixels = 1.0;
	vec2 dudv = trueFwidth(fragWorldPos.xz);
	float dudvLength = length(dudv);
	float lod = max(0.0, (log(dudvLength * minPixels / baseSize) / log(300.0)) + 1.0);
	float lodLevel = floor(lod);
	float lodFade = fract(lod);
	float cell0 = baseSize * pow(10.0, lodLevel);
	float cell1 = cell0 * 10.0;
	float cell2 = cell1 * 10.0;
	float alpha0 = getGrid(fragWorldPos.xz, cell0);
	float alpha1 = getGrid(fragWorldPos.xz, cell1);
	float alpha2 = getGrid(fragWorldPos.xz, cell2);
	float alpha = max(alpha2, max(alpha1, alpha0 * (1.0 - lodFade)));
	float dist = length(fragWorldPos - camPos);
	float fade = 1.0 - clamp(dist / 300.0, 0.0, 1.0);
	float finalAlpha = alpha * fade;
	vec3 gridColor = vec3(0.9);
	finalColor = vec4(gridColor, finalAlpha);
}
