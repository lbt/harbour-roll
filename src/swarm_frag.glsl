struct BaseLight
{
    highp vec3 Color;
    highp float AmbientIntensity;
    highp float DiffuseIntensity;
};

struct DirectionalLight
{
    highp vec3 Color;
    highp float AmbientIntensity;
    highp float DiffuseIntensity;

    highp vec3 Direction;
};


struct PointLight
{
    BaseLight Base;
    highp vec3 Position;
    highp float AConstant;
    highp float ALinear;
    highp float AExp;
};

uniform sampler2D textureU;
uniform DirectionalLight directionalLightU;
uniform DirectionalLight directional2LightU;

uniform PointLight pointLights[3];

uniform highp vec3 eyeWorldPosU;
uniform highp float matSpecularIntensityU;
uniform highp float specularPowerU;

varying highp vec2 texcoordV;
varying highp vec3 normalV;
varying highp vec3 posV;

highp vec3 ColorFromBaseLight(BaseLight light, vec3 dir, vec3 norm) {
    highp vec3 col = light.Color * light.AmbientIntensity;
    highp float diffuseFactor = dot(norm, dir);
    diffuseFactor *= (0.5 + 0.5 * sign(diffuseFactor)) ; // 0 if < 0

    // for Specular calcs
    highp float specularFactor ;
    highp vec3 vertexToEye = normalize(eyeWorldPosU - posV);
    highp vec3 lightReflect;

    lightReflect = normalize(reflect(dir, norm));
    specularFactor = dot(vertexToEye, lightReflect);
    specularFactor *= (0.5 + 0.5 * sign(specularFactor)) ; // 0 if < 0
    specularFactor = pow(specularFactor, specularPowerU); // pow param *must* be > 0 so do it after the check

    return col + light.Color * light.DiffuseIntensity * diffuseFactor + light.Color * matSpecularIntensityU * specularFactor;
}

highp vec3 ColorFromDirectionalLight(DirectionalLight light, vec3 dir, vec3 norm) {
    return ColorFromBaseLight(BaseLight(light.Color, light.AmbientIntensity, light.DiffuseIntensity), dir, norm);
}

highp vec3 ColorFromPointLight(int Index, highp vec3 norm) {
    highp vec3 lightDirection = posV - pointLights[Index].Position;
    highp float Distance = length(lightDirection);
    lightDirection = normalize(lightDirection);
    highp vec3 Color = ColorFromBaseLight(pointLights[Index].Base, lightDirection, norm);

    highp float Attenuation = pointLights[Index].AConstant + pointLights[Index].ALinear * Distance +  pointLights[Index].AExp * Distance * Distance;
    return Color / Attenuation;
}

void main() {

    highp vec3 TotalColor;
    highp vec3 normal = normalize(normalV);

    // light 1
    TotalColor = ColorFromDirectionalLight(directionalLightU, directionalLightU.Direction, normal);

    // light 2
    TotalColor += ColorFromDirectionalLight(directional2LightU, directional2LightU.Direction, normal);

    //  Setup for point lights
//        for (int i = 0 ; i < 3 ; i++) {
            TotalColor += ColorFromPointLight(1, normal);
            TotalColor += ColorFromPointLight(2, normal);
            TotalColor += ColorFromPointLight(3, normal);
 //       }

    gl_FragColor = texture2D(textureU, texcoordV) * vec4(TotalColor, 1.0);
}

