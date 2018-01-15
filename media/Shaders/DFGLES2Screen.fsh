uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform int isFlipped;
varying highp vec2 TexCoord0;
varying highp vec2 ScreenPos;

//lowp float rand(lowp vec2 co){
//    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
//}

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

//    if( TexCoord0.y < 0.5 )
    {
        gl_FragColor = texture2D(Texture0, nTexCoord);
    }
//    else
//        gl_FragColor = texture2D(Texture1, nTexCoord);
        //gl_FragColor = vec4(TexCoord0.xy, 1, 1);
}
