#version 330 core

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 fragNormal;

// uniforms used for lighting
uniform vec3 AmbientColor = vec3(0.2);
uniform vec3 LightDirection = normalize(vec3(2.0f, 8.0f, -4.0));
//uniform vec3 LightColor = vec3(221/255.0f, 130/255.0f, 238/255.0f);
uniform vec3 LightColor = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 DiffuseColor;	
// passed in from c++ side NOTE: you can also set the value here and then remove 
// color from the c++ side

uniform vec3 secondLightDirection = normalize(vec3(-1, -5, 2)); 
uniform vec3 secondLightColor = vec3(.0f/255.0f, 255.0f/255.0f, .0f/255.0f);
//uniform vec3 secondLightColor = vec3(0.5f, 1.0f, 0.0f);

uniform vec3 thirdLightDirection = normalize(vec3(-1, 5, -3)); 
uniform vec3 thirdLightColor = vec3(1.0f, .0f/255.0f, .0f/255.0f);

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
	// Compute irradiance (sum of ambient & direct lighting)
	vec3 irradiance = AmbientColor 
//		+ thirdLightColor * max(0, dot(thirdLightDirection, fragNormal)) 
//		+ secondLightColor * max(0, dot(secondLightDirection, fragNormal)) 
		+ LightColor * max(0, dot(LightDirection, fragNormal));

	// Diffuse reflectance
	vec3 reflectance = ( irradiance ) * DiffuseColor;

	// What about color clamping? 

	// Gamma correction
	fragColor = vec4(sqrt(reflectance), 1);
	//	fragColor = vec4(vec3(0.5, 0.5f, 0.5f), 1);
}