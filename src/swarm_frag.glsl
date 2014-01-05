//uniform lowp float t;
//varying highp vec2 coords;
//void main() {
//    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));
//    i = smoothstep(t - 0.8, t + 0.8, i);
//    i = floor(i * 20.) / 20.;
//    gl_FragColor = vec4(coords * .5 + .5, i, i);
//}
uniform sampler2D texture;
varying vec2 v_texcoord;
varying lowp vec4 col;

void main() {
//    gl_FragColor = col;
    gl_FragColor = (texture2D(texture, v_texcoord)+vec4(0.2,0.2,0.2,1.0))*col;
}
