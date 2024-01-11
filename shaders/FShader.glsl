#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec4 position;

void main() {
    gl_FragColor = position;
    //gl_FragColor = vec4(153.0/255.0, 204.0/255.0, 255.0/255.0, 1.0);
}

