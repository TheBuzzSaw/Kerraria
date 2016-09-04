#ifndef WindowEventHandler_hpp
#define WindowEventHandler_hpp

#include <SDL.h>

class WindowEventHandler
{
    Uint64 _frameLength;
    bool _running;

protected:
    void SetUpdatesPerSecond(int updatesPerSecond);

public:
    WindowEventHandler();
    WindowEventHandler(const WindowEventHandler&) = delete;
    WindowEventHandler(WindowEventHandler&&) = delete;

    virtual ~WindowEventHandler();

    WindowEventHandler& operator=(const WindowEventHandler&) = delete;
    WindowEventHandler& operator=(WindowEventHandler&&) = delete;

    void Run(SDL_Window* window);
    void OnEvent(SDL_Event event);

    /// high level operation
    virtual void OnOpen();
    virtual void OnClose();
    virtual void OnPrepareRender();
    virtual void OnRender();
    virtual void OnLoop();
    virtual void OnUpdate();
    virtual void OnSecond(int framesPerSecond);

    /// input events

    /// Key events
    virtual void OnKeyDown(SDL_Keysym keysym);
    virtual void OnKeyRepeat(SDL_Keysym keysym);
    virtual void OnKeyUp(SDL_Keysym keysym);

    /// Mouse events
    virtual void OnMouseFocus();
    virtual void OnMouseBlur();
    virtual void OnMouseMove(SDL_MouseMotionEvent event);
    virtual void OnMouseWheel(SDL_MouseWheelEvent event);
    virtual void OnMouseButtonDown(SDL_MouseButtonEvent event);
    virtual void OnMouseButtonUp(SDL_MouseButtonEvent event);

    /// Joystick events
    virtual void OnJoyAxis(SDL_JoyAxisEvent event);
    virtual void OnJoyButtonDown(SDL_JoyButtonEvent event);
    virtual void OnJoyButtonUp(SDL_JoyButtonEvent event);
    virtual void OnJoyHat(SDL_JoyHatEvent event);
    virtual void OnJoyBall(SDL_JoyBallEvent event);

    /// Gamepad events
    virtual void OnControllerAxis(SDL_ControllerAxisEvent event);
    virtual void OnControllerButtonDown(
        SDL_ControllerButtonEvent event);
    virtual void OnControllerButtonUp(
        SDL_ControllerButtonEvent event);

    /// Window events
    virtual void OnInputFocus();
    virtual void OnInputBlur();
    virtual void OnMinimize();
    virtual void OnMaximize();
    virtual void OnRestore();
    virtual void OnResize(Sint32 width, Sint32 height);
    virtual void OnExpose();
    virtual void OnExit();
    virtual void OnUser(SDL_UserEvent event);
};

#endif

