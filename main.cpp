#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <stdint.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <cstdlib>
#include <ctime>
#include "Objects.h"


//Constantes
const int HEIGHT = 600;
const int WIDTH = 1200;
enum KEYS { UP, DOWN, LEFT, RIGHT, SPACE };
bool keys[5] = {false, false, false, false, false};
const int NUM_GROUND = 2;

//Variáveis do Allegro
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;

//Protótipos das Funções
void InitCharacter(Character &avatar, Ground ground[]);
void DrawCharacter(Character &avatar, float spr);
void MoveCharLeft(Character &avatar);
void MoveCharRight(Character &avatar);
bool JumpCharUp(Character &avatar);

void InitGround(Ground ground[], int size);
void DrawGround(Ground ground[], int size);
bool ColisionGround(Character &avatar, Ground ground[], int size);
bool ColisionGround2(Character &avatar, Ground ground[], int size);
void StartGround(Ground ground[], int size);
void UpdateGround(Ground ground[], int size);



//Função Principal
int main(void )
{
    //Variáveis primitivas
    bool done = false;
    bool redraw = true;
    bool falling = true;
    bool jumping = false;
    float GRAVITY = 0;
    const int FPS = 60;
    int cx_0 = 0;
    float sprite = 0;
    float score = 0;

    //Variáveis de objetos
    Character avatar;
    Ground ground[NUM_GROUND];

    if (!al_init())
        return -1;

    if (!al_init_image_addon())
        return -1;

    if (!al_install_keyboard())
        return -1;

    display = al_create_display(WIDTH, HEIGHT);
    if (!display)
        return -1;

    if (!al_init_primitives_addon())
        return -1;

    al_init_font_addon();

    if (!al_init_ttf_addon())
        return -1;

    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0/ FPS);

    srand(time(NULL));

    //Inicializar objetos
    InitGround(ground, NUM_GROUND);
    InitCharacter(avatar, ground);

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_start_timer(timer);

    //Imagens e Fontes

    ALLEGRO_BITMAP *cena_0 = al_load_bitmap("cena.png");
    ALLEGRO_FONT *font = al_load_font("batmfa__.ttf", 36, NULL);

    while(!done)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            redraw = true;

            if(cx_0 < -1199)cx_0 = 0;
            score += 0.5;
            cx_0 -= 6;

            if(sprite > 50)
            {
                sprite = 0;
            }
            else sprite += 3;

            al_draw_bitmap(cena_0,cx_0,0, ALLEGRO_FLIP_HORIZONTAL);
            al_draw_bitmap(cena_0,cx_0 + 1199,0, ALLEGRO_FLIP_HORIZONTAL);


            DrawCharacter(avatar, sprite);
            StartGround(ground, NUM_GROUND);
            UpdateGround(ground, NUM_GROUND);
            ColisionGround(avatar, ground, NUM_GROUND);
            ColisionGround2(avatar, ground, NUM_GROUND);

            if(avatar.y >= HEIGHT)
            {
                al_draw_text(font, al_map_rgb(255, 0, 0) , WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
            }
            else al_draw_textf(font, al_map_rgb(255, 0, 0), 10, HEIGHT / 50, ALLEGRO_ALIGN_LEFT, "Pontuacao: %.0f", score);


            if (keys[LEFT])
                MoveCharLeft(avatar);
            if (keys[RIGHT])
                MoveCharRight(avatar);
            if (keys[UP]){
                    int acel;
                    acel = 200;
                    avatar.y -= acel;
                    if(acel >= 200)
                        keys[UP] = false;
            }
        }


        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                done = true;
                break;
            case ALLEGRO_KEY_LEFT:
                keys[LEFT] = true;
                break;
            case ALLEGRO_KEY_RIGHT:
                keys[RIGHT] = true;
                break;
            case ALLEGRO_KEY_UP:
                keys[UP] = true;
                break;
            }
        }

        else if (ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                done = true;
                break;
            case ALLEGRO_KEY_LEFT:
                keys[LEFT] = false;
                break;
            case ALLEGRO_KEY_RIGHT:
                keys[RIGHT] = false;
                break;
            }
        }

        //Sair do Jogo
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
        }

        if (redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;

            StartGround(ground, NUM_GROUND);
            UpdateGround(ground, NUM_GROUND);
            DrawGround(ground, NUM_GROUND);

            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));
        }

        if (ColisionGround(avatar, ground, NUM_GROUND))
        {
            falling = false;
            GRAVITY = 0;
        }
        else
        {
            falling = true;
        }
        if (ColisionGround2(avatar, ground, NUM_GROUND))
        {
            falling = false;
            GRAVITY = 0;
        }
        else
        {
            falling = true;
        }

        if (falling == true)
        {
            GRAVITY += 0.5;
            avatar.y += GRAVITY;
        }
    }

    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_bitmap(cena_0);
    al_destroy_font(font);
    //Destruir objetos na tela

    return 0;
}

void InitCharacter(Character &avatar, Ground ground[])
{
    avatar.ID = PLAYER;
    avatar.x = 200;
    avatar.y = 100;
    avatar.lives = 3;
    avatar.speed = 7;
    avatar.bx = 100;
    avatar.by = 120;
    avatar.score = 0;
}

void DrawCharacter(Character &avatar, float spr)
{
    ALLEGRO_BITMAP *run_0 = al_load_bitmap("3_RUN_000.png");
    ALLEGRO_BITMAP *run_1 = al_load_bitmap("3_RUN_001.png");
    ALLEGRO_BITMAP *run_2 = al_load_bitmap("3_RUN_002.png");
    ALLEGRO_BITMAP *run_3 = al_load_bitmap("3_RUN_003.png");
    ALLEGRO_BITMAP *run_4 = al_load_bitmap("3_RUN_004.png");

    if(spr >= 0 && spr <= 10)
    {
        al_draw_bitmap(run_0,avatar.x-20,avatar.y -50, 0);
    }

    if(spr > 10 && spr <= 20)
    {
        al_destroy_bitmap(run_0);
        al_draw_bitmap(run_1,avatar.x-20,avatar.y -50, 0);
    }
    if(spr > 20 && spr <= 30)
    {
        al_destroy_bitmap(run_1);
        al_draw_bitmap(run_2,avatar.x-20,avatar.y -50, 0);
    }
    if(spr > 30 && spr <= 40)
    {
        al_destroy_bitmap(run_2);
        al_draw_bitmap(run_3,avatar.x-20,avatar.y -50, 0);
    }
    if(spr > 40 && spr <= 50)
    {
        al_destroy_bitmap(run_3);
        al_draw_bitmap(run_4,avatar.x-20,avatar.y -50, 0);
    }

    if(spr > 50){
        al_destroy_bitmap(run_4);
    }

}

void MoveCharLeft(Character &avatar)
{
    avatar.x -= avatar.speed;
    if (avatar.x + avatar.bx > WIDTH)
        avatar.x = WIDTH - avatar.bx;
    else if (avatar.x < 0)
        avatar.x = 0;
}

void MoveCharRight(Character &avatar)
{
    avatar.x += avatar.speed;
    if (avatar.x + avatar.bx > WIDTH)
        avatar.x = WIDTH - avatar.bx;
    else if (avatar.x < 0)
        avatar.x = 0;
}

void InitGround(Ground ground[], int size)
{
    for (int i=0; i < size; i++)
    {
        ground[i].ID = GROUND;
        ground[i].x = 200;
        ground[i].y = 500;
        ground[i].bx = 350;
        ground[i].by = 50;
        ground[i].speed = 10;
        ground[i].live = false;
        ground[i].cor = 200;
    }
}

void DrawGround(Ground ground[], int size)
{
    for(int i = 0; i < size; i++)
    {
        ALLEGRO_BITMAP *gnd = al_load_bitmap("ground.png");
        if(ground[i].live)
        {
            al_draw_bitmap(gnd,ground[i].x,ground[i].y -20, ALLEGRO_FLIP_HORIZONTAL);
        }
    }
}

bool ColisionGround(Character &avatar, Ground ground[], int size)
{
    for (int i = 0; i < size; i++)
    {
        if(ground[i].live)
        {
            if (avatar.y + avatar.by >= ground[i].y &&
                avatar.x <= ground[i].x + ground[i].bx &&
                avatar.x + avatar.bx >= ground[i].x &&
                avatar.y + avatar.by <= ground[i].y + ground[i].by)
            {
                avatar.y = ground[i].y - avatar.by;
                return true;
            }
            else
                return false;
        }
    }
}

bool ColisionGround2(Character &avatar, Ground ground[], int size)
{
    for (int i = 1; i < size; i++)
    {
        if(ground[i].live)
        {
            if (avatar.y + avatar.by >= ground[i].y &&
                avatar.x <= ground[i].x + ground[i].bx &&
                avatar.x + avatar.bx >= ground[i].x &&
                avatar.y + avatar.by <= ground[i].y + ground[i].by)
            {
                avatar.y = ground[i].y - avatar.by;
                return true;
            }
            else
                return false;
        }
    }
}

void StartGround(Ground ground[], int size)
{
    if (!ground[0].live)
    {
        ground[0].live = true;
        ground[0].x = WIDTH;
        ground[0].y = 50 + rand() % (HEIGHT -100);
        }
        if(ground[0].x + ground[0].bx == WIDTH - 500)
        {
            ground[1].live = true;
            ground[1].x = WIDTH;
            ground[1].y = 50 + rand() % (HEIGHT -100);
    }
}


void UpdateGround( Ground ground[], int size)
{
    for (int i=0; i < size; i++)
    {
        if (ground[i].live)
        {
            ground[i].x -= ground[i].speed;

            if (ground[i].x + ground[i].bx <= 0)
                ground[i].live = false;
        }
    }
}

