#include "TestHandler.hpp"
#include "Debug.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
using namespace std;

static constexpr float Delta = 1.0f / 8.0f;
static constexpr float PixelsPerSpace = 64.0f;

TestHandler::TestHandler()
    : _mt(time(nullptr))
{
    _grid = GenerateSimple({256, 128}, _mt);
    
    _tileViewCenter = {
        static_cast<float>(_grid.size.x / 2),
        static_cast<float>(_grid.size.y / 2)};
}

TestHandler::~TestHandler()
{
}

void TestHandler::OnOpen()
{
}

void TestHandler::OnClose()
{
}

void TestHandler::OnPrepareRender()
{
    auto halfSpace = _tileViewSpace / 2.0f;
    Point<float> center = {
        Restricted(_tileViewCenter.x, halfSpace.x, _grid.size.x - halfSpace.x),
        Restricted(_tileViewCenter.y, halfSpace.y, _grid.size.y - halfSpace.y)};

    auto tileViewOffset = (center - halfSpace)
        .Cast<int>()
        .Restricted(
            0,
            _grid.size.x - _tileViewSize.x,
            0,
            _grid.size.y - _tileViewSize.y);
    
    _buffer.Generate(_grid, tileViewOffset, _tileViewSize);

    auto translation = -center + tileViewOffset.Cast<float>();

    _rotateMatrix =
        //RotateZ(_rotation) *
        Translate(
            translation.x,
            translation.y,
            0.0f);
    
    _buffer.matrix = _projectionMatrix * _rotateMatrix;

    if (_logDump)
    {
        _logDump = false;
        Log() << "halfSpace -- " << halfSpace << '\n';
        Log() << "center -- " << center << '\n';
        Log() << "translation -- " << translation << '\n';
    }
}

void TestHandler::OnRender()
{
    _renderer.Render(_buffer);
}

void TestHandler::OnUpdate()
{
    if (_panAnchor.x < 0) _tileViewCenter += _delta * _multiplier;
    _rotation -= (1.0f / 128.0f);
}

void TestHandler::OnKeyDown(SDL_Keysym keysym)
{
    WindowEventHandler::OnKeyDown(keysym);
    switch (keysym.sym)
    {
        case SDLK_l:
            _logStats = !_logStats;
            break;

        case SDLK_BACKSLASH:
            //SDL_Delay(750);
            _logDump = true;
            break;

        case SDLK_F11:
        {
            auto flag = SDL_GetWindowFlags(
                Window()) & SDL_WINDOW_FULLSCREEN_DESKTOP;
            SDL_SetWindowFullscreen(
                Window(),
                flag ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
            break;
        }

        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            _multiplier = 12.0f;
            break;

        case SDLK_LCTRL:
        case SDLK_RCTRL:
            _multiplier = 0.25f;
            break;

        case SDLK_a:
        case SDLK_LEFT:
            _delta.x = -Delta;
            break;
        case SDLK_d:
        case SDLK_RIGHT:
            _delta.x = Delta;
            break;
        case SDLK_w:
        case SDLK_UP:
            _delta.y = Delta;
            break;
        case SDLK_s:
        case SDLK_DOWN:
            _delta.y = -Delta;
            break;

        default: break;
    }
}

void TestHandler::OnKeyUp(SDL_Keysym keysym)
{
    WindowEventHandler::OnKeyUp(keysym);
    switch (keysym.sym)
    {
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            if (_multiplier > 1.0f) _multiplier = 1.0f;
            break;

        case SDLK_LCTRL:
        case SDLK_RCTRL:
            if (_multiplier < 1.0f) _multiplier = 1.0f;
            break;

        case SDLK_a:
        case SDLK_LEFT:
            if (_delta.x < 0.0f) _delta.x = 0.0f;
            break;

        case SDLK_d:
        case SDLK_RIGHT:
            if (_delta.x > 0.0f) _delta.x = 0.0f;
            break;

        case SDLK_w:
        case SDLK_UP:
            if (_delta.y > 0.0f) _delta.y = 0.0f;
            break;

        case SDLK_s:
        case SDLK_DOWN:
            if (_delta.y < 0.0f) _delta.y = 0.0f;
            break;
    }
}

void TestHandler::OnMouseMove(SDL_MouseMotionEvent event)
{
    if (_panAnchor.x >= 0)
    {
        Point<int> mouse{event.x, event.y};
        auto delta = (_panAnchor - mouse).Cast<float>() / PixelsPerSpace;
        delta.y = -delta.y;
        _tileViewCenter = _tileViewCenterAnchor + delta;
    }
}

void TestHandler::OnMouseWheel(SDL_MouseWheelEvent event)
{
    (void)event;
}

void TestHandler::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT)
    {
        _panAnchor = {event.x, event.y};
        _tileViewCenterAnchor = _tileViewCenter;
    }
    else if (event.button == SDL_BUTTON_RIGHT)
    {
        Point<int> position = {event.x, _displaySize.y - 1 - event.y};
        auto halfSpace = _tileViewSpace / 2.0f;
        auto spaceOffset = position.Cast<float>() / PixelsPerSpace - halfSpace;
        auto worldCoordinates = (_tileViewCenter + spaceOffset).Cast<int>();
        
        if (worldCoordinates.x >= 0 &&
            worldCoordinates.x < _grid.size.x &&
            worldCoordinates.y >= 0 &&
            worldCoordinates.y < _grid.size.y)
        {
            _grid.ToSpan2D()(worldCoordinates.x, worldCoordinates.y) = NoTile;
        }
        else
        {
            Log() << "out of bounds -- " << worldCoordinates << '\n';
        }
    }
}

void TestHandler::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT)
    {
        _panAnchor = {-1, -1};
    }
}

void TestHandler::OnResize(Sint32 width, Sint32 height)
{
    _displaySize = {width, height};

    WindowEventHandler::OnResize(width, height);
    _tileViewSpace = {
        float(width) / PixelsPerSpace,
        float(height) / PixelsPerSpace};

    _tileViewSize = (_tileViewSpace.Cast<int>() + Point<int>{2, 2})
        .Restricted(1, _grid.size.x, 1, _grid.size.y);

    auto halfSpace = _tileViewSpace / 2.0f;
    _projectionMatrix = Orthographic(
        -halfSpace.x,
        halfSpace.x,
        -halfSpace.y,
        halfSpace.y,
        1.0f,
        -1.0f);
}

