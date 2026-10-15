#include "game.h"
float pw_abs(float x){
    return x<0?-x:x;
}

float pw_clamp(float v, float lo, float hi){
    return v<lo?lo:(v>hi?hi:v);
}

float pw_length(float x,float y){
    return (float)sqrt(x*x+y*y);
}

static float approach(float x, float y, float by){
    return x<y?(x+by>y?y:x+by):(x-by<y?y:x-by);
}

static float random01(Game*g){
    g->seed=g->seed*1664525u+1013904223u;
    return (g->seed>>8)*(1.0f/16777216.0f);
}

static Box body(float x, float y, float w, float h) {
    return (Box){
        x-w*.5f,y-h*.5f,w,h
    };
}

int pw_overlap(Box a, Box b){
    return a.x<b.x+b.w-.01f&&a.x+a.w>b.x+.01f&&a.y<b.y+b.h-.01f&&a.y+a.h>b.y+.01f;
}

static int point(Box b, float x, float y){
    return x>=b.x&&x<=b.x+b.w&&y>=b.y&&y<=b.y+b.h;
}

void pw_message(Game*g,const char*s){
    snprintf(g->toast,sizeof(g->toast),"%s",s);
    g->toast_time=4.0f;
}

void pw_effect(Game*g,int room, float x, float y, unsigned color, int count) {
    (void)g; (void)room; (void)x; (void)y; (void)color; (void)count;
}

int pw_powered(const Game*g){
    int i;
    for(i=0;i<g->nrooms;i++){
        if(g->require_pad&&g->rooms[i].has_pad&&!g->rooms[i].repaired)return 0;
        if(g->require_battery&&g->rooms[i].has_socket&&!g->rooms[i].installed)return 0;
    }
    return 1;
}

int pw_alive(const Game*g){
    int n=0,i;
    for(i=0;i<PW_ENEMIES;i++)if(g->enemies[i].active)n++;
    return n;
}

static int pads_ready(const Game*g){
    int i;
    for(i=0;i<g->nrooms;i++)if(g->rooms[i].has_pad&&!g->rooms[i].repaired) return 0;
    return 1;
}

/*build checking opposing room edges in desktop coords so that doors never open through overlapps */
int pw_link(const Game*g, int r, int side) {
    int j;
    const Room*a=&g->rooms[r];
    for(j=0;j<g->nrooms;j++)if(j!=r){
        const Room*b=&g->rooms[j];
        float dx=side>0?b->ox-(a->ox+PW_W):(a->ox-(b->ox+PW_W));
        if(dx>=0&&dx<=PW_GAP+18&&pw_abs(a->oy-b->oy)<=18)return j;
    }
    return -1;
}

void pw_move_room(Game*g,int r, float x, float y, int snap){
    int j;
    Room*a;
    if(r<0||r>=g->nrooms)return;
    a=&g->rooms[r];
    if(pw_length(x-a->ox,y-a->oy)>1)g->dragged+=(int)pw_length(x-a->ox,y-a->oy);
    a->ox=x;
    a->oy=y;
    if(snap)for(j=0;j<g->nrooms;j++)if(j!=r){
        Room*b=&g->rooms[j];
        if(pw_abs(a->oy-b->oy)<30){
            if(pw_abs(a->ox-(b->ox+PW_W+PW_GAP))<34){
                a->ox=b->ox+PW_W+PW_GAP;
                a->oy-b->oy;
                break;
            }
            if(pw_abs(a->ox+PW_W+PW_GAP-b->ox)<34){
                a->ox=b->ox-PW_W-PW_GAP;
                a->oy=b->oy;
                break;
            }
        }
    }
}

static int solids(const Game*g,int r, Box*out, int ignore){
    const Room*room=&g->rooms[r];
    int n=0,i;
    for (i=0;i<room->nwalls;i++)out[n++]=room->walls[i];
    if(room->has_gate&&!pw_powered(g))out[n++]=room->gate;
    for(i=0;i<PW_CRATES;i++){
        const Crate*c=&g->crates[i];
        if(i!=ignore&&c->active&&!c->held&&c->room==r)out[n++]=body(c->x,c->y,c->w,c->h);
    }
    return n;
}

static int cleat_at(const Game*g,int r, float x, float y, float w, float h, int ignore){
    Box s[40],b=body(x,y,w,h);
    int i,n = solids(g,r,s,ignore);
    for(i=0;i<n;i++)if(pw_overlap(b,s[i]))return 0;
    return y-h/2>=1&&y+h/2<=PW_H;
}

/*shi to move body*/
static void move_body(Game*g,int *r, float *x, float *y, float*vx, float *vy, float w, float h, int*ground, int ignore, float dt, int actor){
    
}