attribute vec3 inVertexPosition;
attribute vec2 inTexCoord0;
uniform mat4 mWorldViewProj;
varying vec2 TexCoord0;
varying vec2 ScreenPos;

void main(void)
{
    gl_Position = mWorldViewProj * vec4(inVertexPosition,1.0);
    //gl_TexCoord[0]=gl_MultiTexCoord0;
    TexCoord0 = inTexCoord0;
    //TexCoord0 = vec4(gl_MultiTexCoord0.xyz,1.0);
    ScreenPos = (gl_Position.xy / gl_Position.w + 1.0) * 0.5;
}

