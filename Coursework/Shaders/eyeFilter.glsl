# version 150 core

uniform sampler2D diffuseTex;
uniform int useTexture;
uniform float iTime;

in Vertex {
	vec2 texCoord ;
	vec4 colour ;
} IN;
 
 out vec4 gl_FragColor ;
 
void main ( void ) {
	vec2 iResolution = vec2(1920.0, 1080.0);
	vec2 uv = IN.texCoord.xy / iResolution.xy;
	gl_FragColor = IN.colour;
	if( useTexture > 0) {
		gl_FragColor = texture ( diffuseTex , IN.texCoord );
	}
	
	vec4 noise = vec4(0.0,0.0,0.0,1.0);//texture(iChannel1, uv);
	
	vec4 combination;
	
	gl_FragColor.a = sin(iTime/1.5);//sin/cos - fade in and out, tan - blinding light
	
	/*combination = vec4(noise.x*(1.0-gl_FragColor.a) + gl_FragColor.x*gl_FragColor.a,
					noise.y*(1.0-gl_FragColor.a) + gl_FragColor.y*gl_FragColor.a,
					noise.z*(1.0-gl_FragColor.a) + gl_FragColor.z*gl_FragColor.a,
					gl_FragColor.a);*/
					
	combination = vec4(noise.xyz, gl_FragColor.a);				
	
	gl_FragColor = combination;
}