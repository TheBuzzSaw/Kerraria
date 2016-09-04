#include "TestHandler.hpp"
using namespace std;

TestHandler::TestHandler()
{
}

TestHandler::~TestHandler()
{
}

void TestHandler::OnPrepareRender()
{
    auto third = Tau<float>() / 3.0f;

    for (int i = 0; i < 3; ++i)
    {
        Vector4F v = {{0.0f, 1.0f, 0.0f, 1.0f}};
        auto r = float(i) * third + _rotation;

        auto matrix = _projectionMatrix * RotateZ(r);
        v = matrix * v;
        v = Divided(v);
        _vertices[i * 2 + 0] = v.values[0];
        _vertices[i * 2 + 1] = v.values[1];
    }
}

void TestHandler::OnRender()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 1.0f, 1.0f);
    for (int i = 0; i < 3; ++i) glVertex2fv(_vertices + i * 2);
    glEnd();
}

void TestHandler::OnUpdate()
{
    _rotation += (1.0f / 128.0f);
}

void TestHandler::OnResize(Sint32 width, Sint32 height)
{
    WindowEventHandler::OnResize(width, height);
    auto ratio = float(width) / float(height);
    _projectionMatrix = Orthographic(2.0f, ratio);
}