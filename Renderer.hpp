#ifndef Renderer_hpp
#define Renderer_hpp

#include "OpenGL.hpp"
#include "RenderGridBuffer.hpp"

class Renderer
{
    GLuint _texture;
    GLuint _program;
	GLint _matrixUniform;
    GLint _textureUniform;
    GLint _positionAttribute;
    GLint _colorAttribute;
    GLint _textureCoordinateAttribute;

public:
    Renderer();
    Renderer(Renderer&&) = delete;
    Renderer(const Renderer&) = delete;
    ~Renderer();

    Renderer& operator=(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void Render(const RenderGridBuffer& buffer);
};

#endif
