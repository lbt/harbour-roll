//attribute highp vec4 vertices;
//varying highp vec2 coords;
//void main() {
//    gl_Position = vertices;
//    coords = vertices.xy;
//}

attribute highp vec4 posAttr;
attribute lowp vec2 colAttr;
varying lowp vec4 col;
uniform highp mat4 matrix;
uniform lowp vec4 modelCol;

struct DirectionalLight
{
    vec3 Color;
    float AmbientIntensity;
};

uniform DirectionalLight gDirectionalLight;

varying vec2 v_texcoord;


void main() {
//    col = colAttr;
//    col = modelCol;
    gl_Position = matrix * posAttr;
    highp vec4 pos = clamp(gl_Position, 0.0, 1.0);
    col = clamp(pos, 0.0, 1.0);
//    col = modelCol;
    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = colAttr;
}
