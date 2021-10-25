#version 330 core

struct Material {
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct dirLight {
	vec4 direction;
	vec3 ambient;
	vec3 diffuse; 
	vec3 specular;
};

struct pointLight {
	vec4 position;
	vec3 ambient;
	vec3 diffuse; 
	vec3 specular;

	float k_constant;
	float k_linear;
	float k_quad;
};

out vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPos;

vec3 AmbientColor = vec3(0.2);
vec3 LightDirection = vec3(-2.0f, -8.0f, -4.0);
vec3 LightColor = vec3(1.0f, 1.0f, 1.0f);
vec3 DiffuseColor = vec3(1.0f, 1.0f, 1.0f);	

uniform pointLight pt_light;
uniform dirLight dir_light;
uniform Material material;
uniform vec3 view_position;

// Function prototypes
vec3 calcDirLight(dirLight dir_light, Material mat, vec4 normal, vec4 view_dir);
vec3 calcPointLight(pointLight point_light, Material mat, vec4 normal, vec4 frag_pos, vec4 view_dir);

void main()
{
	// Compute irradiance (sum of ambient & direct lighting)
	vec3 normal = normalize(fragNormal);
	vec3 view_dir = normalize(view_position - fragPos);
	vec3 dir_to_light = normalize(-LightDirection);

	// Diffuse reflectance
	// vec3 result = vec3(0.2f) * material.diffuse + diff * material.diffuse * dir_light.diffuse;
	// fragColor = vec4(sqrt(result), 1);

	float diff = max(dot(dir_to_light, normal), .0f);
	vec3 result = calcPointLight(pt_light, material, vec4(normal, .0f), vec4(fragPos, 1.0f), vec4(view_dir, .0f));
	result += calcDirLight(dir_light, material, vec4(normal, .0f), vec4(view_dir, .0f));

	fragColor = vec4(result, 1.0f);
}

vec3 calcDirLight(dirLight dir_light, Material mat, vec4 normal, vec4 view_dir) {
	vec4 norm = normalize(normal);
	vec4 light_dir = normalize(-dir_light.direction);
	vec4 refl_dir = reflect(-light_dir, norm);

	// vec3 ambient = mat.ambient * dir_light.ambient;
	vec3 ambient = mat.diffuse.rgb * dir_light.ambient;
	vec3 diffuse = max(dot(light_dir, norm), 0.0f) * mat.diffuse.rgb * dir_light.diffuse;
	vec3 specular = pow(max(dot(view_dir.xyz, refl_dir.xyz), .0f), mat.shininess) * mat.specular.rgb * dir_light.specular;
	
	// dir lights are infinitely far away, with a constant brightness on all objects, 
	// hence theres no attenuation from infinitely far away light sources?
	vec3 result = ambient + diffuse + specular;
	// texture(texSampler2Dnum, texCoords);
	return result;
}

vec3 calcPointLight(pointLight point_light, Material mat, vec4 normal, vec4 frag_pos, vec4 view_dir) {
	float dist_to_light = length(point_light.position - frag_pos);
	vec4 norm = normalize(normal);
	vec4 light_dir = normalize(point_light.position - frag_pos);
	vec4 refl_dir = reflect(-light_dir, norm);

	vec3 ambient = mat.diffuse.rgb * point_light.ambient;
	vec3 diffuse = max(dot(light_dir, norm), 0.0f) * mat.diffuse * point_light.diffuse;
	vec3 specular = pow(max(dot(view_dir, refl_dir), .0f), mat.shininess) * mat.specular * point_light.specular;
	
	// Point lights have to be attenuated, since dist to light is some finite distance
	float attenuation = 1.0 / (point_light.k_constant + point_light.k_linear * dist_to_light + point_light.k_quad * pow(dist_to_light, 2.0f));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	vec3 result = ambient + diffuse + specular;
	
	return result;
}
