// gcc -o main main.c function.c -lSDL2

#include <SDL2/SDL.h>
#include <unistd.h>
#include "function.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320
#define FPS 60

#define PADDLE_HEIGHT 20
#define PADDLE_WIDTH WINDOW_WIDTH/3
#define PADDLE_SPEED 3
#define BALL_RADIUS 9
#define NB_LIGNE 4
#define NB_BRIQUE 10
#define GAP_VERTI_BRICK 8
#define GAP_HORIZ_BRICK 8
#define NB_LIVES_BRICKS 1
#define PIX_WALL 130
#define IN_GAME 1
#define LOSE 2
#define WIN 3
#define PAUSE 0

float paddle_x;
float paddle_y;
float ball_x;
float ball_y;
float ball_speed_x = -2;
float ball_speed_y = -1;
int nb_global_bricks = NB_LIGNE * NB_BRIQUE;
int state_game = PAUSE;

int initBricks();

struct rectangle {
    double pos_x;
    double pos_y;
    double length;
    double height;
    int red;
    int green;
    int blue;
};

struct brick {
    struct rectangle scrawlBrick;
    int lives;
};

struct brick tabBricks[NB_LIGNE][NB_BRIQUE];

void init_game() {
    paddle_x = 30;
    paddle_y = WINDOW_HEIGHT - 50;
    ball_x = (paddle_x + PADDLE_WIDTH) / 2;
    ball_y = paddle_y - BALL_RADIUS;
    initBricks();
}



/**
 * Draw ball
 * @param ball
*/
void drawBall() {
    changeColor(255,255,255);
    drawCircle(ball_x, ball_y, BALL_RADIUS);
}

void drawPaddle() {
    changeColor(116,148,169);
    drawRect(paddle_x, paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT);
}

void drawBricks() {
    
    int i, j;

    for (i = 0; i < NB_LIGNE; i++) {
        for (j = 0; j < NB_BRIQUE; j++) {
            if (tabBricks[i][j].lives >= 1) {
                changeColor(tabBricks[i][j].scrawlBrick.red,tabBricks[i][j].scrawlBrick.green,tabBricks[i][j].scrawlBrick.blue);
                drawRect(tabBricks[i][j].scrawlBrick.pos_x, tabBricks[i][j].scrawlBrick.pos_y, tabBricks[i][j].scrawlBrick.length, tabBricks[i][j].scrawlBrick.height);
            }
        }
    }
}

void drawGame() {
    clear();
    drawPaddle();
    drawBall();
    drawBricks();
    actualize();
    usleep(1000000 / FPS); // 60 images par seconde | 1000000 = 1 seconde
}



/**
 * Moves the paddle along the x-axis at the given speed
 * @param speed (-1 : to the left | 1 : to the right)
*/
void MovePaddle(int speed) { 
    paddle_x += speed;
    if (paddle_x < 0) {
        paddle_x = 0;
    }
    else if (paddle_x + PADDLE_WIDTH > WINDOW_WIDTH) {
        paddle_x = WINDOW_WIDTH - PADDLE_WIDTH;
    }
}

void KeyPressed(SDL_Keycode touche) {
    switch (touche) {
        case SDLK_q:
            MovePaddle(-PADDLE_SPEED);
            break;
        case SDLK_d:
            MovePaddle(PADDLE_SPEED);
            break;
        case SDLK_ESCAPE:
            freeAndTerminate();
            break;
        case (SDLK_q && SDLK_d):
            state_game = IN_GAME; 
        default:
            break;
    }
}

void joyButtonPressed() {

}

/**
 * Collision between ball and wall
*/
void WallCollision() {
    //Top wall
    if (ball_y - BALL_RADIUS <= 0) {
        ball_speed_y = -ball_speed_y;
        ball_y = BALL_RADIUS;
    }
    //Bot wall
    if (ball_y + BALL_RADIUS >= WINDOW_HEIGHT) {
        state_game = LOSE;
    }
    
    //Left wall 
    if (ball_x - BALL_RADIUS <= 0) {
        ball_speed_x = -ball_speed_x;
        ball_x = BALL_RADIUS;
    }
    //Right wall
    if (ball_x + BALL_RADIUS >= WINDOW_WIDTH) {
        ball_speed_x = -ball_speed_x;
        ball_x = WINDOW_WIDTH - BALL_RADIUS;
    }
}

/**
 * Collision between ball and paddle
*/
void PaddleCollision() {
    //Position relative of the ball
    int top_ball = ball_y - BALL_RADIUS;
    int bot_ball = ball_y + BALL_RADIUS;
    int left_ball = ball_x - BALL_RADIUS;
    int right_ball = ball_x + BALL_RADIUS;

    //Position relative of the paddle
    int top_paddle = paddle_y;
    int bot_paddle = paddle_y + PADDLE_HEIGHT;
    int left_paddle = paddle_x;
    int right_paddle = paddle_x + PADDLE_WIDTH;
    int third_paddle_width = PADDLE_WIDTH / 3;

    //
    int change_speed_x = 0, change_speed_y = 0;

    if (bot_ball - top_paddle >= 0 && bot_ball - top_paddle <= ball_speed_y) {
        // ball hit top left side
        if (ball_x >= left_paddle && ball_x <= left_paddle + third_paddle_width) { 
            if (ball_speed_x != 0)
                ball_speed_x = -abs(ball_speed_x);
            else
                ball_speed_x = -1;
        }
        // ball hit top middle side
        else if (ball_x >= left_paddle + third_paddle_width && ball_x <= left_paddle + third_paddle_width * 2) {
            //printf("Deuxième tier\n");
        }
        // ball hit top right side
        else if (ball_x >= left_paddle + third_paddle_width * 2 && ball_x <= right_paddle) {
            if (ball_speed_x != 0)
                ball_speed_x = abs(ball_speed_x);
            else
                ball_speed_x = 1;
        }
    }
    
    //Axe Vertical
    if (right_ball >= left_paddle && left_ball <= right_paddle) {
        //Top collision
        if (bot_ball - top_paddle >= 0 && bot_ball - top_paddle <= ball_speed_y) {
            ball_speed_y = -ball_speed_y;
        }
        // Bottom collision
        else if (top_ball - bot_paddle <= 0 && top_ball - bot_paddle >= ball_speed_y) {
            ball_speed_y = -ball_speed_y;
        }
    }

    //Axe Horizontal
    if (bot_ball >= top_paddle && top_ball <= bot_paddle) {
        //Left collision
        if (right_ball - left_paddle >= 0 && right_ball - left_paddle <= abs(ball_speed_x) + PADDLE_SPEED) {
            ball_x = left_paddle - BALL_RADIUS;
            change_speed_x = 1;
        } 
        //Right collision
        else if (left_ball - right_paddle <= 0 && left_ball - right_paddle >= abs(ball_speed_x) - PADDLE_SPEED) {
            ball_x = right_paddle + BALL_RADIUS;
            change_speed_x = 1;
        }
    }

    if (change_speed_y)
        ball_speed_y = -ball_speed_y;
    if (change_speed_x)
        ball_speed_x = -ball_speed_x;
}

/**
 * Collision between ball and brick
*/
void BrickCollision(int i, int j) {
    if (tabBricks[i][j].lives == 0) {
        return;
    }
    
    //Position relative of the ball
    int top_ball = ball_y - BALL_RADIUS;
    int bot_ball = ball_y + BALL_RADIUS;
    int left_ball = ball_x - BALL_RADIUS;
    int right_ball = ball_x + BALL_RADIUS;

    //Position relative of the brick
    int top_brick = tabBricks[i][j].scrawlBrick.pos_y;
    int bot_brick = tabBricks[i][j].scrawlBrick.pos_y + tabBricks[i][j].scrawlBrick.height;
    int left_brick = tabBricks[i][j].scrawlBrick.pos_x;
    int right_brick = tabBricks[i][j].scrawlBrick.pos_x + tabBricks[i][j].scrawlBrick.length;

    //
    int change_speed_x = 0, change_speed_y = 0;

    //Axe Vertical
    if (right_ball >= left_brick && left_ball <= right_brick) {
        //Top collision
        if (bot_ball - top_brick >= 0 && bot_ball - top_brick <= ball_speed_y) {
            change_speed_y = 1;
        }
        //Bottom collision
        else if (top_ball - bot_brick <= 0 && top_ball - bot_brick >= ball_speed_y) {
            change_speed_y = 1;
        }
    }
    //Axe Horizontal
    if (bot_ball >= top_brick && top_ball <= bot_brick) {
        //Left collision
        if (right_ball - left_brick >= 0 && right_ball - left_brick <= ball_speed_x) {
            change_speed_x = 1;
        }
        //Right collision
        else if (left_ball - right_brick <= 0 && left_ball - right_brick >= ball_speed_x) {
            change_speed_x = 1;
        }
    }

    if (change_speed_y)
        ball_speed_y = -ball_speed_y;
    if (change_speed_x)
        ball_speed_x = -ball_speed_x;

    if (change_speed_x == 1 || change_speed_y == 1) {
        tabBricks[i][j].lives--;
        if (tabBricks[i][j].lives == 0) {
            nb_global_bricks--;
            if (nb_global_bricks == 0) {
                state_game = WIN;
            }
        }
    }

}

/**
 * Collision between ball and bricks
*/
void BricksCollision() {
    for(int i = 0; i < NB_LIGNE; i++) {
        for(int j = 0; j < NB_BRIQUE; j++) {
            BrickCollision(i, j);
        }
    }
}

void moveBall() {
    ball_x += ball_speed_x;
    ball_y += ball_speed_y;

    WallCollision();
    PaddleCollision();
    BricksCollision();
}

void gameLoop() {
    int programLaunched = 1;
    while (programLaunched == 1) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // boucle pour la gestion d'évenement
            switch (event.type) {
                case SDL_QUIT:
                    programLaunched = 0;
                    break;
                case SDL_MOUSEBUTTONUP:
                    printf("position de la souris x : %d , y : %d\n", event.motion.x, event.motion.y);
                    break;
                case SDL_KEYDOWN:
                    KeyPressed(event.key.keysym.sym);
                    break;
                case SDL_JOYBUTTONDOWN:
                    break;
                default:
                    break;
            }
        }
        if (state_game == IN_GAME) {
            moveBall();
            drawGame();
        }
    }
}

int main() {
    init(WINDOW_WIDTH, WINDOW_HEIGHT);
    init_game();
    gameLoop();
    printf("Fin du programme\n");
    freeAndTerminate();
}

int initBricks (){
    /*On vient indiquer les coordonnées de mes briques.
    Le tableau va garder en mémoire les positions de chaque case (briques).*/

    int i, j;

    for (i = 0; i < NB_LIGNE; i++) {
        for (j = 0; j < NB_BRIQUE; j++) {
            tabBricks[i][j].lives = NB_LIVES_BRICKS;           
            if ((i+j)%2 == 0) {
                tabBricks[i][j].scrawlBrick.red = 251;
                tabBricks[i][j].scrawlBrick.green = 226;
                tabBricks[i][j].scrawlBrick.blue = 22;
            } else {
                tabBricks[i][j].scrawlBrick.red = 175;
                tabBricks[i][j].scrawlBrick.green = 165;
                tabBricks[i][j].scrawlBrick.blue = 209;
            }
            tabBricks[i][j].scrawlBrick.height = ((PIX_WALL / NB_LIGNE) - GAP_VERTI_BRICK);
            tabBricks[i][j].scrawlBrick.length = (((WINDOW_WIDTH - GAP_HORIZ_BRICK)/NB_BRIQUE)-GAP_HORIZ_BRICK);
            /* Formule qui calcule la position x de chaque case du tableau*/
            tabBricks[i][j].scrawlBrick.pos_x = (GAP_HORIZ_BRICK + ((tabBricks[i][j].scrawlBrick.length + GAP_HORIZ_BRICK)*j));
            /* Formule qui calcule la position y de chaque case du tableau*/
            tabBricks[i][j].scrawlBrick.pos_y = (GAP_VERTI_BRICK +((tabBricks[i][j].scrawlBrick.height + GAP_VERTI_BRICK)*i));
        }
    }
}