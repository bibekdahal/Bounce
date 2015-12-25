
Uint32 timePassed;
Uint32 deltaTime;

#define FPS_CONST 20

void InitTimer()
{
    timePassed = SDL_GetTicks();
}

Uint32 Delta()
{
    Uint32 nt = SDL_GetTicks();
    deltaTime = nt - timePassed;
    timePassed = nt;
    return deltaTime;
}
