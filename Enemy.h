object enemy;
object deadEnemy;
instance* enemies;

SDL_Rect* blocks;


char* deadTime;
char* dead;

float* eSpeed;

void CreateEnemy()
{
    enemies = malloc(sizeof(instance)* MAX_ENEMY);
    blocks = malloc(sizeof(SDL_Rect)*MAX_BLOCKS);
    deadTime = malloc(sizeof(char)* MAX_ENEMY);
    dead = malloc(sizeof(char)* MAX_ENEMY);
    eSpeed = malloc(sizeof(float)* MAX_ENEMY);

    unsigned i,j=0;
    for (i=0;i<MAX_TILES;i++)
        if (lvlMap[i]=='e')
        {
            enemies[j].obj = &enemy;
            enemies[j].pos.x = (i%TILES_PER_ROW)*WALL_SIZE;
            enemies[j].pos.y = (i/TILES_PER_ROW)*WALL_SIZE;
            enemies[j].visible = 1;
            dead[j]=0;
            eSpeed[j]=3.5;
            j++;
        }
    j=0;
    for (i=0;i<MAX_TILES;i++)
        if (lvlMap[i]=='b')
        {
            blocks[j].x = (i%TILES_PER_ROW)*WALL_SIZE;
            blocks[j].y = (i/TILES_PER_ROW)*WALL_SIZE;
            blocks[j].w = blocks[j].h = WALL_SIZE;
            j++;
        }

}

void FreeEnemy()
{
    if (enemies) free(enemies);
    if (blocks) free(blocks);
    if (deadTime) free(deadTime);
    if (dead) free(dead);
    if (eSpeed) free(eSpeed);
}

void DrawEnemy()
{
    unsigned i;
    int col=-1;
    for (i=0;i<MAX_ENEMY;i++)
    if (enemies[i].visible)
    {
        SDL_BlitSurface(enemies[i].obj->surface, NULL, screen, &enemies[i].pos);

        if (dead[i])
        {
            deadTime[i]++;
            if (deadTime[i]>=10) enemies[i].visible = 0;
        }
        else
        {
            // enemies[i].h/w are automatically filled in by SDL_BlitSurface
            //enemies[i].pos.h = enemies[i].obj->surface->clip_rect.h;
            //enemies[i].pos.w = enemies[i].obj->surface->clip_rect.w;
            hero.pos.h = obj_char.surface->clip_rect.h;
            hero.pos.w = obj_char.surface->clip_rect.w;
            if (TestColBall(&enemies[i].pos, &hero.pos))
                col = i;
            else
            {
                unsigned j;
                for (j=0;j<MAX_WALLS;j++)
                    if (TestColBox(&enemies[i].pos, &walls[j].pos))
                        if (walls[j].pos.y < enemies[i].pos.y+enemies[i].pos.h && walls[j].pos.y + walls[j].pos.h > enemies[i].pos.y)
                            eSpeed[i]*=-1;
                for (j=0;j<MAX_BLOCKS;j++)
                    if (TestColBox(&enemies[i].pos, &blocks[j]))
                        eSpeed[i]*=-1;
                enemies[i].pos.x += eSpeed[i];
            }
        }
    }
    if (col>=0)
    {
        SDL_Rect * ERect = &enemies[col].pos;
        SDL_Rect * HRect = &hero.pos;

        if (ERect->y > HRect->y + HRect->h/2.0 && VSPEED > 0)
        {
            enemies[col].obj = &deadEnemy;
            deadTime[col] = 0;
            Score++;
            DisplayScore();
            dead[col]=1;
        }
        else
        {
            //SDL_Delay(1000);
            hero.pos.x = HERO_INIT_X;
            hero.pos.y = HERO_INIT_Y;
        }
    }

}

