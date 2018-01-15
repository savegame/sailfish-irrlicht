uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform int isFlipped;
varying highp vec2 TexCoord0;
varying highp vec2 ScreenPos;

//highp float rand(lowp vec2 co){
//    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
//}

precision lowp    float;


// Irrationals with precision shifting
//
float PHI = 1.61803398874989484820459 * 00000.1; // Golden Ratio
float PI  = 3.14159265358979323846264 * 00000.1; // PI
float SRT = 1.41421356237309504880169 * 10000.0; // Square Root of Two


// Gold Noise function
//
float gold_noise(in vec2 coordinate, in float seed)
{
 return fract(sin(dot(coordinate*seed, vec2(PHI, PI)))*SRT);
}

vec4 gaussianBlur(sampler2D texture, vec2 uv, float radius, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 step = radius / resolution * direction;
  color += texture2D(texture, uv - 4.0 * step) * 0.02699548325659403;
  color += texture2D(texture, uv - 3.0 * step) * 0.06475879783294587;
  color += texture2D(texture, uv - 2.0 * step) * 0.12098536225957168;
  color += texture2D(texture, uv - 1.0 * step) * 0.17603266338214976;
  color += texture2D(texture, uv) * 0.19947114020071635;
  color += texture2D(texture, uv + 1.0 * step) * 0.17603266338214976;
  color += texture2D(texture, uv + 2.0 * step) * 0.12098536225957168;
  color += texture2D(texture, uv + 3.0 * step) * 0.06475879783294587;
  color += texture2D(texture, uv + 4.0 * step) * 0.02699548325659403;
  return color;
}

void main(void)
{
    //highp vec2 nTexCoord = vec2(ScreenPos.y*0.5, ScreenPos.x);
    //if(isFlipped == 1)
    //    nTexCoord = vec2(ScreenPos.y*0.5, rand(ScreenPos)-ScreenPos.x);
    //gl_FragColor = texture2D(Texture0, nTexCoord);

    //gl_FragColor =  texture2D(Texture0, ScreenPos.st );

    highp vec2 nTexCoord = vec2(-ScreenPos.y, ScreenPos.x);
    if( isFlipped == 1 )
        nTexCoord = vec2(ScreenPos.y, -ScreenPos.x);

    if( TexCoord0.y < 0.5 )
    {
        gl_FragColor = texture2D(Texture0, nTexCoord);
    }
    else
    {
        //lowp float r1 = gold_noise(nTexCoord, 5.0f);
        //lowp float r2 = gold_noise(nTexCoord.yx, 5.0f);
       // nTexCoord = vec2(/*nTexCoord.x + */r1,/*nTexCoord.y - */r2);

        //gl_FragColor = vec4(r1,1,r2,1);//texture2D(Texture0, nTexCoord);
        gl_FragColor = gaussianBlur(Texture0, nTexCoord, 4, vec2(800,480), vec2(1,0.7) );
    }
        //gl_FragColor = vec4(TexCoord0.xy, 1, 1);
}
