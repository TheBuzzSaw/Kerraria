uniform highp mat4 theMatrix;
attribute highp vec3 position;
attribute lowp vec3 color;
attribute lowp vec2 textureCoordinates;
varying lowp vec3 _color;
varying lowp vec2 _textureCoordinates;

void main()
{
    _color = color;
    _textureCoordinates = textureCoordinates;
    gl_Position = theMatrix * vec4(position, 1.0);
}

