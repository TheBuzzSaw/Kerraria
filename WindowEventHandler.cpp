#include "WindowEventHandler.hpp"
#include "Debug.hpp"
#include <SDL_opengl.h>
#include <iostream>
#include <cassert>
using namespace std;

WindowEventHandler::WindowEventHandler()
{
    SetUpdatesPerSecond(60);
}

WindowEventHandler::~WindowEventHandler()
{
}

void WindowEventHandler::SetUpdatesPerSecond(int updatesPerSecond)
{
    assert(updatesPerSecond > 0);
    _frameLength = SDL_GetPerformanceFrequency() / updatesPerSecond;
}

void WindowEventHandler::Run(SDL_Window* window)
{
    _window = window;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    OnOpen();

    int w;
    int h;
    SDL_GetWindowSize(window, &w, &h);
    OnResize(w, h);

    auto secondLength = SDL_GetPerformanceFrequency();
    auto lastUpdate = SDL_GetPerformanceCounter();
    auto lastSecond = lastUpdate;
    int peakUpdateCount = 0;
    int sleepCount = 0;
    _running = true;

    while (_running)
    {
        bool doSleep = true;
        _needPrepareRender = false;
        _needRender = false;

        SDL_Event event;
        while (SDL_PollEvent(&event)) OnEvent(event);

        auto now = SDL_GetPerformanceCounter();

        assert(now > lastSecond);
        if ((now - lastSecond) >= secondLength)
        {
            cerr << "sleep count -- " << sleepCount << '\n';
            sleepCount = 0;
            OnSecond();
            lastSecond = now;
            FlushLog();
        }

        int updateCount = 0;

        assert(now > lastUpdate);
        while ((now - lastUpdate) >= _frameLength)
        {
            OnUpdate();
            lastUpdate += _frameLength;
            ++updateCount;
        }

        if (updateCount > 0)
        {
            if (updateCount > peakUpdateCount)
            {
                Log() << "new peak update count: " << peakUpdateCount << " -> "
                    << updateCount << '\n';
                peakUpdateCount = updateCount;
            }

            _needPrepareRender = true;
            _needRender = true;
        }

        if (_needPrepareRender)
        {
            doSleep = false;
            OnPrepareRender();
        }

        if (_needRender)
        {
            doSleep = false;
            OnRender();
            SDL_GL_SwapWindow(window);
        }

        if (doSleep)
        {
            ++sleepCount;
            SDL_Delay(1);
        }
    }

    OnClose();
    _window = nullptr;
}

void WindowEventHandler::OnOpen()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void WindowEventHandler::OnClose()
{
}

void WindowEventHandler::OnPrepareRender()
{
}

void WindowEventHandler::OnRender()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.5f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.0f, 0.0f);
    glEnd();
}

void WindowEventHandler::OnLoop()
{
}

void WindowEventHandler::OnUpdate()
{
}

void WindowEventHandler::OnSecond()
{
}

void WindowEventHandler::OnEvent(SDL_Event event)
{
    //static int n = 0;
    //cout << "OnEvent " << ++n << endl;
    switch (event.type)
    {
        case SDL_WINDOWEVENT:
        {
            switch (event.window.event)
            {
                case SDL_WINDOWEVENT_ENTER: OnMouseFocus(); break;
                case SDL_WINDOWEVENT_LEAVE: OnMouseBlur(); break;
                case SDL_WINDOWEVENT_FOCUS_GAINED: OnInputFocus(); break;
                case SDL_WINDOWEVENT_FOCUS_LOST: OnInputBlur(); break;
                case SDL_WINDOWEVENT_RESTORED: OnRestore(); break;
                case SDL_WINDOWEVENT_MINIMIZED: OnMinimize(); break;
                case SDL_WINDOWEVENT_MAXIMIZED:
                    OnMaximize();
                    break;
                case SDL_WINDOWEVENT_EXPOSED:
                    _needRender = true;
                    OnExpose();
                    break;

                //case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    _needPrepareRender = true;
                    _needRender = true;
                    OnResize(
                        event.window.data1,
                        event.window.data2);
                    break;

                default: break;
            }

            break;
        }

        case SDL_KEYDOWN:
            if (event.key.repeat)
                OnKeyRepeat(event.key.keysym);
            else
                OnKeyDown(event.key.keysym);
            break;

        case SDL_KEYUP: OnKeyUp(event.key.keysym); break;
        case SDL_MOUSEMOTION: OnMouseMove(event.motion); break;
        case SDL_MOUSEWHEEL: OnMouseWheel(event.wheel); break;
        case SDL_MOUSEBUTTONDOWN: OnMouseButtonDown(event.button); break;
        case SDL_MOUSEBUTTONUP: OnMouseButtonUp(event.button); break;

        case SDL_JOYAXISMOTION: OnJoyAxis(event.jaxis); break;
        case SDL_JOYBALLMOTION: OnJoyBall(event.jball); break;
        case SDL_JOYHATMOTION: OnJoyHat(event.jhat); break;
        case SDL_JOYBUTTONDOWN: OnJoyButtonDown(event.jbutton); break;
        case SDL_JOYBUTTONUP: OnJoyButtonUp(event.jbutton); break;
        case SDL_QUIT: OnExit(); break;
        case SDL_SYSWMEVENT: break;

        case SDL_CONTROLLERAXISMOTION: OnControllerAxis(event.caxis); break;
        case SDL_CONTROLLERBUTTONDOWN: OnControllerButtonDown(event.cbutton); break;
        case SDL_CONTROLLERBUTTONUP: OnControllerButtonUp(event.cbutton); break;

        default: OnUser(event.user);break;
    }
}

/// Key Events
void WindowEventHandler::OnKeyDown(SDL_Keysym keysym)
{
    if (keysym.sym == SDLK_ESCAPE) _running = false;
}

void WindowEventHandler::OnKeyRepeat(SDL_Keysym keysym)
{
    (void)keysym;
}

void WindowEventHandler::OnKeyUp(SDL_Keysym keysym)
{
    (void)keysym;
}

/// Mouse Events
void WindowEventHandler::OnMouseFocus()
{
}

void WindowEventHandler::OnMouseBlur()
{
}

void WindowEventHandler::OnMouseMove(SDL_MouseMotionEvent event)
{
    (void)event;
}

void WindowEventHandler::OnMouseWheel(SDL_MouseWheelEvent event)
{
    (void)event;
}

void WindowEventHandler::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    (void)event;
}

void WindowEventHandler::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    (void)event;
}

///Joystick Events
void WindowEventHandler::OnJoyAxis(SDL_JoyAxisEvent event)
{
    (void)event;
}

void WindowEventHandler::OnJoyButtonDown(SDL_JoyButtonEvent event)
{
    (void)event;
}

void WindowEventHandler::OnJoyButtonUp(SDL_JoyButtonEvent event)
{
    (void)event;
}

void WindowEventHandler::OnJoyHat(SDL_JoyHatEvent event)
{
    (void)event;
}

void WindowEventHandler::OnJoyBall(SDL_JoyBallEvent event)
{
    (void)event;
}

///Gamepad Events
void WindowEventHandler::OnControllerAxis(SDL_ControllerAxisEvent event)
{
    (void)event;
}

void WindowEventHandler::OnControllerButtonDown(SDL_ControllerButtonEvent event)
{
    (void)event;
}

void WindowEventHandler::OnControllerButtonUp(SDL_ControllerButtonEvent event)
{
    (void)event;
}

///Window Events
void WindowEventHandler::OnInputFocus()
{
}

void WindowEventHandler::OnInputBlur()
{
}

void WindowEventHandler::OnMinimize()
{
}

void WindowEventHandler::OnMaximize()
{
}

void WindowEventHandler::OnRestore()
{
}

void WindowEventHandler::OnResize(Sint32 width, Sint32 height)
{
    (void)width;
    (void)height;
    glViewport(0, 0, width, height);
}

void WindowEventHandler::OnExpose()
{
}

void WindowEventHandler::OnExit()
{
    _running = false;
}

void WindowEventHandler::OnUser(SDL_UserEvent event)
{
    (void)event;
}
