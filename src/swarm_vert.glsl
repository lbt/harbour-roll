//attribute highp vec4 vertices;
//varying highp vec2 coords;
//void main() {
//    gl_Position = vertices;
//    coords = vertices.xy;
//}

attribute highp vec4 posA;
attribute lowp vec2 colA;
varying lowp vec4 col;
uniform highp mat4 matrixU;
uniform lowp vec4 modelColU;

struct DirectionalLight
{
    vec3 Color;
    float AmbientIntensity;
    vec3 Direction;
    float DiffuseIntensity;
};

uniform DirectionalLight gDirectionalLight;

varying vec2 v_texcoord;


void main() {
//    col = colA;
    col = modelColU;
    gl_Position = matrixU * posA;
//    highp vec4 pos = clamp(gl_Position, 0.0, 1.0);
//    col = clamp(poAs, 0.0, 1.0);
//    col = modelColU;
    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = colA;
}
