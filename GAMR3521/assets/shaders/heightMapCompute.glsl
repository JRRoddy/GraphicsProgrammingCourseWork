# version 460 core 



layout(local_size_x  = 16, local_size_y = 16) in;

layout(binding  = 0,rgba16f) uniform image2D outputImage; 


vec2 hash2(vec2 p);
float noise(in vec2 p);
float remap(float currValue, float  inMin,float inMax,float outMin, float outMax);
float FBM(vec2 position);
float ridgedNoise(vec2 position);
float turbulentNoise(vec2 position);
uniform float u_lacrunarity; 
uniform float u_persistance; 
uniform float u_frequency;
uniform float u_amplitude;

uniform int u_FBM;
uniform int u_useRidgedNoise;
uniform int u_turbulentNoise;
uniform int u_combination;
uniform int u_octaves;
uniform vec2 u_heightMapSize;
float shouldSmooth = 0.0;
void main()
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv =  vec2(pixelCoords) / vec2(imageSize(outputImage));

	
     
	 
	shouldSmooth =  float((uv.x <= 0.0 || uv.x >= 1.0 || uv.y <= 0.0|| uv.y >= 1.0));
   
    
   
	
	vec4 heightPacked ;
	
	if(u_FBM == 1)
	{
	  float fractal = FBM(uv);
	  fractal = remap(fractal,-1, 1, 0, 1); 


	  heightPacked = vec4(fractal);
	
	}
	if(u_useRidgedNoise == 1)
	{
	  float ridged = ridgedNoise(uv);
      ridged = remap(ridged,-1, 1, 0, 1);
	  heightPacked = vec4(ridged);
	}
	 if(u_turbulentNoise == 1)
	{
	  float turb = turbulentNoise(uv);
      turb = remap(turb,-1, 1, 0, 1);
	  heightPacked = vec4(turb);
	
	}
    if(u_combination == 1)
	{
	  float combo  = FBM(uv) + 0.3 * ridgedNoise(uv) + 0.4 * turbulentNoise(vec2(FBM(uv),uv.x));
	  combo = remap(combo,-1,1,0,1);
      heightPacked = vec4(combo);	
	}
	
	imageStore(outputImage,pixelCoords,heightPacked);

	
   


}


float FBM(vec2 position)
{
    float edgeScalar = 0.25;
    float total = 0.0f; 
	float totalAmplitude = 0.0f;

	float trueAmp =  u_amplitude;
	float amplitude = trueAmp;


	float frequency = u_frequency;
	float lacrunarity = u_lacrunarity;
	float persistance = u_persistance;
	
	for (int  i = 0; i < u_octaves;i++)
	{
	   
	   float noiseValue = (noise(position * frequency));

	   total += noiseValue * amplitude;
	   
	   frequency *= lacrunarity;
	   amplitude *= persistance;
	   totalAmplitude += amplitude;
	
	}




	return (total * trueAmp / totalAmplitude);
	 
    

}


float ridgedNoise(vec2 position)
{

    float total = 0.0f; 
	float totalAmplitude = 0.0f;
	float amplitude = u_amplitude; 
	float frequency = u_frequency;
	float lacrunarity = u_lacrunarity;
	for (int  i = 0; i < u_octaves;i++)
	{

	   float noise = noise(position * frequency) * u_amplitude ;

	   noise  = abs(noise);
	   noise = 1.0 - noise;
	   total += noise;
	  
	   


	   frequency *= lacrunarity ;
	  
	   totalAmplitude += amplitude;
	   
	  
	    
	
	}


	return (total * u_amplitude / totalAmplitude);
	 





}


float turbulentNoise(vec2 position)
{

   float totalAmp  = 0.0;
   float total = 0.0;
   float f = u_frequency;
   float a  = u_amplitude;
   for(int i = 0; i < u_octaves;i++)
   {
      
      float n = noise(position * f) * a; 

      n = abs(n);
     
	
      total += n * a;
      totalAmp += a;
	  f *= u_lacrunarity;
	  a *= u_persistance;
   }

   return (total * u_amplitude / totalAmp);





}


vec2 hash2(vec2 p) 
{
	p = vec2(dot(p, vec2(127.1, 311.7)),
		dot(p, vec2(269.5, 183.3)));

	return -1.0 + 2.0 * fract(sin(p) * 43758.5453123 );
}

float noise(in vec2 p)
{
	vec2 i = floor(p);
	vec2 f = fract(p);

	vec2 u = f * f * (3.0 - 2.0 * f);

	return mix(mix(dot(hash2(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),
		dot(hash2(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
		mix(dot(hash2(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),
			dot(hash2(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x), u.y);
}

float remap(float currValue, float  inMin,float inMax,float outMin, float outMax) {
	
	float t = (currValue - inMin) / (inMax - inMin);
	return mix(outMin, outMax, t);
}

