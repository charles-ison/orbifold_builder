#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec3 color;
varying vec3 normal;
varying vec4 position;

void main() {
    float ambient_strength = 0.1;
    vec3 light_color = vec3(1.0, 1.0, 1.0);
    vec3 ambient = ambient_strength * light_color;

    vec3 light_position = vec3(-10, 0, -10);
    vec3 light_dir = normalize(light_position - position.xyz);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    vec3 lighting = diffuse + ambient;
    gl_FragColor = vec4(lighting * color, 1.0);
}

