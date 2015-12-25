#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include<stdio.h>
#include<string.h>
#include<windows.h>

Uint32 FPS;

#include "map.h"
#include "timer.h"

SDL_Surface *screen = NULL;
SDL_Surface *window = NULL;
SDL_Rect view;

/*
typedef struct
{
    SDL_Surface* surface;
    SDL_Rect rect;
    Sint16 XSpan;   // Span between sheet images
    Sint16 YSpan;
} sprite;*/

typedef struct
{
    //Sint8 IsSheet;    // surface is used as full image if IsSheet==0, else spr is used
    SDL_Surface* surface;
    //sprite* spr;
    Sint16 imdId;      // Index of image in the sprite sheet
} object;

typedef struct
{
    object* obj;
    SDL_Rect pos;
    Uint8 visible;
} instance;

//sprite spr_charL;
//sprite spr_charR;
object obj_char;
instance hero;

object obj_exit;
instance lvlExit;


float HSPEED = 0;
float VSPEED = 0;

#include "collision.h"
#include "Walls.h"
Sint16 HERO_INIT_X = 0;
Sint16 HERO_INIT_Y = 0;

int Score=0;

void DisplayScore()
{
    char msg[20] = "BOUNCE - SCORE : ";
    itoa(Score, &msg[strlen(msg)],10);
    SDL_WM_SetCaption(msg, NULL);
}
#include "Enemy.h"

void CreateResources()
{
    obj_exit.surface = IMG_Load("door.png");
    lvlExit.obj = &obj_exit;

    obj_char.surface = IMG_Load("ball.png");
    hero.obj = &obj_char;

    obj_wall.surface = SDL_CreateRGBSurface(SDL_SWSURFACE, WALL_SIZE, WALL_SIZE, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000); // Little endian ARGB
    SDL_FillRect(obj_wall.surface, &obj_wall.surface->clip_rect, SDL_MapRGB(obj_wall.surface->format,0xDD,0xDD,0xDD));

    enemy.surface = IMG_Load("enemy.png");
    deadEnemy.surface = IMG_Load("deadenemy.png");

}
void FreeResources()
{
    FreeWalls(); FreeEnemy();
    SDL_FreeSurface(obj_exit.surface);
    SDL_FreeSurface(obj_char.surface);
    SDL_FreeSurface(obj_wall.surface);
    SDL_FreeSurface(enemy.surface);
    SDL_FreeSurface(deadEnemy.surface);
}

void CreateExit()
{

    unsigned i;
    for (i=0;i<MAX_TILES;i++)
        if (lvlMap[i]=='x')
        {
            lvlExit.pos.x = (i%TILES_PER_ROW)*WALL_SIZE;
            lvlExit.pos.y = (i/TILES_PER_ROW)*WALL_SIZE;
            break;
        }
}

void DrawExit()
{
    SDL_BlitSurface(obj_exit.surface, NULL, screen, &lvlExit.pos);
    hero.pos.h = obj_char.surface->clip_rect.h;
    hero.pos.w = obj_char.surface->clip_rect.w;
    if (TestColBox(&lvlExit.pos, &hero.pos))
    {
        char msg[70] = "YOU JUST WON THE GAME - FINAL SCORE : ";
        itoa(Score, &msg[strlen(msg)],10);
        SDL_WM_SetCaption(msg, NULL);
        SDL_Delay(1000);
        SDL_Event e; e.type = SDL_QUIT;
        SDL_PushEvent(&e);
    }
}

void CreateChar()
{
    unsigned i;
    for (i=0;i<MAX_TILES;i++)
        if (lvlMap[i]=='h')
        {
            HERO_INIT_X = (i%TILES_PER_ROW)*WALL_SIZE;
            HERO_INIT_Y = (i/TILES_PER_ROW)*WALL_SIZE;
            break;
        }


    hero.pos.x = HERO_INIT_X;
    hero.pos.y = HERO_INIT_Y;
}
void DrawChar()
{
    SDL_BlitSurface(obj_char.surface, NULL, screen, &hero.pos);
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

    #define LIMIT_HSPEED 6
    #define HPEED_ACCL 0.6
    #define FRICTION 0.2
    if (keys[SDLK_LEFT]) HSPEED -= HPEED_ACCL;
    if (keys[SDLK_RIGHT]) HSPEED += HPEED_ACCL;

    if (HSPEED > 0 && HSPEED > LIMIT_HSPEED) HSPEED = LIMIT_HSPEED;
    if (HSPEED < 0 && HSPEED < -LIMIT_HSPEED) HSPEED = -LIMIT_HSPEED;

    if ((HSPEED<0 && HSPEED > -FRICTION) || (HSPEED>0 && HSPEED < FRICTION)) HSPEED = 0;
    else if (HSPEED < 0) HSPEED += FRICTION;
    else if (HSPEED > 0) HSPEED -= FRICTION;
    if (FreeToWalk) hero.pos.x += HSPEED;

    #define LIMIT_VSPEED 9
    #define JUMP_SPEED 9
    #define GRAVITY 0.4
    if (keys[SDLK_UP] || keys[SDLK_SPACE]) if (OnGround) VSPEED = -JUMP_SPEED;
    if (VSPEED > LIMIT_VSPEED) VSPEED = LIMIT_VSPEED;

    hero.pos.y += VSPEED;
    if (!OnGround) VSPEED += GRAVITY;

    MoveToContact();
}

void LoadMap(const char* fname)
{
    FILE* fin = fopen(fname, "r");
    if (!fin) return;

    fscanf(fin, "%d%d", &MAX_TILES, &TILES_PER_ROW);
    if (lvlMap) free(lvlMap);
    lvlMap = malloc(sizeof(char)*MAX_TILES+1);
    fscanf(fin, "%s", lvlMap);

    MAX_WALLS = MAX_ENEMY = MAX_BLOCKS = 0;
    unsigned i;
    for (i=0;i<MAX_TILES;i++)
    {
        switch(lvlMap[i])
        {
            case '1': MAX_WALLS++; break;
            case 'e': MAX_ENEMY++; break;
            case 'b': MAX_BLOCKS++; break;
        }
    }

    SCREEN_WIDTH = WALL_SIZE*TILES_PER_ROW;
    SCREEN_HEIGHT = WALL_SIZE * (MAX_TILES/TILES_PER_ROW);
    if (screen) SDL_FreeSurface(screen);
    screen = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000); // Little endian ARGB

    FreeWalls();
    FreeEnemy();

    CreateChar(); CreateExit(); CreateWalls(); CreateEnemy();
    fclose(fin);
}

void UnloadMap()
{
    free(lvlMap);
}

int main(int argc, char *argv[])
{
    SDL_putenv("SDL_VIDEO_CENTERED=center");
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_SetVideoMode(VIEW_WIDTH, VIEW_HEIGHT, 32, SDL_SWSURFACE);

    view.x = 0; view.y = 0;
    view.w = VIEW_WIDTH; view.h = VIEW_HEIGHT;

    DisplayScore();

    InitTimer();

    //CreateWalls(); CreateChar(); CreateEnemy(); CreateExit();
    CreateResources();
    LoadMap("level0.txt");

    int quit=0;
    SDL_Event event;

    Uint32 frames=0;
    Uint32 tp;
    tp = SDL_GetTicks();
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
        Delta();

        DrawWalls(); DrawExit(); DrawEnemy(); DrawChar();
        SDL_BlitSurface(screen, &view, window, NULL);
        SDL_Flip(window);

        Update();
        // Constant FPS
        if(deltaTime < 1000/FPS_CONST)
            SDL_Delay(1000/FPS_CONST - deltaTime);
        frames++;
        if (timePassed-tp>=1000)
        {
            FPS=frames;
            frames=0;
            tp=timePassed;
            //char chr[20];
            //itoa(FPS, chr, 10);
            //SDL_WM_SetCaption(chr, NULL);
        }

    }
    UnloadMap();
    FreeResources();
    //DestroyWalls(); DestroyChar(); DestroyEnemy(); DestroyExit();
    SDL_FreeSurface(screen);
    SDL_Quit();

    return 0;
}

