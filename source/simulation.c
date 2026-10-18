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
                a->oy=b->oy;
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

static int clear_at(const Game*g,int r, float x, float y, float w, float h, int ignore){
    Box s[40],b=body(x,y,w,h);
    int i,n = solids(g,r,s,ignore);
    for(i=0;i<n;i++)if(pw_overlap(b,s[i]))return 0;
    return y-h/2>=1&&y+h/2<=PW_H;
}

/*shi to move body*/
static void move_body(Game*g,int *r, float *x, float *y, float*vx, float *vy, float w, float h, int*ground, int ignore, float dt, int actor){
    Box s[40];
    int i,n=solids(g,*r,s,ignore);
    float oldx=*x;
    *x+=*vx*dt;
    for(i=0;i<n;i++)if(pw_overlap(body(*x,*y,w,h),s[i])){
        if(*vx>0)*x=s[i].x-w/2;
        else if(*vx<0)*x=s[i].x+s[i].w+w/2;
        *vx=0;
    }
    if(*x-w/2<0||*x+w/2>PW_W){
        int side=*x<PW_W/2?-1:1,next=pw_link(g,*r,side);
        float yy=*y;
        float xx=side>0?w/2+1:PW_W-w/2-1;
        if(next>=0)yy+=g->rooms[*r].oy-g->rooms[next].oy;
        if(next>=0&&*y-h/2>=201&&*y+h/2<=267&&yy-h/2>=201&&yy+h/2<=267&&clear_at(g,next,xx,yy,w,h,ignore)){
            int prior=*r;
            *r=next;
            *x=xx;
            *y=yy;
            n=solids(g,*r,s,ignore);
            if(actor){
                g->transfers++;
                g->p.hook=-1;
                pw_effect(g,prior,side>0?PW_W-2:2,*y,0x1baab0,9);
                g->sounds|=32;
                if(g->p.carry>=0)g->crates[g->p.carry].room=next;
            }
        }
        else{
            *x=pw_clamp(*x,w/2,PW_W-w/2);
            *vx=0;
        }
    }
    *ground=0;
    *y+=*vy*dt;
    for(i=0;i<n;i++)if(pw_overlap(body(*x,*y,w,h),s[i])){
        if(*vy>=0){
            *y=s[i].y-h/2;
            *ground=1;
        }
        else *y=s[i].y+s[i].h+h/2;
        *vy=0;
    }
    if(*y<h/2){
        *y=h/2;
        *vy=0;
    }
    if(actor&&pw_abs(*x-oldx)>0.2f)g->walked++;
}

static void respawn(Game*g){
    Player*p=&g->p;
    int i;
    pw_effect(g,p->room,p->x,p->y,0xe95e45,26);
    g->deaths++;
    p->hp=5;
    p->hurt=1.6f;
    p->room=0;
    p->x=54;
    p->y=238;
    p->vx=p->vy=0;
    p->hook=-1;
    p->dash=0;
    if(p->carry>=0){
        Crate*c=&g->crates[p->carry];
        c->held=0;
        c->room=c->home_room;
        c->x=c->home_x;
        c->y=c->home_y;
        c->vx=c->vy=0;
        p->carry=-1;
    }
    /*clear nearby hostile projectiles instead of instant damagin respawn*/
    for(i=0;i<PW_SHOTS;i++)if(g->shots[i].enemy)g->shots[i].active=0;
    pw_message(g,"Reassembled. Repairs and cleared drones are kept.");
    g->sounds|=4;
}

void pw_hurt(Game*g,int damage, int reset){
    (void)g; (void)damage; (void)reset;
}

/*N E S W are bits 1 2 4 8 and Neighbours must agree about a shared copper egde */
int pw_board_solved(const int b[9]){(void)b;return 0;}

void pw_rotate(Game*g,int r, int cell){
    (void)g;(void)r;(void)cell;
}

void pw_interact(Game*g, int up){
    (void)g; (void)up;
}

static void fire(Game*g, int room, float x, float y, float vx, float vy, int enemy){
    (void)g;(void)room;(void)x;(void)y;(void)vx;(void)vy;(void)enemy;
}

static int segment_blocked(const Game*g, int r, float ax, float ay, float bx, float by){(void)g;(void)r;(void)ax;(void)ay;(void)bx;(void)by;return 1;}

static void update_player(Game*g, Input in, float dt){
    Player*p=&g->p;
    float prior_y=p->y;
    p->hurt=approach(p->hurt,0,dt);
    p->dash_cd=approach(p->dash_cd,0,dt);
    p->gun_cd=approach(p->gun_cd,0,dt);
    p->dash=approach(p->dash,0,dt);
    if(in.interact)pw_interact(g,in.up);
    if(g->editing>=0){
        p->vx=0;
        p->hurt=.1f;
        in.axis=0;
        in.jump=in.dash=in.fire=in.grapple=0; /**/
    }
    if(in.axis!=0)p->face=in.axis>0?1:-1;
    if(in.jump&&p->ground){p->vy=-395;p->ground=0;g->jumps++;}
    p->vx=in.axis*210;p->vy+=1060*dt;
    p->vx=pw_clamp(p->vx,-650,650);
    p->vy=pw_clamp(p->vy,-590,700);
    move_body(g,&p->room,&p->x,&p->y,&p->vx,&p->vy,20,28,&p->ground,-1,dt,1);
    if(prior_y+1<p->y&&p->ground)pw_effect(g,p->room,p->x,p->y+14,0x72868a,3);
    if (p->room==g->goal_room&&pw_length(p->x-g->goal_x,p->y-g->goal_y)<31){
        if(pw_powered(g)&&(!g->require_clear||pw_alive(g)==0)){
            g->won=1;
            g->editing=-1;
            g->sounds|=64;
            pw_effect(g,p->room,p->x,p->y,0x15a896,55);
            pw_message(g,"Assembly complete. Enter for the next job.");
        }
        else if(g->toast_time<.1f)pw_message(g,!pw_powered(g)?"The dock needs a repaired circuit and its cell.":"Clear the remaining drones before docking.");
    }   
}

static void update_crates(Game*g,float dt){
    (void)g;(void)dt;
}

static void update_enemies(Game*g,float dt){
    (void)g;(void)dt;
}

static void update_shots(Game*g,float dt){
    (void)g;(void)dt;
}

void pw_step(Game*g,Input in, float dt) {
    int i;
    if(g->paused)return;
    dt=pw_clamp(dt,0,.025f);
    g->time+=dt;
    g->toast_time=approach(g->toast_time,0,dt);
    g->shake=approach(g->shake,0,dt*17);
    for(i=0;i<PW_SPARKS;i++){
        Spark*s=&g->sparks[i];
        if(s->active){
            s->life-=dt;
            s->x+=s->vx*dt;
            s->y+=s->vy*dt;
            s->vy+=320*dt;
            if(s->life<=0)s->active=0;
        }
    }
    if(g->won)return;
    update_crates(g,dt);
    update_player(g,in,dt);
    update_enemies(g,dt);
    update_shots(g,dt);
}
/*-*/
/*_*/
void pw_guide(const Game*g,char*t,char*b,int cap) {
    snprintf(t,cap,"%s",g->won?"Dock reached":"Repair shift");
    snprintf(b,cap,"%s",g->won?"R restarts this job.":"A/D move. Space jumps. Move the windows to connect doors.");
}