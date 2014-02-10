// These attributes are set in the GL_ARRAY_BUFFER
attribute highp vec3 posA;

uniform highp mat4 worldMatrixU;
uniform highp mat4 projViewMatrixU;
uniform highp mat4 projMatrixU;
uniform highp mat4 viewMatrixU;


void main(void)
{
    gl_Position = projViewMatrixU * worldMatrixU * vec4(posA, 1.0);
}
