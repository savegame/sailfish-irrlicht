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
//float PHI = 1.61803398874989484820459 * 00000.1; // Golden Ratio
//float PI  = 3.14159265358979323846264 * 00000.1; // PI
//float SRT = 1.41421356237309504880169 * 10000.0; // Square Root of Two


// Gold Noise function
//
//float gold_noise(in vec2 coordinate, in float seed)
//{
// return fract(sin(dot(coordinate*seed, vec2(PHI, PI)))*SRT);
//}

vec4 gaussianBlur(in sampler2D texture, in vec2 uv, in float radius, in vec2 resolution, in vec2 direction) {
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
    highp vec2 nTexCoord = vec2(-ScreenPos.y, ScreenPos.x);
    if( isFlipped == 1 )
        nTexCoord = vec2(ScreenPos.y, -ScreenPos.x);

//    if( TexCoord0.y < 0.5 )
//    {
//        gl_FragColor = texture2D(Texture0, nTexCoord);
//    }
//    else
//    {
        lowp float depth = texture2D(Texture1,nTexCoord).r;
        //depth = (depth > 0.5 )?depth * 4.0f:depth;
        if( depth >= 0.995 /*&& depth < 1.0*/)
            gl_FragColor = gaussianBlur(Texture0, nTexCoord, (1.0 - depth) * 1000.0f, vec2(960,540), (nTexCoord - 0.5)*2.0 );
        else
            gl_FragColor = texture2D(Texture0, nTexCoord);
//    }
}
