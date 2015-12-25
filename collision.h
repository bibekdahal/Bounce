
int TestColBox(SDL_Rect* box1, SDL_Rect* box2)
{
    if (box1->x >= box2->x+box2->w) return 0;
    if (box2->x >= box1->x+box1->w) return 0;
    if (box1->y >= box2->y+box2->h) return 0;
    if (box2->y >= box1->y+box1->h) return 0;
    return 1;
}

int TestColBall(SDL_Rect* box1, SDL_Rect* box2)
{
    /* Since used only for detection between hero and enemy, radii are set constant to 32 */
    //int r1 = box1->w/2;
    //int r2 = box2->w/2;
    int cx1 = box1->x + 32; //r1*2;
    int cy1 = box1->y + 32; //r1*2;
    int cx2 = box2->x + 32; //r2*2;
    int cy2 = box2->y + 32; //r2*2;

    if (((cx2-cx1)*(cx2-cx1) + (cy2-cy1)*(cy2-cy1)) <= 32*32)//(r1+r2)*(r1+r2))
        return 1;
    return 0;
}
