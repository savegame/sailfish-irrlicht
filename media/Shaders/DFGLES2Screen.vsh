#ifdef GLES2
precision highp   mat4;
precision highp   vec2;
precision lowp    float;
precision lowp    int;
#endif
attribute vec3 inVertexPosition;
attribute vec2 inTexCoord0;
//attribute vec3 gl_MultiTexCoord0;
uniform mat4 mWorldViewProj;
varying vec2 TexCoord0;
varying vec2 ScreenPos;

void main(void)
{
    gl_Position = mWorldViewProj * vec4(inVertexPosition,1.0);
    TexCoord0 = inTexCoord0;
    ScreenPos = (gl_Position.xy / gl_Position.w + 1.0) * 0.5;
}

