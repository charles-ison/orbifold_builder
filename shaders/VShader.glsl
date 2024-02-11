#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;

attribute vec4 a_position;
attribute vec4 input_color;

varying vec4 color;

void main() {
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_position;
    color = input_color;
}
