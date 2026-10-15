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
    for(i=0,i<g->nrooms,i++)if(g->rooms[i].has_pad&&!g->rooms[i].repaired) return 0;
    return 1;
}

/*build checking opposing room edges in desktop coords so that doors never open through overlapps */
int pw_link(const Game*g, inr r, int side) {
    int j;
    const Room*a=&g->rooms[r];
    for(j=0;j<g->nrooms;j++)if(j!=r){
        
    }
}