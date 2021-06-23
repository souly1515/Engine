#version 330 core  
layout (location = 0) in vec3 position; // The position variable has attribute position 0 
layout (location = 1) in vec4 color; // The color variable has attribute position 1  
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 worldTrans;
uniform vec4 uColor;

out vec4 vertexColor; // Specify a color output to the fragment shader

void main(void) { 
  mat4 transform2 = worldTrans * transform;

  gl_Position = transform2 * vec4(position, 1.0); // see how we directly give a vec3 to vec4's constructor
  vertexColor = uColor; // set the output variable to a dark-red color
}