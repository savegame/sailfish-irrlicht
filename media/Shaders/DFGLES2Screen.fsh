uniform sampler2D Texture0;
varying highp vec4 TexCoord0;

void main(void)
{
    gl_FragColor = vec4(1.f,1.f,1.f,1.f); //texture2D(Texture0, qt_TexCoord0.st);
}
