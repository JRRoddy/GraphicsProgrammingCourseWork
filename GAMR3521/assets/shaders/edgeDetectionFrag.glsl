# version 460 


// edge detection using the sobel filter and convolution matrices 


layout(location =  0) out vec4 edgeColour;


in vec2 screenTexCoords;

uniform sampler2D u_colourBufferTexture;

uniform int u_active;


// convolution matrix for detecting horizontal edges
mat3 xGradient = mat3 
(
    1.0,  2.0,  1.0, 
    0.0,  0.0,  0.0, 
   -1.0, -2.0, -1.0 


);
// convolution matrix for detecting vertical edges
mat3 yGradient = mat3
(
    1.0,  0.0,  -1.0, 
    2.0,  0.0,  -2.0, 
    1.0,  0.0,  -1.0 
);





void main()
{

// store the legnth of each of the pixels we are sampling within our 3x3 grid 
// these are used to detect harsh changes in colour gradient which would potentially indicate an edge 
  mat3 colourGradients;
  
  // itterating through each pixel in the 3x3 grid we are sampling from
  for(int x = 0; x < 3 ;x++)
  {
    
    for(int y = 0; y < 3; y++)
    {
       // get the offsets we will apply to the current screen coordinate
       int uvx = x - 1;
       int uvy = y - 1;
       // get the current offset pixel coordinate 
       vec3 sampleColour = texelFetch(u_colourBufferTexture, ivec2(gl_FragCoord) + ivec2(uvx,uvy),0).rgb;
       // clamp the sample coords to ensure we dont go out of range 

       // calculate the gradient of each colour 
       colourGradients[x][y] = length(sampleColour);


    }
     
  
  }

  
  // sample the original colour 
  vec3 colourBufferTextureSample = texture(u_colourBufferTexture,screenTexCoords).rgb;
  
  /// here we multiply each column of the colour gradient matrix we established through each column of the xGradient sobel filter matrix 
  // summing up each dot product in order to obtain our final gradient(multipled through the xGradient filter used in sobel edge detection as 
  //the x gradient filter is designed to detect horizontal edges)
  float xSum = dot(xGradient[0],colourGradients[0]) + dot(xGradient[1],colourGradients[1]) + dot(xGradient[2],colourGradients[2]);
  // same above but using the yGradient matrix to detect vertical edges
  float ySum = dot(yGradient[0],colourGradients[0]) + dot(yGradient[1],colourGradients[1]) + dot(yGradient[2],colourGradients[2]);

  // we get the final colour gradient lenght  taking into account both axis this time
  float edgeDetectionResult = sqrt( pow(xSum,2.0) + pow(ySum,2.0) );

  float isActive = float(u_active);
  // take the difference between our curret colour and the final edge detectioncolour gradient lenght as a vec3 
  // if the distance for our original colour is greater the colour will show up more as the orignal 
  // where as if the colour is closer to the edgeDetectionResult as a vec3 it will be closer to 0 when we subtract the two 
  // this ensures we keep some of the orignal colour meaning edges will show up more 
  vec3 edgeDetectionColour = colourBufferTextureSample - vec3(edgeDetectionResult);

  // clamp the final result to ensure it stays in 0-1 range
  edgeDetectionColour = clamp(edgeDetectionColour,0.0,1.0);
  edgeColour =  vec4(edgeDetectionColour,1.0) * isActive + vec4(colourBufferTextureSample,1.0) * (1.0 - isActive);  


}