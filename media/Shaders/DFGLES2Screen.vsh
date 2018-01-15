attribute vec3 inVertexPosition;
//attribute vec3 inVertexNormal;
//attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;
//attribute vec3 gl_MultiTexCoord0;
uniform highp mat4 mWorldViewProj;
varying highp vec4 TexCoord0;
varying highp vec2 ScreenPos;

void main(void)
{
    gl_Position = mWorldViewProj * vec4(inVertexPosition,1.0);
    TexCoord0 = vec4(inTexCoord0,inTexCoord0);
    //TexCoord0 = vec4(gl_MultiTexCoord0.xyz,1.0);
    ScreenPos = (gl_Position.xy / gl_Position.w + 1.0) * 0.5;
}

