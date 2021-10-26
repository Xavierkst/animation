#version 330 core

in vec3 fragNormal;

uniform vec3 AmbientColor = vec3(0.2);
uniform vec3 LightDirection = normalize(vec3(2.0f, 8.0f, -4.0));
uniform vec3 LightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 DiffuseColor;	

uniform vec3 secondLightDirection = normalize(vec3(-1, -5, 2)); 
uniform vec3 secondLightColor = vec3(.0f/255.0f, 255.0f/255.0f, .0f/255.0f);

uniform vec3 thirdLightDirection = normalize(vec3(-1, 5, -3)); 
uniform vec3 thirdLightColor = vec3(1.0f, .0f/255.0f, .0f/255.0f);

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
	// Compute irradiance (sum of ambient & direct lighting)
	vec3 irradiance = AmbientColor + LightColor * max(0, dot(LightDirection, fragNormal));

	// Diffuse reflectance
	vec3 reflectance = ( irradiance ) * DiffuseColor;

	// What about color clamping? 

	// Gamma correction
	fragColor = vec4(sqrt(reflectance), 1);
}