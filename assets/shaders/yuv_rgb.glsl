//#vertexShader
#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

out vec2 o_TexCoord;

void main()
{
	o_TexCoord = aTex;
	gl_Position = vec4(aPos, 0.0f, 1.0f);
}

//#fragmentShader
#version 330 core

out vec4 FragColor;
in vec2 o_TexCoord;

uniform sampler2D texture_y;  
uniform sampler2D texture_u;  
uniform sampler2D texture_v; 

vec3 yuv_to_rgb(float y, float u, float v)
{
    y = 1.1643 * ( y - 0.0625 );
    u = u - 0.5;
    v = v - 0.5;
    float rTmp = y + (1.5958 * v); 
    
    float gTmp = y - (0.39173 * u) - (0.81290 * v); 

    float bTmp = y + (2.017 * u);

    vec3 rgb;

    rgb.x = clamp(rTmp, 0.0f, 1.0f);
    rgb.y = clamp(gTmp, 0.0f, 1.0f);
    rgb.z = clamp(bTmp, 0.0f, 1.0f);

    return rgb;
}

void main() {  
    float y = texture(texture_y, o_TexCoord).r;  
    float u = texture(texture_u, o_TexCoord).r ;  
    float v = texture(texture_v, o_TexCoord).r ; 

    vec3 rgb = {0.0f};
    rgb = yuv_to_rgb(y, u, v);

    FragColor = vec4(rgb, 1.0f);  
}