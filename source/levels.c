#include "game.h"
/*levels yippi*/
const Mission pw_missions[PW_MISSIONS]={{
    "MACHINE BENCH","WORK IN PROGRESS","Connect the rooms and reach the dock.","A/D move; drag Windows title bars to line up the doors."
}};
static void wall(Room*r, float x, float y, float w, float h){
    if(r->nwalls<18)r->walls[r->nwalls++]=(Box){
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

static void drone(Game*g, int slot, int room, float x, float y, int type){
    Enemy*e=&g->enemies[slot];
    *e=(Enemy){
        0
    };
    e-> active=1;
    e->room=room;
    e->x=x;
    e->y=e->base_y=y;
    e->type=type;
    e->hp=type==2?20: (type==1?4:3);
    e->vx=(slot%2?-1:1)*52;
    e->timer=.7f+(slot%3)*.28f;
}

static void pad(Room*r,float x){
    static const int start[9]={
        12,6,10,3,12,5,6,9,10
    };
    r->has_pad=1;
    r->pad=(Box){
        x,207,32,48
    };
    memcpy(r->board,start,sizeof(start));
}

static void pit(Room*r,float x, float w){
    r->nwalls=0;
    wall(r,0,266,x,22);
    wall(r,x+w,266,PW_W-x-w,22);
    r->lava[r->nlava++]=(Box){
        x,270,w,18
    };
}

void pw_load(Game*g,int mission) {
    int i;(void)mission;
    memset(g,0,sizeof(*g));g->nrooms=2;g->editing=-1;g->effects=1;
    g->seed=1234567u;g->p.hp=5;g->p.carry=-1;g->p.hook=-1;g->p.face=1;
    g->p.x=54;g->p.y=238;g->goal_room=1;g->goal_x=385;g->goal_y=241;
    for(i=0;i<g->nrooms;i++){
        Room*r=&g->rooms[i];r->ox=100+i*480;r->oy=216+(i==1?40:0);
        r->tint=0x199f96;snprintf(r->name,sizeof(r->name),"%s",i==0?"SERVICE BAY":"TRANSFER DECK");
        wall(r,0,266,PW_W,22);
    }
    wall(&g->rooms[0],174,237,36,29);
    pw_message(g,"Move the windows. Line up the door lamps. R restarts this job.");
}