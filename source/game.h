#ifndef PATCHWORK_GAME_H
#define PATCHWORK_GAME_H
/* i am currently refactrugbn the czurrent fgame*/
#ifdef _WIN32
#define CRT __declspec(dllimport)
CRT void *memset(void*,int,unsigned long long);
CRT void *memcpy(void*,const void *, unsigned long long);
CRT unsigned long long strlen(const char*);
CRT double sqrt(double);
CRT double sin(double);
CRT double cos(double);
CRT int _snprintf(char *, unsigned long long, const char*, ...);
#define snprintf _snprintf
#else
#include <string.h>
#include <math.h>
#include <stdio.h>
#endif
#define PW_W 432
#define PW_H 288
#define PW_TITLE 36
#define PW_GAP 12
#define PW_ROOMS 3
#define PW_CRATES 12
#define PW_ENEMIES 18
#define PW_SHOTS 96
#define PW_SPARKS 180
#define PW_MISSIONS 1
#define PW_DT (1.0f/120.0f)
typedef struct{
    float x,y;
}

Vec;
typedef struct {
    float x,y,w,h;
}

Box;
typedef struct{
    int active,room,kind,held,ground;
    /* kind 0: crate, 1: battery                              */
    float x,y,vx,vy,w,h,home_x,home_y;
    int home_room;
}

Crate;
typedef struct {
    int active,room,type,hp;
    float x,y,base_y,vx,timer,hurt;
}

Enemy;
typedef struct {
    int active,room,enemy;
    float x,y,vx,vy,life;
}

Shot;
typedef struct {
    int active,room;
    float x,y,vx,vy,life,maxlife;
    unsigned color;
}

Spark;
typedef struct{
    float ox,oy;
    char name[32];
    unsigned tint;
    Box walls[18], lava[4];
    int nwalls, nlava;
    Vec anchors[5];
    int nanchors;
    Box pad,socket,gate,spring,conveyor;
    int has_pad,repaired,has_socket,installed,has_gate,board[9];
}

Room;
typedef struct{
    int room,face,ground,hp,carry,hook,hook_room;
    float x,y,vx,vy,coyote,buffer,dash,dash_cd,gun_cd,hurt,rope;
}

Player;
typedef struct{
    float axis;
    int jump,jump_held,dash,fire,grapple,interact,up;
    float aim_x, aim_y;
    /*player room coords*/
}

Input;
typedef struct {
    int mission,nrooms,paused,won,editing,guide_step,effects;
    int require_pad,require_battery,require_clear,goal_room;
    float goal_x,goal_y,time,toast_time,shake;
    int walked,jumps,dragged,transfers,shots_fired,hits,kills,grapples,repairs,stacks,deaths;
    int sounds;
    unsigned seed;
    char toast[120];
    Room rooms[PW_ROOMS];
    Player p;
    Crate crates[PW_CRATES];
    Enemy enemies[PW_ENEMIES];
    Shot shots[PW_SHOTS];
    Spark sparks[PW_SPARKS];
}

Game;
typedef struct{
    const char *title,*tag,*brief,*lesson;
}

Mission;
extern const Mission pw_missions[PW_MISSIONS];
float pw_clamp(float v, float lo, float hi);
float pw_abs(float x);
float pw_length(float x, float y);
int pw_overlap(Box a, Box b);
void pw_load(Game * g,int mission);
void pw_step(Game *g,Input in, float dt);
void pw_move_room(Game *g,int room, float x, float y, int snap);
int pw_link(const Game*g, int room, int side);
int pw_powered(const Game*g);
int pw_alive(const Game*g);
int pw_board_solved(const int board[9]);
void pw_rotate(Game*g,int room, int cell);
void pw_interact(Game*g,int up);
void pw_hurt(Game *g, int damage, int respawn);
void pw_message(Game*g,const char *text);
void pw_effect(Game*g,int room, float x, float y, unsigned color, int count);
void pw_guide(const Game*g,char*title,char*body,int cap);
/*hi*/
enum{
    OP_RECT,OP_LINE,OP_CIRCLE,OP_TEXT
};
typedef struct {
    int kind;
    float x,y,w,h;
    unsigned color;
    float width;
    int font;
    char text[128];
}

DrawOp;
typedef struct {
 int count;
 DrawOp ops[2400];
}

Drawing;
void pw_rect(Drawing*d,float x, float y, float w, float h, unsigned c);
void pw_line(Drawing*d, float x, float y, float xx, float yy, unsigned c, float thickness);
void pw_circle(Drawing*d,float x,float y, float radius, unsigned c);
void pw_text(Drawing*d, float x, float y, int size, unsigned c, const char*text);
void pw_render(const Game*g, int room, Drawing*d);
#endif