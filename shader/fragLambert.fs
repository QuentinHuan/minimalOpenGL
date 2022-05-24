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
  vec3 diffuse;
  vec3 specular;
  float shininess;
};
Material material;

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
  material.diffuse = vec3(0.8f, 0.7f, 0.7f);
  material.specular = vec3(1.0f, 1.0f, 1.0f);
  material.shininess = 32.0f;
  // ambient
  // vec3 ambient = light.ambient * material.ambient;
  vec3 ambient = light.ambient * material.diffuse;
  // diffuse
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * (diff * material.diffuse);

  // specular
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * material.specular);

  vec3 result = ambient + diffuse + specular;

  // result = vec3(1,1,1);
  FragColor = vec4(result, 1.0);
};