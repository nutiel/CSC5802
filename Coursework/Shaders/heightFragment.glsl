#version 150
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	vec4 height;
} IN;

out vec4 gl_FragColor;

void main(void){
	vec4 tex = texture(diffuseTex, IN.texCoord);
	
	vec4 heigN;
	
	heigN.x = IN.height.x/(256.0*1.25);
	heigN.y = IN.height.y/(255.0*1.25);
	heigN.z = IN.height.z/(256.0*1.25);
	heigN.w = 1.0;
	
	/*if(heigN.y > 0.8){
		tex = vec4(tex.x*(1.0+heigN.y), tex.y, tex.z, tex.w);
	}else if(heigN.y > 0.6){
		tex = vec4(tex.x*(0.5+heigN.y + 0.1), tex.y*(0.5+heigN.y + 0.1), tex.z, tex.w);
	}else if(heigN.y > 0.4){
		tex = vec4(tex.x, tex.y*(1.0+heigN.y + 0.3), tex.z, tex.w);
	}else if(heigN.y > 0.2){
		tex = vec4(tex.x, tex.y*(0.5+heigN.y + 0.5), tex.z*(0.5+heigN.y + 0.5), tex.w);
	}else{
		tex = vec4(tex.x, tex.y, tex.z*(1.0+heigN.y + 0.8), tex.w);
	}*/
	
	if(heigN.y > 0.8){
		tex = vec4(tex.x, tex.y*(heigN.y), tex.z*(heigN.y), tex.w);
	}else if(heigN.y > 0.6){
		tex = vec4(tex.x, tex.y, tex.z*(heigN.y + 0.2), tex.w);
	}else if(heigN.y > 0.4){
		tex = vec4(tex.x*(heigN.y + 0.4), tex.y, tex.z*(heigN.y + 0.4), tex.w);
	}else if(heigN.y > 0.2){
		tex = vec4(tex.x*(heigN.y + 0.6), tex.y, tex.z, tex.w);
	}else{
		tex = vec4(tex.x*(heigN.y + 0.8), tex.y*(heigN.y + 0.8), tex.z, tex.w);
	}
	
	gl_FragColor = tex;
}