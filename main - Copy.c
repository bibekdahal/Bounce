#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
int VIEW_WIDTH = 640;
int VIEW_HEIGHT = 480;
const int SCREEN_BPP = 32; // Bits per pixel



SDL_Surface *screen = NULL;
SDL_Surface *window = NULL;
SDL_Rect view;


typedef struct
{
    SDL_Surface* surface;
    SDL_Rect rect;
    Sint16 XSpan;   // Span between sheet images
    Sint16 YSpan;
} sprite;

typedef struct
{
    Sint8 IsSheet;    // surface is used as full image if IsSheet==0, else spr is used
    SDL_Surface* surface;
    sprite* spr;
    Sint16 imdId;      // Index of image in the sprite sheet
} object;

typedef struct
{
    object* obj;
    SDL_Rect pos;
} instance;

//sprite spr_charL;
//sprite spr_charR;
object obj_char;
instance hero;


float HSPEED = 0;
float VSPEED = 0;

#include "Walls.h"

void CreateChar()
{
    obj_char.surface = IMG_Load("ball.png");
    hero.obj = &obj_char;
    hero.pos.x = WALL_SIZE;
    hero.pos.y = SCREEN_HEIGHT - WALL_SIZE*2;
}
void DrawChar()
{
    SDL_BlitSurface(obj_char.surface, NULL, screen, &hero.pos);
}
void DestroyChar()
{
    SDL_FreeSurface(obj_char.surface);
}

void UpdateView()
{
    if (hero.pos.x < view.x || hero.pos.x - view.x < 128) view.x = hero.pos.x - 128;
    if (hero.pos.x > view.x+view.w || view.x+view.w - hero.pos.x < 128) view.x = 128 - (view.w - hero.pos.x);

    if (hero.pos.y < view.y || hero.pos.y - view.y < 128) view.y = hero.pos.y - 128;
    if (hero.pos.y > view.y+view.h || view.y+view.h - hero.pos.y < 128) view.y = 128 - (view.h - hero.pos.y);

    if (view.x < 0) view.x = 0;
    if (view.x > SCREEN_WIDTH-VIEW_WIDTH) view.x = SCREEN_WIDTH-VIEW_WIDTH;
    if (view.y < 0) view.y = 0;
    if (view.y > SCREEN_HEIGHT-VIEW_HEIGHT) view.y = SCREEN_HEIGHT-VIEW_HEIGHT;
}
void Update()
{
    UpdateView();
    Uint8 *keys = SDL_GetKeyState(NULL);

    char FreeToWalk = 1, OnGround = 0;
    Sint16 PosX = hero.pos.x, PosY = hero.pos.y;
    Uint16 w = obj_char.surface->clip_rect.w, h = obj_char.surface->clip_rect.w, i;
    for (i=0; i<MAX_WALLS; i++)
    {
        if (CollisionWall(i, PosX+HSPEED, PosY, w, h))
            FreeToWalk = 0;
        else if (CollisionWall(i, PosX, PosY+1, w, h) && walls[i].pos.y > PosY)
            OnGround = 1;

    }
    #define LIMIT_HSPEED 8
    #define HPEED_ACCL 0.8
    #define FRICTION 0.2
    if (keys[SDLK_LEFT]) HSPEED -= HPEED_ACCL;
    if (keys[SDLK_RIGHT]) HSPEED +=HPEED_ACCL;

    if (HSPEED > 0 && HSPEED > LIMIT_HSPEED) HSPEED = LIMIT_HSPEED;
    if (HSPEED < 0 && HSPEED < -LIMIT_HSPEED) HSPEED = -LIMIT_HSPEED;

    if ((HSPEED<0 && HSPEED > -FRICTION) || (HSPEED>0 && HSPEED < FRICTION)) HSPEED = 0;
    else if (HSPEED < 0) HSPEED += FRICTION;
    else if (HSPEED > 0) HSPEED -= FRICTION;
    if (FreeToWalk) hero.pos.x += HSPEED;

    #define LIMIT_VSPEED 9
    #define JUMP_SPEED 9
    #define GRAVITY 0.4
    if (keys[SDLK_SPACE]) if (OnGround) VSPEED = -JUMP_SPEED;
    if (VSPEED > LIMIT_VSPEED) VSPEED = LIMIT_VSPEED;
    hero.pos.y += VSPEED;
    if (!OnGround) VSPEED += GRAVITY;

    MoveToContact();
}
int main(int argc, char *argv[])
{
    SDL_putenv("SDL_VIDEO_CENTERED=center");
    SDL_Init(SDL_INIT_EVERYTHING);


    SCREEN_WIDTH = WALL_SIZE*WALLS_PER_ROW;
    SCREEN_HEIGHT = WALL_SIZE * (MAX_WALLS/WALLS_PER_ROW);
    window = SDL_SetVideoMode(VIEW_WIDTH, VIEW_HEIGHT, 32, SDL_SWSURFACE);

    screen = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000); // Little endian ARGB

    view.x = 0; view.y = 0;
    view.w = VIEW_WIDTH; view.h = VIEW_HEIGHT;

    SDL_WM_SetCaption("Platform Sample", NULL);

    CreateWalls(); CreateChar();

    int quit=0;
    SDL_Event event;
    while(!quit)
    {
        while(SDL_PollEvent(&event))
        {
            if (event.type==SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    SDL_Event e; e.type = SDL_QUIT;
                    SDL_PushEvent(&e);
                }
            }
            else if (event.type==SDL_QUIT)
                quit = 1;
        }
        SDL_FillRect(screen,&screen->clip_rect, SDL_MapRGB(screen->format,0x99,0xAA,0xBB));
        DrawWalls(); DrawChar();
        SDL_BlitSurface(screen, &view, window, NULL);
        SDL_Flip(window);

        Update();

    }
    DestroyWalls(); DestroyChar();
    SDL_FreeSurface(screen);
    SDL_Quit();

    return 0;
}

