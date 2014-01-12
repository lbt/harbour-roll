struct DirectionalLight
{
    highp vec3 Color;
    highp float AmbientIntensity;
    highp vec3 Direction;
    highp float DiffuseIntensity;
};

uniform sampler2D textureU;
uniform DirectionalLight directionalLightU;

uniform highp vec3 eyeWorldPosU;
uniform highp float matSpecularIntensityU;
uniform highp float specularPowerU;

varying highp vec2 texcoordV;
varying highp vec3 normalV;
varying highp vec3 posV;

void main2() {

    highp vec3 ambientColor = vec3(directionalLightU.Color * directionalLightU.AmbientIntensity);

    highp vec3 diffuseColor = vec3(0.0, 0.0, 0.0);
    highp vec3 lightDirection = -directionalLightU.Direction;
    highp vec3 normal = normalize(normalV);
    highp float diffuseFactor = dot(normal, lightDirection);
    diffuseFactor *= (0.5 + 0.5 * sign(diffuseFactor)) ; // 0 if < 0
    diffuseColor = directionalLightU.Color * directionalLightU.DiffuseIntensity * diffuseFactor;

    highp vec3 specularColor = vec3(0.0, 0.0, 0.0);
    highp vec3 vertexToEye = normalize(eyeWorldPosU - posV);
    highp vec3 lightReflect = normalize(reflect(directionalLightU.Direction, normal));
    highp float specularFactor = dot(vertexToEye, lightReflect);
    specularFactor = pow(specularFactor, specularPowerU);
    specularFactor *= (0.5 + 0.5 * sign(specularFactor)) ; // 0 if < 0
    specularColor = directionalLightU.Color * matSpecularIntensityU * specularFactor;
    highp vec4 mix = vec4(ambientColor + diffuseColor + specularColor, 1.0);
    gl_FragColor = texture2D(textureU, texcoordV) * mix;
    return;
}

void main() {

    highp vec4 c;

    highp vec3 ambientColor = vec3(directionalLightU.Color * directionalLightU.AmbientIntensity);

    highp vec3 lightDirection = -directionalLightU.Direction;
    highp vec3 normal = normalize(normalV);
    highp float diffuseFactor = dot(normal, lightDirection);
    highp vec3 diffuseColor = vec3(0.0, 0.0, 0.0);

    diffuseFactor *= (0.5 + 0.5 * sign(diffuseFactor)) ; // 0 if < 0
    diffuseColor = vec3(directionalLightU.Color * directionalLightU.DiffuseIntensity * diffuseFactor);

    highp vec3 specularColor = highp vec3(0.0, 0.0, 0.0);
    highp float specularFactor ;
    highp vec3 vertexToEye = normalize(eyeWorldPosU - posV);
    highp vec3 lightReflect = normalize(reflect(directionalLightU.Direction, normal));
    specularFactor = dot(vertexToEye, lightReflect);
    specularFactor *= (0.5 + 0.5 * sign(specularFactor)) ; // 0 if < 0
    specularFactor = pow(specularFactor, specularPowerU); // pow param *must* be > 0
    specularColor = directionalLightU.Color * matSpecularIntensityU * specularFactor;

    gl_FragColor = texture2D(textureU, texcoordV) * vec4(ambientColor + diffuseColor + specularColor, 1.0);
}

