#define highp

// These attributes are set in the GL_ARRAY_BUFFER
attribute highp vec3 posA;
attribute highp vec3 normalA;

uniform highp mat4 worldMatrixU;
uniform highp mat4 projViewMatrixU;
varying highp vec3 normalV;
varying highp vec3 posV;

//varying lowp vec2 zw;

void main() {
    // gl_position is relative to the eye/camera
    gl_Position = projViewMatrixU * worldMatrixU * vec4(posA, 1.0);
    gl_PointSize = 15.0;

    // normalV and posV are world-space (relative to 0,0,0)
    normalV = vec3(worldMatrixU * vec4(normalA, 0.0)); // vector has 0 as 4th component
    posV = vec3(worldMatrixU * vec4(posA, 1.0)); // point has 1...
}
