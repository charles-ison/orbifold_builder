#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform vec3 line_color;
uniform float input_boundary_count;
uniform float input_color_flag;

//attribute float input_color_scale;
attribute vec4 a_position;

varying vec4 position;
varying vec3 color;
varying float color_flag;
//varying float color_scale;
varying float boundary_count;

void main() {
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_position;
    position = a_position;
    color_flag = input_color_flag;
    color = line_color;
    //color_scale = input_color_scale;
    boundary_count = input_boundary_count;
}
