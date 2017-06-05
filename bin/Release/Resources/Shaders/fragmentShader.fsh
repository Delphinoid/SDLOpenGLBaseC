#version 330 core

in vec2 UV;
uniform sampler2D textureSampler0;
uniform float alpha;
out vec4 colour;

void main(){
	vec4 tempColour = texture(textureSampler0, UV).rgba;
	tempColour.a *= alpha;
	if(tempColour.a == 0.0){
		discard;
	}
	colour = tempColour;
}