#version 330 core
//---------------------------------------------
//                  IN and OUT
//---------------------------------------------
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

//---------------------------------------------
//                  Camera
//---------------------------------------------

uniform vec3 viewPos;
//---------------------------------------------
//                  Material
//---------------------------------------------
struct Material {
  vec3 albedo;
  float roughness;
  float metalness;
};
uniform Material material;

//---------------------------------------------
//                  light
//---------------------------------------------
struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
uniform Light light;

struct DirLight {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
uniform DirLight dirLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main() {
  //----------------------||
  //        vectors
  //----------------------||
  vec3 n = normalize(Normal); // normal vector

  vec3 v = normalize(viewPos - FragPos);        // view vector
  vec3 l = normalize(light.position - FragPos); // light direction
  vec3 h = normalize(l + v);

  //----------------------||
  //       textures
  //----------------------||
  vec3 albedo = material.albedo;
  albedo.x = pow(albedo.x, 2.2f);
  albedo.y = pow(albedo.y, 2.2f);
  albedo.z = pow(albedo.z, 2.2f);
  //----------------------||
  //  Specular Component
  //----------------------||
  float alpha = material.roughness;
  float alphaSqr = pow(alpha, 2);
  float metallic = material.metalness;

  float D = alphaSqr /
            (3.14 * pow(pow(max(dot(n, h), 0.0f), 2) * (alphaSqr - 1) + 1, 2));

  float NdotV = max(dot(n, v), 0.0f);
  float k = pow(alpha + 1, 2) / 8; // direct light
  float G1 = NdotV / (NdotV * (1 - k) + k);

  float NdotL = max(dot(n, l), 0.0f);
  float G2 = NdotL / (NdotL * (1 - k) + k);
  float G = G1 * G2;

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);
  vec3 F = F0 + (1 - F0) * pow(1 - max(dot(h, v), 0.0f), 5);

  vec3 specular =
      (D * F * G) /
      max(4.0f * max(dot(v, n), 0.0f) * max(dot(l, n), 0.0f), 0.001);

  //----------------------||
  //   Diffuse Component
  //----------------------||
  vec3 kd = vec3(1.0f) - F;
  kd = kd * (1.0f - metallic);

  float NdotOmega = max(dot(n, l), 0.0);
  vec3 diffuse = kd * albedo / 3.14;

  vec3 result = 1 * (1 * diffuse + 1 * specular) * NdotOmega * light.diffuse +
                (1 * albedo * light.ambient);
  result = result / (result + vec3(1.0));
  result = pow(result, vec3(1.0 / 2.2));

  FragColor = vec4(result.rgb, 1);
  // FragColor = vec4(texture(material.texture_roughness1, TexCoords).rgb, 1);
};