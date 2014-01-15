// These attributes are set in the GL_ARRAY_BUFFER
attribute highp vec3 posA;
attribute highp vec2 texA;
attribute highp vec3 normalA;

uniform highp mat4 worldMatrixU;
uniform highp mat4 viewMatrixU;
uniform highp mat4 projMatrixU;
varying highp vec2 texcoordV;
varying highp vec3 normalV;
varying highp vec3 posV;

//varying lowp vec2 zw;

void main() {
    gl_Position = projMatrixU * viewMatrixU * worldMatrixU * vec4(posA, 1.0);

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    texcoordV = texA;
    normalV = vec3(viewMatrixU * worldMatrixU * vec4(normalA, 0.0));
    posV = vec3(viewMatrixU * worldMatrixU * vec4(posA, 0.0));
}
