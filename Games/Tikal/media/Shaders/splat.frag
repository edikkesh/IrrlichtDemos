uniform sampler2D A1;
uniform sampler2D t0;
uniform sampler2D t1;
uniform sampler2D t2;
uniform sampler2D t3;
uniform sampler2D t4;

varying vec4 texCoord;

varying vec4 diffuse,ambientGlobal, ambient;
varying vec3 normal,lightDir,halfVector;
varying float dist;


//===========
uniform sampler2D ShadowMapSampler;
uniform vec4 LightColour;
varying float lightVal;

float testShadow(vec2 smTexCoord, vec2 offset, float realDistance)
{
	vec4 texDepth = texture2D( ShadowMapSampler, vec2( smTexCoord + offset));
	float extractedDistance = (texDepth.r + texDepth.g / 256.0);
	
	return (extractedDistance <= realDistance) ? (1.0  / float(4)) : 0.0;
}

vec2 offsetArray[16];

vec4 pixelMain(in vec4 SMPos, in vec4 MVar, in vec2 TexCoords)
{
	offsetArray[0] = vec2(0.0, 0.0);
	offsetArray[1] = vec2(0.0, 1.0);
	offsetArray[2] = vec2(1.0, 1.0);
	offsetArray[3] = vec2(-1.0, -1.0);
	offsetArray[4] = vec2(-2.0, 0.0);
	offsetArray[5] = vec2(0.0, -2.0);
	offsetArray[6] = vec2(2.0, -2.0);
	offsetArray[7] = vec2(-2.0, 2.0);
	offsetArray[8] = vec2(3.0, 0.0);
	offsetArray[9] = vec2(0.0, 3.0);
	offsetArray[10] = vec2(3.0, 3.0);
	offsetArray[11] = vec2(-3.0, -3.0);
	offsetArray[12] = vec2(-4.0, 0.0);
	offsetArray[13] = vec2(0.0, -4.0);
	offsetArray[14] = vec2(4.0, -4.0);
	offsetArray[15] = vec2(-4.0, 4.0);

    SMPos.xy  = SMPos.xy / SMPos.w / 2.0 + vec2(0.5, 0.5);

	vec4 finalCol = vec4(0.0, 0.0, 0.0, 0.0);
	vec2 clampedSMPos = clamp(SMPos.xy, vec2(0.0, 0.0), vec2(1.0, 1.0));
	float lightFactor = 1.0;
	float realDist = MVar.x / MVar.z - 0.002;

	// If this point is within the light's frustum.
    if(clampedSMPos.x == SMPos.x && clampedSMPos.y == SMPos.y && SMPos.z > 0.0 && SMPos.z < MVar.z)
	{
		for(int i = 0;i < 4; i++)
			lightFactor -= testShadow(SMPos.xy, offsetArray[i] * MVar.w, realDist);

		// Multiply with diffuse.
##ifdef ROUND_SPOTLIGHTS
		finalCol = LightColour * lightFactor * MVar.y * clamp(5.0 - 10.0 * length(SMPos.xy - vec2(0.5, 0.5)), 0.0, 1.0);
##else
		finalCol = LightColour * lightFactor * MVar.y;
##endif
	}

	return finalCol;
}
//==================


vec4 splat(void)
{
	vec4 a1   = texture2D( A1, texCoord.xy );
	vec4 tex0    = texture2D( t0, texCoord.xy*30 );//underForest
	vec4 tex1    = texture2D( t1, texCoord.xy*30 );//sand
	vec4 tex2    = texture2D( t2, texCoord.xy*35 );//grass
	vec4 tex3    = texture2D( t3, texCoord.xy*100 );
	vec4 tex4    = texture2D( t4, texCoord.xy );

	tex1 = mix( tex0, tex1, a1.r );
	tex2 = mix( tex1, tex2, a1.g );
	tex2 = mix( tex3, tex2, 1-a1.b);


	vec3 n,halfV,viewV,ldir;
	float NdotL,NdotHV;
	vec4 color = ambientGlobal;
	float att;
		
	n = normalize(normal);
		
	NdotL = max(dot(n,normalize(lightDir)),0.0);

	att = 1.0 / (gl_LightSource[0].constantAttenuation +
			gl_LightSource[0].linearAttenuation * dist +
			gl_LightSource[0].quadraticAttenuation * dist * dist);
	color += att * (diffuse * NdotL + ambient);
	pow(NdotHV,gl_FrontMaterial.shininess);
	
	return tex2*tex4.a;// * color;
}


// Main entry point.
void main() 
{

	gl_FragColor = splat()-pixelMain(vec4(gl_TexCoord[0]), vec4(gl_TexCoord[1]), vec2(gl_TexCoord[2].xy));
}


