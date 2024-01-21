#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
uniform vec3 line_color;
uniform float use_line_color;

attribute vec4 a_position;

varying vec4 position;
varying vec3 color;
varying float use_color;

void main() {
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * a_position;
    position = a_position;
    use_color = use_line_color;
    color = line_color;
}
