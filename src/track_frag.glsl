// Allows the optimiser to run
#define highp

struct BaseLight
{
    highp vec3 Color;
    highp float AmbientIntensity;
    highp float DiffuseIntensity;
};

struct DirectionalLight
{
    BaseLight Base;

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

uniform DirectionalLight directionalLights[2];
uniform PointLight pointLights[3];
uniform vec4 Glow;

uniform highp vec3 eyeWorldPosU;
uniform highp float matSpecularIntensityU;
uniform highp float specularPowerU;

varying highp vec3 normalV;
varying highp vec3 posV;

highp vec3 ColorFromBaseLight(BaseLight light, vec3 dir, vec3 norm) {
    highp vec3 col = light.Color * light.AmbientIntensity;
    highp float diffuseFactor = dot(norm, -dir);
    highp float haveDiffuse = (0.5 + 0.5 * sign(diffuseFactor)) ; // used to do 0 if < 0
    diffuseFactor *= haveDiffuse ; // 0 if < 0

    // for Specular calcs
    highp float specularFactor ;
    highp vec3 vertexToEye = normalize(eyeWorldPosU - posV);
    highp vec3 lightReflect;

    lightReflect = normalize(reflect(dir, norm));
    specularFactor = dot(vertexToEye, lightReflect);
    specularFactor *= (0.5 + 0.5 * sign(specularFactor)) ; // 0 if < 0
    specularFactor *= haveDiffuse; // only use specular light if we have a diffuse component
    specularFactor = pow(specularFactor, specularPowerU); // pow param *must* be > 0 so do it after the check
    return col + light.Color * light.DiffuseIntensity * diffuseFactor + light.Color * matSpecularIntensityU * specularFactor;
}

highp vec3 ColorFromDirectionalLight(DirectionalLight light, vec3 norm) {
    return ColorFromBaseLight(light.Base, light.Direction, norm);
}

highp vec3 ColorFromPointLight(PointLight light, highp vec3 norm) {
    highp vec3 lightDirection = posV - light.Position;
    highp float Distance = length(lightDirection);
    lightDirection = normalize(lightDirection);
    highp vec3 Color = ColorFromBaseLight(light.Base, lightDirection, norm);

    highp float Attenuation = light.AConstant + light.ALinear * Distance +  light.AExp * Distance * Distance;
    return Color / Attenuation;
}

void main() {

    highp vec3 TotalColor;
    highp vec3 normal = normalize(normalV);

    // light 1
    TotalColor = ColorFromDirectionalLight(directionalLights[0], normal);

    // light 2
    TotalColor += ColorFromDirectionalLight(directionalLights[1], normal);

    //  Setup for point lights
    //        for (int i = 0 ; i < 3 ; i++) {
    TotalColor += ColorFromPointLight(pointLights[0], normal);
    TotalColor += ColorFromPointLight(pointLights[1], normal);
    TotalColor += ColorFromPointLight(pointLights[2], normal);
    //       }
//    highp vec3 lightDirection = posV - pointLights[0].Position;
//    lightDirection = normalize(lightDirection);
//    highp vec3 check = -lightDirection;
//    TotalColor = vec3(
//                dot(-normalize(posV - pointLights[0].Position), normal),
//                dot(-normalize(posV - pointLights[1].Position), normal),
//                dot(-normalize(posV - pointLights[2].Position), normal));
//    dot(check, vec3(-1.0, 0.0, 0.0)),
//    dot(check, vec3(0.0, -1.0, 0.0)),
//    dot(check, vec3(0.0, 0.0, -1.0)));

    //    TotalColor = ColorFromPointLight(0, normal);
    //    TotalColor = clamp(TotalColor, vec3(0.0,0.0,0.0), vec3(1.0, 1.0, 1.0));

    // Support an additional Glow which overrides the colour *and* alpha
    highp vec4 col = vec4(TotalColor, 1.0) * (vec4(0.5, 0.5, 0.5, 1.0) + Glow);
    col = clamp(col, vec4(0.0,0.0,0.0,0.0), vec4(1.0, 1.0, 1.0, 1.0));


    gl_FragColor = col;
//    gl_FragColor = texture2D(textureU, texcoordV) * vec4(TotalColor, 1.0) + vec4(TotalColor, 1.0); // super shiney
//    gl_FragColor = vec4(texcoordV, 0.5, 1.0) * vec4(TotalColor, 1.0); // colourfull
}

