//#vertexShader
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

uniform vec4 i_Color;

out vec4 o_Color;
out vec2 o_TexCoord;

void main()
{
	o_TexCoord = aTex;
	o_Color = i_Color;
	gl_Position = vec4(aPos, 0.0f, 1.0f);
}

//#fragmentShader
#version 330 core

in vec4 o_Color;
in vec2 o_TexCoord;

out vec4 fragColor;

void main()
{
	fragColor = o_Color;
}