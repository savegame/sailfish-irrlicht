uniform sampler2D Texture0;
uniform sampler2D Texture1;
varying highp vec4 TexCoord0;
varying highp vec2 ScreenPos;

void main(void)
{
    highp vec2 nTexCoord = vec2(ScreenPos.y, ScreenPos.x);
    gl_FragColor = texture2D(Texture0, nTexCoord);
    //gl_FragColor =  texture2D(Texture0, ScreenPos.st );
    //gl_FragColor = vec3(TexCoord0.yx,1);
}
