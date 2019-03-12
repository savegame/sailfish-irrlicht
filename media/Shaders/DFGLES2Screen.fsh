#ifdef GLES2
precision lowp    float;
precision lowp    int;
precision lowp    vec2;
#endif

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform int  inScreenOrientation;
uniform vec2 inResolution;
uniform vec2 inDepthNear;
uniform vec2 inDepthFar;
uniform bool inIsUseDepth;
varying vec2 TexCoord0;
varying vec2 ScreenPos;

//const lowp int OrientationNormal    = 0;
//const lowp int OrientationRotate90  = 1;
//const lowp int OrientationRotate270 = 2;

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
    //OrientationRotate270 - default for SailfishOS
    vec2 nTexCoord = vec2(-ScreenPos.y, ScreenPos.x);
    if( inScreenOrientation == /*OrientationRotate90*/1 )
        nTexCoord = vec2(ScreenPos.y, -ScreenPos.x);
    // no transformations? draw as is
    else if (inScreenOrientation == /*OrientationNormal*/0)
        nTexCoord = ScreenPos.xy;
    else if (inScreenOrientation == /*OrientationRotate180*/2)
        nTexCoord = vec2(-ScreenPos.x,-ScreenPos.y);
    else if (inScreenOrientation == /*OrientationRotate270*/3)
        nTexCoord = vec2(-ScreenPos.y,-ScreenPos.x);

    float depth = texture2D(Texture1,nTexCoord).r;
    //lowp float blur  = 0.0;
    float strength = 5.0;
    bool isUseDepth = false;
    if( depth >= inDepthFar.x && isUseDepth == true)
    {// far depth zone
        float r = (depth - inDepthFar.x)/(1.0 - inDepthFar.x);
//        float full = 1.0 - inDepthFar.x;
        if( r < inDepthFar.y )
        {
            strength = strength * r/inDepthFar.y;
        }
        gl_FragColor = gaussianBlur(Texture0, nTexCoord, strength, inResolution.xy, /*(nTexCoord - 0.5)*2.0*/vec2(0.0,0.5) );
    }
    else
        gl_FragColor = texture2D(Texture0, nTexCoord);
        //gl_FragColor = vec4(1.0,1.0,1.0,1.0);
    //	gl_Fr/agColor = texture2D(Texture0,ScreenPos.xy);
}
