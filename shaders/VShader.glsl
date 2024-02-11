#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform vec3 line_color;
uniform float input_color_flag;

attribute vec3 input_boundary_color;
attribute vec4 a_position;

varying vec4 position;
varying vec3 color;
varying float color_flag;
varying vec3 boundary_color;

void main() {
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_position;

    position = a_position;
    color_flag = input_color_flag;
    color = line_color;

    if (color_flag == 2.0) {
        boundary_color = input_boundary_color;
    }
}
