#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec4 position;
varying vec3 color;
varying float color_flag;
varying vec3 boundary_color;

void main() {
    if (color_flag == 2.0) {
        gl_FragColor = vec4(boundary_color, 1.0);
    } else if (color_flag == 1.0) {
        gl_FragColor = vec4(color, 1.0);
    } else {
        gl_FragColor = position;
    }
}

