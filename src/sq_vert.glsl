//attribute highp vec4 vertices;
//varying highp vec2 coords;
//void main() {
//    gl_Position = vertices;
//    coords = vertices.xy;
//}

attribute highp vec4 posAttr;
attribute lowp vec4 colAttr;
varying lowp vec4 col;
uniform highp mat4 matrix;
void main() {
    col = colAttr;
    gl_Position = matrix * posAttr;
}
