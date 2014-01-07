//attribute highp vec4 vertices;
//varying highp vec2 coords;
//void main() {
//    gl_Position = vertices;
//    coords = vertices.xy;
//}

// These attributes are set in the GL_ARRAY_BUFFER
attribute highp vec3 posA;
attribute lowp vec2 texA;
attribute lowp vec3 normalA;

uniform highp mat4 worldMatrixU;
uniform highp mat4 viewMatrixU;
uniform highp mat4 projMatrixU;
//uniform lowp vec4 modelColU;

varying lowp vec2 texcoordV;
varying lowp vec3 normalV;
varying lowp vec4 colV;

void main() {
//    col = colA;
//    col = modelColU;
    gl_Position = projMatrixU * viewMatrixU * worldMatrixU * vec4(posA, 1.0);
//    highp vec4 pos = clamp(gl_Position, 0.0, 1.0);
//    col = clamp(poAs, 0.0, 1.0);
//    col = modelColU;

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    texcoordV = texA;
    normalV = vec3(viewMatrixU * worldMatrixU * vec4(normalA, 0.0));
//    normalV = vec3(0.0,0.0,0.0);
}
