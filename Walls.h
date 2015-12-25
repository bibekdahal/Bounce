
object obj_wall;
instance* walls=0;

Uint8* WallsInView=0;
void CreateWalls()
{
    walls = malloc(sizeof(instance)* MAX_WALLS);
    WallsInView = malloc(sizeof(Uint8)*MAX_WALLS);

    unsigned i, j=0;
    for (i=0;i<MAX_TILES;i++)
        if (lvlMap[i]=='1')
        {
            walls[j].obj = &obj_wall;
            walls[j].pos.x = (i%TILES_PER_ROW)*WALL_SIZE;
            walls[j].pos.y = (i/TILES_PER_ROW)*WALL_SIZE;
            walls[j].pos.w = walls[j].pos.h = WALL_SIZE;

            WallsInView[j] = 1;
            j++;
        }
}
void FreeWalls()
{
    if (walls) free(walls);
    if (WallsInView) free(WallsInView);
}


void DrawWalls()
{
    unsigned i;
    for (i=0;i<MAX_WALLS;i++)
    {
        WallsInView[i]=0;
        if (!TestColBox(&walls[i].pos, &view)) continue;

        WallsInView[i] = 1;
        SDL_BlitSurface(obj_wall.surface, NULL, screen, &walls[i].pos);
    }
}



int CollisionWall(Uint16 WallId, Sint16 PosX, Sint16 PosY, Sint16 W, Sint16 H)
{
    if (!WallsInView[WallId]) return 0;
    SDL_Rect cr;
    cr.x = PosX; cr.y = PosY; cr.w = W; cr.h = H;
    if (TestColBox(&walls[WallId].pos, &cr)) return 1;
    return 0;
}

void MoveToContact()
{
   Sint16 CollidedWall = -1;
   unsigned i;
   for (i=0;i<MAX_WALLS;i++)
        if (WallsInView[i])
        if (CollisionWall(i,hero.pos.x, hero.pos.y, obj_char.surface->clip_rect.w, obj_char.surface->clip_rect.h))
        {
            CollidedWall = i;
            break;
        }
   if (CollidedWall>=0)
   {
        SDL_Rect* wpos = &walls[CollidedWall].pos;
        SDL_Rect* hpos = &hero.pos;

        if (hpos->y > wpos->y)
            {hpos->y = wpos->y + WALL_SIZE; VSPEED = 0;}
        else if (hpos->y < wpos->y)
            {hpos->y = wpos->y - obj_char.surface->clip_rect.h; if (fabs(VSPEED)<3.2) VSPEED = 0; else VSPEED *=-0.4;}
        else if (hpos->x > wpos->x)
            hpos->x = wpos->x + WALL_SIZE;
        else if (hpos->x < wpos->x)
            hpos->x = wpos->x - obj_char.surface->clip_rect.w;
    }
}
