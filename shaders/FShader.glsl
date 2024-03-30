#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform vec3 camera_position;

varying vec3 color;
varying vec3 normal;
varying vec3 position;

void main() {
    float ambient_strength = 0.4;
    vec3 light_color = vec3(1.0, 1.0, 1.0);
    vec3 ambient = ambient_strength * light_color;

    vec3 light_position = vec3(-3.0, 0.0, 5.0);
    vec3 light_direction = normalize(light_position - position);
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = diff * light_color;

    float specular_strength = 0.3;
    vec3 flipped_camera_position = vec3(camera_position.x, camera_position.y, -camera_position.z);
    vec3 view_direction = normalize(flipped_camera_position - position);
    vec3 halfway_direction = normalize(light_direction + view_direction);
    float spec = pow(max(dot(normal, halfway_direction), 0.0), 8.0);
    vec3 specular = specular_strength * spec * light_color;

    vec3 lighting = ambient + diffuse + specular;
    gl_FragColor = vec4(lighting * color, 1.0);
}

