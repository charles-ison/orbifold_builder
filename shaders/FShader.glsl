#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec4 position;
varying vec3 color;
varying float use_color;

void main() {
    if (use_color == 1.0) {
        gl_FragColor = vec4(color, 1.0);
    } else {
        gl_FragColor = position;
    }
}

