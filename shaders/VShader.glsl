#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform mat3 normal_matrix;

attribute vec4 input_position;
attribute vec3 input_color;
attribute vec3 input_normal;

varying vec3 color;
varying vec3 normal;
varying vec3 position;

void main() {
    gl_Position = mvp_matrix * input_position;
    color = input_color;
    normal = normal_matrix * input_normal;
    position = gl_Position.xyz;
}
