#include "game.h"
/*tiny display list keeps painting independent of bothg simulation and win32*/
static DrawOp*op(Drawing*d,int kind, float x, float y, float w, float h, unsigned c){
    DrawOp*p;
    if(d->count>=2400)return 0;
    p=&d->ops[d->count++];
    memset(p,0,sizeof(*p));
    p->kind=kind;
    p->x=x;
    p->y=y;
    p->w=w;
    p->h=h;
    p->color=c;
    return p;
}

void pw_rect(Drawing*d,float x, float y, float w, float h, unsigned c){
    op(d,OP_RECT,x,y,w,h,c);
}

void pw_line(Drawing*d,float x, float y, float xx, float yy, unsigned c, float t){
    DrawOp*p=op(d, OP_LINE,x,y,xx,yy,c);
    if(p)p->width=t;
}

void pw_circle(Drawing*d,float x, float y, float radius, unsigned c){
    op(d,OP_CIRCLE,x,y,radius,0,c);
}

void pw_text(Drawing*d,float x, float y, int size, unsigned c, const char*s){
    DrawOp*p=op(d,OP_TEXT,x,y,0,0,c);
    if(p){
        p->font = size;
        snprintf(p->text,128,"%s",s);
    }
}

void pw_render(const Game*g, int n, Drawing*d){
    const Room*r=&g->rooms[n];int i; d->count=0;
    pw_rect(d,0,0,PW_W,PW_H+PW_TITLE,0xe8e5d7);
    pw_rect(d,16,15,14,0x254b52,r->name);
    pw_text(d,16,55,12,0x254b52,"A/D move | Space jump | Shift dash | R restart | Esc pause");
    for(i=0;i<r->nwalls;i++){Box b=r->walls[i];pw_rect(d,b.x,b.y+PW_TITLE,b.w,b.h,0x47676c);}
    for(i=0;i<2;i++)pw_rect(d,i?PW_W-7:0,201+PW_TITLE,7,65,pw_link(g,n,i?1:-1)>=0?0x199f96:0xaf7352);
    if(n==g->goal_room)pw_rect(d,g->goal_x-13,g->goal_y-20+PW_TITLE,26,40,g->won?0xe8bb39:0x199f96);
    if(n==g->p.room){
        pw_rect(d,g->p.x-10,g->p.y-14+PW_TITLE,20,28,0xe66b4a);
        pw_rect(d,g->p.x-6,g->p.y-10+PW_TITLE,12,7,0x254b52);
        pw_text(d,16,79,12,0x254b52,g->paused?"Paused. Esc resumes.":g->won?"Dock reached. R restarts.":"Move the normal Windows title bars while playing.");
    }
}