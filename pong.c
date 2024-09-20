#include<SDL2/SDL.h>
#include<stdint.h>
#include<stdbool.h>
#include <time.h>

typedef struct Ball{
    float x;
    float y; 
    float xspeed;
    float yspeed;
    int size;
} Ball;
typedef struct Player{
    int score;
    float yPosition;
} Player;
const int PLAYER_WIDTH = 20;
const int PLAYER_HEIGHT = 75;
const int PLAYER_MARGIN = 10;

const float PLAYER_MOVE_SPEED = 150.0f;
bool served = false;

const int WIDTH = 640;
const int HEIGHT= 480;
const int BALLSIZE = 10;
Ball ball;

Player player1;
Player player2;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

bool initialize(void);
void update(float);
void shutdown(void);
Ball makeBall(int size);
void updateBall(Ball* ball, float elapsed);
void renderBall(const Ball*);

Player makePlayer(void);
void updatePlayers(float elapsed);
void renderPlayers(void);

void updateScore(int player, int points);

int main(){
    //atexit(shutdown);
    srand((unsigned int) time(NULL));

    if (!initialize()){
        exit(1);
    }

    bool quit = false;
    SDL_Event event; 
    uint32_t lastTick = SDL_GetTicks();
    while(!quit){
        while(SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                quit = true;
            }
        }
        if (!quit){
            Uint32 currTick = SDL_GetTicks();
            Uint32 diff = currTick - lastTick;
            float elapsed = diff / 1000.0f;
            update(elapsed);
            lastTick = currTick;
        }
    }
    //SDL_Quit();
    return 0;
}
bool initialize(void){
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        fprintf(stderr, "Error initing video %s\n", SDL_GetError());
        return false;
    }

    window =  SDL_CreateWindow("Pong", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if(!window){
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer){
        return false;
    }
    ball = makeBall(BALLSIZE);
    player1 = makePlayer();
    player2 = makePlayer();
    return true;
}
void update(float elapsed){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    updateBall(&ball, elapsed);
    renderBall(&ball);

    updatePlayers(elapsed);
    renderPlayers();

    SDL_RenderPresent(renderer);

}
void shutdown(void){
    if(renderer) {SDL_DestroyRenderer(renderer); renderer = NULL;}
    if(window) {SDL_DestroyWindow(window); window = NULL;}
    SDL_Quit();
    exit(0);
    
}
bool coinFlip(void){
    return rand() % 2 == 1 ? true: false;
}
Ball makeBall(int size){
    //const float SPEED = 120;
    Ball ball = {
        .x = WIDTH / 2.0f - size / 2.0f,
        .y = HEIGHT / 2.0f - size / 2.0f,
        .size = size,
        .xspeed = coinFlip() ? 1 : -1,
        .yspeed = coinFlip() ? 1 : -1
    };
    return ball;
}

void renderBall(const Ball *ball){
    int size = ball->size;
    int halfsize = size / 2;
    SDL_Rect rect =  {
        .x = (int) ball->x - halfsize,
        .y = (int) ball->y - halfsize,
        .w = size,
        .h = size
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}
void updateBall(Ball* ball, float elapsed){
    if(!served){
        ball->x = WIDTH / 2;
        ball->y = HEIGHT / 2;
        return;
    }

    ball->x += ball->xspeed * elapsed * 100; 
    ball->y += ball->yspeed * elapsed * 100;

    if (ball->x < BALLSIZE / 2){
        //ball->xspeed = fabs(ball->xspeed);
        updateScore(2, 100);
    }
    if (ball->x > WIDTH - BALLSIZE / 2){
        //ball->xspeed = -fabs(ball->xspeed);
        updateScore(1, 100);
    }
    if (ball->y < BALLSIZE / 2){
        ball->yspeed = fabs(ball->yspeed);
    }
    if (ball->y > HEIGHT - BALLSIZE / 2){
        ball->yspeed = -fabs(ball->yspeed);
    }
}

Player makePlayer(void){
    Player player = {
        .yPosition = HEIGHT / 2
    };
    return player;

}
void updatePlayers(float elapsed){
    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
    if (keyboardState[SDL_SCANCODE_SPACE]){
        served = true;
    }

    if(keyboardState[SDL_SCANCODE_W]){
        player1.yPosition -= PLAYER_MOVE_SPEED * elapsed ;
    }

        if(keyboardState[SDL_SCANCODE_X]){
        player1.yPosition += PLAYER_MOVE_SPEED * elapsed ;
    }
    if(keyboardState[SDL_SCANCODE_UP]){
        player2.yPosition -= PLAYER_MOVE_SPEED * elapsed ;
    }

        if(keyboardState[SDL_SCANCODE_DOWN]){
        player2.yPosition += PLAYER_MOVE_SPEED * elapsed ;
    }

    if (player1.yPosition < PLAYER_HEIGHT / 2){
        player1.yPosition = PLAYER_HEIGHT / 2;
    }

    if (player1.yPosition > HEIGHT - PLAYER_HEIGHT / 2){
        player1.yPosition = HEIGHT - PLAYER_HEIGHT / 2;
    }
    if (player2.yPosition < PLAYER_HEIGHT / 2){
        player2.yPosition = PLAYER_HEIGHT / 2;
    }

    if (player2.yPosition > HEIGHT - PLAYER_HEIGHT / 2){
        player2.yPosition = HEIGHT - PLAYER_HEIGHT / 2;
    }

    SDL_Rect ballRect =  {
        .x = ball.x - ball.size / 2,
        .y = ball.y - ball.size / 2,
        .w = ball.size,
        .h = ball.size
    };
        SDL_Rect player1Rect = {
        .x = PLAYER_MARGIN,
        .y = (int)(player1.yPosition) - PLAYER_HEIGHT / 2,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };

    if(SDL_HasIntersection(&ballRect, &player1Rect)){
        ball.xspeed = fabs(ball.xspeed);
    }
    SDL_Rect player2Rect = {
        .x = WIDTH - PLAYER_WIDTH - PLAYER_MARGIN,
        .y = (int)(player2.yPosition) - PLAYER_HEIGHT / 2,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };
    if(SDL_HasIntersection(&ballRect, &player2Rect)){
        ball.xspeed = -fabs(ball.xspeed);
    }


}
void renderPlayers(void){
    // player1, left red
    SDL_SetRenderDrawColor(renderer,255, 0, 0, 255);
    SDL_Rect player1Rect = {
        .x = PLAYER_MARGIN,
        .y = (int)(player1.yPosition) - PLAYER_HEIGHT / 2,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };
    SDL_RenderFillRect(renderer, &player1Rect);

    // player2, right blue
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect player2Rect = {
        .x = WIDTH - PLAYER_WIDTH - PLAYER_MARGIN,
        .y = (int)(player2.yPosition) - PLAYER_HEIGHT / 2,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT
    };
    SDL_RenderFillRect(renderer, &player2Rect); 
}
void updateScore(int player, int points){
    served = false;
    if (player == 1){
        player1.score += points;
    }
    if (player == 2){
        player2.score += points;
    }
    char *fmt = "Player 1: %d | Player 2: %d";
    int len = snprintf(NULL, 0, fmt, player1.score, player2.score);
    char buf[len + 1];
    snprintf(buf, len + 1, fmt, player1.score, player2.score);
    SDL_SetWindowTitle(window, buf);
}