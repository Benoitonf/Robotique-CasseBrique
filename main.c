// gcc -o main main.c function.c -lSDL2

#include <SDL2/SDL.h>
#include <unistd.h>
#include "function.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320
#define FPS 60

#define PADDLE_WIDTH WINDOW_WIDTH/3
#define BALL_RADIUS 9

float paddle_x;
float paddle_y;
float ball_x;
float ball_y;
float ball_speed_x = 0;
float ball_speed_y = -1;


void init_game() {
    paddle_x = 30;
    paddle_y = WINDOW_HEIGHT - 50;
    ball_x = (paddle_x + PADDLE_WIDTH) / 2;
    ball_y = paddle_y - BALL_RADIUS;
}

/**
 * Draw ball
 * @param ball
*/
void drawBall() {
    changeColor(100,50,200);
    drawCircle(ball_x, ball_y, BALL_RADIUS);
}

void drawPaddle() {
    changeColor(0,0,255);
    drawRect(paddle_x, paddle_y, PADDLE_WIDTH, 20);
}

void drawGame() {
    clear();
    drawPaddle();
    drawBall();
    actualize();
    usleep(1000000 / FPS); // 60 images par seconde | 1000000 = 1 seconde
}

/**
 * Moves the paddle along the x-axis at the given speed
 * @param speed (-1 : to the left | 1 : to the right)
*/
void MovePaddle(int speed) {
    paddle_x += speed;
    if (paddle_x < 0)
        paddle_x = 0;
    else if (paddle_x + PADDLE_WIDTH > WINDOW_WIDTH)
        paddle_x = WINDOW_WIDTH - PADDLE_WIDTH;
}

void KeyPressed(SDL_Keycode touche) {
    switch (touche) {
        case SDLK_q:
            printf("Q\n");
            MovePaddle(-1);
            break;
        case SDLK_d:
            printf("D\n");
            MovePaddle(1);
            break;
        case SDLK_ESCAPE:
            freeAndTerminate();
            break;
        default:
            break;
    }
}

void joyButtonPressed() {

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
        
        drawGame();
    }
}

int main() {
    init(WINDOW_WIDTH, WINDOW_HEIGHT);
    init_game();
    gameLoop();
    printf("Fin du programme\n");
    freeAndTerminate();
}