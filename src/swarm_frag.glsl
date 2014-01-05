//uniform lowp float t;
//varying highp vec2 coords;
//void main() {
//    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));
//    i = smoothstep(t - 0.8, t + 0.8, i);
//    i = floor(i * 20.) / 20.;
//    gl_FragColor = vec4(coords * .5 + .5, i, i);
//}
struct DirectionalLight
{
    lowp vec3 Color;
    lowp float AmbientIntensity;
    lowp vec3 Direction;
    lowp float DiffuseIntensity;
};

uniform sampler2D textureU;
uniform DirectionalLight directionalLightU;

varying lowp vec2 texcoordV;
varying lowp vec3 normalV;
varying lowp vec4 colV;

void main() {
    //    gl_FragColor = col;

    lowp vec4 ambientColor = vec4(directionalLightU.Color, 1.0) * directionalLightU.AmbientIntensity;
    float diffuseFactor = dot(normalize(normalV), -directionalLightU.Direction);

    lowp vec4 diffuseColor;
    if (diffuseFactor > 0.0) {
        diffuseColor = vec4(directionalLightU.Color, 1.0) * directionalLightU.DiffuseIntensity * diffuseFactor;
    }
    else {
        diffuseColor = vec4(0.0, 0.0, 0.0, 0.0);
    }

//    gl_FragColor = (texture2D(textureU, texcoordV)+vec4(0.2,0.2,0.2,1.0));
    gl_FragColor = texture2D(textureU, texcoordV) * (ambientColor + diffuseColor);
}
