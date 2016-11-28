uniform sampler2D theTexture;
varying lowp vec3 _color;
varying lowp vec2 _textureCoordinates;

void main()
{
    gl_FragColor = texture2D(theTexture, _textureCoordinates) * vec4(_color, 1.0);
}
