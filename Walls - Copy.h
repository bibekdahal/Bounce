
#define WALL_SIZE 32
#define MAX_WALLS 475
#define WALLS_PER_ROW 25
char wlls[]=
	"1111111111111111111111111"
	"1000000000000000000000001"
	"1000111111111111111111111"
	"1000000000000000000000001"
	"1111000000000000000000001"
	"1000000000000000000000001"
	"1000111110111110111110001"
	"1000000000000000000000001"
	"1000000000000000000000001"
	"1000000000000000000000111"
	"1000000000000000000100001"
	"1000000000011100001100001"
	"1000000000000100000111001"
	"1000001111111100011100001"
	"1000000000000100000100011"
	"1011111111100100111100001"
	"1000000000000100000111001"
	"1000000000000100000100001"
	"1111111111111111111111111";

int nWalls;
object obj_wall;
instance* walls;
void CreateWalls()
{
    obj_wall.IsSheet = 0;
    obj_wall.surface = SDL_CreateRGBSurface(SDL_SWSURFACE, WALL_SIZE, WALL_SIZE, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000); // Little endian ARGB
    SDL_FillRect(obj_wall.surface, &obj_wall.surface->clip_rect, SDL_MapRGB(obj_wall.surface->format,0xDD,0xDD,0xDD));

    unsigned i;
    for (i=0;i<MAX_WALLS;i++)
        if (wlls[i]=='1') nWalls++;
    walls = (instance*) malloc(nWalls*sizeof(instance));

    unsigned j=0;
    for (i=0;i<MAX_WALLS;i++)
        if (wlls[i]=='1')
        {
            walls[j].obj = &obj_wall;
            walls[j].pos.x = (i%WALLS_PER_ROW)*WALL_SIZE;
            walls[j].pos.y = (i/WALLS_PER_ROW)*WALL_SIZE;
            j++;
        }
}
void DestroyWalls()
{
    SDL_FreeSurface(obj_wall.surface);
    free(walls);
}

Uint8 WallsInView[MAX_WALLS];
void DrawWalls()
{
    unsigned i;
    for (i=0;i<nWalls;i++)
    {
        WallsInView[i]=0;

        if (walls[i].pos.x >= view.x+view.w) continue;
        if (walls[i].pos.x + WALL_SIZE <= view.x) continue;
        if (walls[i].pos.y >= view.y + view.h) continue;
        if (walls[i].pos.y + WALL_SIZE <= view.y) continue;

        WallsInView[i] = 1;
        SDL_BlitSurface(obj_wall.surface, NULL, screen, &walls[i].pos);
        //nVWalls++;
    }
}



int CollisionWall(Uint16 WallId, Sint16 PosX, Sint16 PosY, Sint16 W, Sint16 H)
{
    if (!WallsInView[WallId]) return 0;
    SDL_Rect * wpos = &walls[WallId].pos;
    if (wpos->x >= PosX+W) return 0;
    if (wpos->x + WALL_SIZE <= PosX) return 0;
    if (wpos->y >= PosY+H) return 0;
    if (wpos->y + WALL_SIZE <= PosY) return 0;
    return 1;
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
            {hpos->y = wpos->y - obj_char.surface->clip_rect.h; VSPEED = 0;}
        else if (hpos->x > wpos->x)
            hpos->x = wpos->x + WALL_SIZE;
        else if (hpos->x < wpos->x)
            hpos->x = wpos->x - obj_char.surface->clip_rect.w;
    }
}
