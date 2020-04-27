#version 300 es

#define MAX_LIGHT_COUNT 10
#define PI 3.1415926535897932384626433832795

precision highp float;
precision highp int;

struct Light {
  vec4 position;
  vec3 direction;
  vec3 color;
  float energy;
  float angle;
};

struct Material {
  vec3 baseColor;
  float metallicFactor;
  float roughnessFactor;

  sampler2D albedoMap;
  sampler2D normalMap;
  sampler2D metallicMap;
  sampler2D roughnessMap;
  sampler2D ambientOcclusionMap;
};

in struct MeshInfo {
  vec3 vertPosition;
  vec2 vertTexcoords;
  mat3 vertTBNMatrix;
} fragMeshInfo;

uniform uint uniLightCount;
uniform Light uniLights[MAX_LIGHT_COUNT];

layout (std140) uniform uboCameraMatrices {
  mat4 viewMat;
  mat4 invViewMat;
  mat4 projectionMat;
  mat4 invProjectionMat;
  mat4 viewProjectionMat;
  vec3 cameraPos;
};

uniform Material uniMaterial;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec3 bufferNormal;

// Normal Distribution Function: Trowbridge-Reitz GGX
float computeNormalDistrib(vec3 normal, vec3 halfVec, float roughness) {
  float sqrRough  = roughness * roughness;
  float frthRough = sqrRough * sqrRough;

  float halfVecAngle    = max(dot(halfVec, normal), 0.0);
  float sqrHalfVecAngle = halfVecAngle * halfVecAngle;

  float divider = (sqrHalfVecAngle * (frthRough - 1.0) + 1.0);
  divider       = PI * divider * divider;

  return frthRough / max(divider, 0.001);
}

// Fresnel: Shlick
vec3 computeFresnel(float cosTheta, vec3 baseReflectivity) {
  // Seemingly optimized version:
  //    pow(2.0, (-5.55473 * cosTheta - 6.98316) * cosTheta)
  // From: http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
  return baseReflectivity + (1.0 - baseReflectivity) * pow(1.0 - cosTheta, 5.0);
}

// Shlick-Beckmann for Geometry part
float computeGeomShlickGGX(float angle, float roughness) {
  float incrRough   = (roughness + 1.0);
  float roughFactor = (incrRough * incrRough) / 8.0;

  float denom = angle * (1.0 - roughFactor) + roughFactor;

  return angle / denom;
}

// Geometry: Smith's Shlick GGX
float computeGeometry(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
  float viewAngle  = max(dot(viewDir, normal), 0.0);
  float lightAngle = max(dot(lightDir, normal), 0.0);

  float viewGeom  = computeGeomShlickGGX(viewAngle, roughness);
  float lightGeom = computeGeomShlickGGX(lightAngle, roughness);

  return viewGeom * lightGeom;
}

void main() {
  // Gamma correction for albedo (sRGB presumed)
  vec3 albedo     = pow(texture(uniMaterial.albedoMap, fragMeshInfo.vertTexcoords).rgb, vec3(2.2)) * uniMaterial.baseColor;
  float metallic  = texture(uniMaterial.metallicMap, fragMeshInfo.vertTexcoords).r * uniMaterial.metallicFactor;
  float roughness = texture(uniMaterial.roughnessMap, fragMeshInfo.vertTexcoords).r * uniMaterial.roughnessFactor;
  float ambOcc    = texture(uniMaterial.ambientOcclusionMap, fragMeshInfo.vertTexcoords).r;

  vec3 normal = texture(uniMaterial.normalMap, fragMeshInfo.vertTexcoords).rgb;
  normal      = normalize(normal * 2.0 - 1.0);
  normal      = normalize(fragMeshInfo.vertTBNMatrix * normal);

  vec3 viewDir = normalize(cameraPos - fragMeshInfo.vertPosition);

  // Base Fresnel (F)
  vec3 baseReflectivity = mix(vec3(0.04), albedo, metallic);

  vec3 lightRadiance = vec3(0.0);

  for (uint lightIndex = 0u; lightIndex < uniLightCount; ++lightIndex) {
    vec3 fullLightDir;
    float attenuation = uniLights[lightIndex].energy;

    if (uniLights[lightIndex].position.w != 0.0) {
      fullLightDir = uniLights[lightIndex].position.xyz - fragMeshInfo.vertPosition;

      float sqrDist = dot(fullLightDir, fullLightDir);
      attenuation  /= sqrDist;
    } else {
      fullLightDir = -uniLights[lightIndex].direction;
    }

    vec3 lightDir = normalize(fullLightDir);
    vec3 halfDir  = normalize(viewDir + lightDir);
    vec3 radiance = uniLights[lightIndex].color * attenuation;

    // Normal distrib (D)
    float normalDistrib = computeNormalDistrib(normal, halfDir, roughness);

    // Fresnel (F)
    vec3 fresnel = computeFresnel(max(dot(halfDir, viewDir), 0.0), baseReflectivity);

    // Geometry (G)
    float geometry = computeGeometry(normal, viewDir, lightDir, roughness);

    vec3 DFG         = normalDistrib * fresnel * geometry;
    float lightAngle = max(dot(lightDir, normal), 0.0);
    float divider    = 4.0 * max(dot(viewDir, normal), 0.0) * lightAngle;
    vec3 specular    = DFG / max(divider, 0.001);

    vec3 diffuse = vec3(1.0) - fresnel;
    diffuse     *= 1.0 - metallic;

    lightRadiance += (diffuse * albedo / PI + specular) * radiance * lightAngle;
  }

  vec3 ambient = vec3(0.03) * albedo * ambOcc;
  vec3 color   = ambient + lightRadiance;

  // HDR tone mapping
  color = color / (color + vec3(1.0));
  // Gamma correction
  color = pow(color, vec3(1.0 / 2.2));

  fragColor = vec4(color, metallic);

  // Sending fragment normal to next framebuffer(s), if any
  bufferNormal = normal;
}
