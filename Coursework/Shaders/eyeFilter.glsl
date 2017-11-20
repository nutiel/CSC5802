# version 150 core

uniform sampler2D diffuseTex;
uniform int useTexture;
uniform float iTime;
uniform int switchScene;

in Vertex {
	vec2 texCoord ;
	vec4 colour ;
} IN;
 
 out vec4 gl_FragColor ;
 
void main ( void ) {
	
	vec4 combination;
	
	if(switchScene == 1 || switchScene == 2){
		vec2 iResolution = vec2(1920.0, 1080.0);
		vec2 uv = IN.texCoord.xy / iResolution.xy;
		gl_FragColor = IN.colour;
		if( useTexture > 0) {
			gl_FragColor = texture ( diffuseTex , IN.texCoord );
		}
	
		vec4 noise = vec4(0.0,0.0,0.0,1.0);//texture(iChannel1, uv);
	
		gl_FragColor.a = sin(iTime);//sin/cos - fade in and out, tan - blinding light
					
		combination = vec4(noise.xyz, gl_FragColor.a);
	}
	else{
		combination = IN.colour;
	}
	
	gl_FragColor = combination;
}