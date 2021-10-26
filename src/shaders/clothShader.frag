#version 330 core

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
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
in vec2 tex_coord;

uniform pointLight pt_light1;
uniform pointLight pt_light2;
uniform dirLight dir_light;
uniform Material material;
uniform vec3 view_position;
// uniform sampler2D clothTexture;

// Function prototypes
vec3 calcDirLight(dirLight dir_light, Material mat, vec4 normal, vec4 view_dir);
vec3 calcPointLight(pointLight point_light, Material mat, vec4 normal, vec4 frag_pos, vec4 view_dir);

void main()
{
	// Compute irradiance (sum of ambient & direct lighting)
	vec3 normal = normalize(fragNormal);
	vec3 view_dir = normalize(view_position - fragPos);

	// Diffuse reflectance
	// float diff = max(dot(dir_to_light, normal), .0f);
	// vec3 result = vec3(0.2f) * material.diffuse + diff * material.diffuse * dir_light.diffuse;
	// fragColor = vec4(sqrt(result), 1);

	vec3 result; 
	result = calcPointLight(pt_light1, material, vec4(normal, .0f), vec4(fragPos, 1.0f), vec4(view_dir, .0f));
	result += calcPointLight(pt_light2, material, vec4(normal, .0f), vec4(fragPos, 1.0f), vec4(view_dir, .0f));
	// result += calcDirLight(dir_light, material, vec4(normal, .0f), vec4(view_dir, .0f));

	fragColor = vec4(result, 1.0f);
	// fragColor = vec4(texture(material.texture_diffuse1, tex_coord));
}

vec3 calcDirLight(dirLight dir_light, Material mat, vec4 normal, vec4 view_dir) {
	vec4 norm = normalize(normal);
	vec4 light_dir = normalize(-dir_light.direction);
	vec4 refl_dir = reflect(-light_dir, norm);

	// vec3 ambient = mat.ambient * dir_light.ambient;
	vec3 ambient = texture(mat.texture_diffuse1, tex_coord).rgb * dir_light.ambient;
	vec3 diffuse = max(dot(light_dir, norm), 0.0f) * texture(mat.texture_diffuse1, tex_coord).rgb * dir_light.diffuse;
	vec3 specular = pow(max(dot(view_dir.xyz, refl_dir.xyz), .0f), mat.shininess) * texture(mat.texture_specular1, tex_coord).rgb * dir_light.specular;
	
	// dir lights are infinitely far away, with a constant brightness on all objects, 
	// hence theres no attenuation from infinitely far away light sources?
	vec3 result = ambient + diffuse + specular; 

	return result;
}

vec3 calcPointLight(pointLight point_light, Material mat, vec4 normal, vec4 frag_pos, vec4 view_dir) {
	float dist_to_light = length(point_light.position - frag_pos);
	vec4 norm = normalize(normal);
	vec4 light_dir = normalize(point_light.position - frag_pos);
	vec4 refl_dir = reflect(-light_dir, norm);

	// vec3 ambient = texture(mat.texture_diffuse1, tex_coord).rgb * point_light.ambient;
	vec3 ambient = texture(mat.texture_diffuse1, tex_coord).rgb * point_light.ambient;
	// vec3 diffuse = max(dot(light_dir, norm), 0.0f) * texture(mat.texture_diffuse1, tex_coord).rgb * point_light.diffuse;
	vec3 diffuse = texture(mat.texture_diffuse1, tex_coord).rgb * point_light.diffuse;
	vec3 specular = pow(max(dot(view_dir, refl_dir), .0f), mat.shininess) * texture(mat.texture_specular1, tex_coord).rgb * point_light.specular;
	
	// Point lights have to be attenuated, since dist to light is some finite distance
	float attenuation = 1.0 / (point_light.k_constant + point_light.k_linear * dist_to_light + point_light.k_quad * pow(dist_to_light, 2.0f));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	vec3 result = ambient + diffuse + specular; 
	
	return result;
}
