#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec4 position;
varying vec3 color;

void main() {
    gl_FragColor = position;
    //gl_FragColor = vec4(color, 1.0);
}

