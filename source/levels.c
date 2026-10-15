#include "game.h"
/*levels yippi*/
const Mission pw_missions[PW_MISSIONS]={{
    "MACHINE BENCH","WORK IN PROGRESS","Connect the rooms and reach the dock.","A/D move; drag Windows title bars to line up the doors."
}};
static void wall(Room*r, float x, float y, float w, float h){
    if(r->nwalls<18)r->walls[r->walls++]=(Box){
        x,y,w,h
    };
}

static void crate(Game*g, int slot, int room, float x, float y, int kind){
    Crate*c=&g->crates[slot];
    *c=(Crate){
        0
    };
    c->active=1;
    c->room=c->home_room=room;
    c->x=c->home_x=x;
    c->y=c->home_y=y;
    c->kind=kind;
    c->w=kind?24:32;
    c->h=kind?28:32;
}