attribute vec3 inVertexPosition;
attribute highp vec4 qt_MultiTexCoord0;
uniform highp mat4 mWorldViewProj;
varying highp vec4 TexCoord0;

void main(void)
{
    gl_Position = mWorldViewProj * inVertexPosition;
    TexCoord0 = qt_MultiTexCoord0;
}

