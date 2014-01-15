struct DirectionalLight
{
    highp vec3 Color;
    highp float AmbientIntensity;
    highp vec3 Direction;
    highp float DiffuseIntensity;
};

uniform sampler2D textureU;
uniform DirectionalLight directionalLightU;
uniform DirectionalLight directional2LightU;

uniform highp vec3 eyeWorldPosU;
uniform highp float matSpecularIntensityU;
uniform highp float specularPowerU;

varying highp vec2 texcoordV;
varying highp vec3 normalV;
varying highp vec3 posV;


void main() {

    highp vec3 ambientColor = vec3(directionalLightU.Color * directionalLightU.AmbientIntensity);
    highp vec3 ambientColor2 = vec3(directional2LightU.Color * directional2LightU.AmbientIntensity);

    highp vec3 lightDirection = -directionalLightU.Direction;
    highp vec3 normal = normalize(normalV);
    highp float diffuseFactor = dot(normal, lightDirection);
    highp vec3 diffuseColor = vec3(0.0, 0.0, 0.0);
    diffuseFactor *= (0.5 + 0.5 * sign(diffuseFactor)) ; // 0 if < 0
    diffuseColor = vec3(directionalLightU.Color * directionalLightU.DiffuseIntensity * diffuseFactor);

    lightDirection = -directional2LightU.Direction;
    diffuseFactor = dot(normal, lightDirection);
    highp vec3 diffuseColor2 = vec3(0.0, 0.0, 0.0);
    diffuseFactor *= (0.5 + 0.5 * sign(diffuseFactor)) ; // 0 if < 0
    diffuseColor2 = vec3(directional2LightU.Color * directional2LightU.DiffuseIntensity * diffuseFactor);

    highp vec3 specularColor = highp vec3(0.0, 0.0, 0.0);
    highp float specularFactor ;
    highp vec3 vertexToEye = normalize(eyeWorldPosU - posV);
    highp vec3 lightReflect = normalize(reflect(directionalLightU.Direction, normal));
    specularFactor = dot(vertexToEye, lightReflect);
    specularFactor *= (0.5 + 0.5 * sign(specularFactor)) ; // 0 if < 0
    specularFactor = pow(specularFactor, specularPowerU); // pow param *must* be > 0 so do it after the check
    specularColor = directionalLightU.Color * matSpecularIntensityU * specularFactor;

    highp vec3 specularColor2 = highp vec3(0.0, 0.0, 0.0);
    lightReflect = normalize(reflect(directional2LightU.Direction, normal));
    specularFactor = dot(vertexToEye, lightReflect);
    specularFactor *= (0.5 + 0.5 * sign(specularFactor)) ; // 0 if < 0
    specularFactor = pow(specularFactor, specularPowerU); // pow param *must* be > 0 so do it after the check
    specularColor2 = directional2LightU.Color * matSpecularIntensityU * specularFactor;

    gl_FragColor = texture2D(textureU, texcoordV) * vec4(ambientColor + diffuseColor + specularColor + ambientColor2 + diffuseColor2 + specularColor2, 1.0);
}

