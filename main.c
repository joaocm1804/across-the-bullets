#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include <math.h>

#define MAX_BULLET  2


typedef struct Inventario{
    int pedra;
    int madeira;
} Inventario;

typedef struct Player{
    Vector2 position;
    Texture2D texture;
    int vida;
    int speed;
    Inventario backpack;
    int width;
    int height;
} Player;


typedef struct Bullet{
    int speed;
    Vector2 position;
    Vector2 direction;
    //int damage;
    bool ativo;
    struct Bullet * next;
} Bullet;

//Inicializa texturas
static Texture2D background;
static Texture2D personagem;
static Texture2D bulletTex;

// Variaveis Globais:
static const int screenWidth = 1280;
static const int screenHeight = 720;

//

static bool gameOver = false;
static bool pause = false;
static bool victory = false;


static Player player = { 0 };
static Bullet bullet[MAX_BULLET] = { 0 }; //ajustar


static float tempo_decorrido = 0;

static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game



int main(void)
{
    InitWindow(screenWidth, screenHeight, "Across the Bullets");


    InitGame();

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        UpdateGame();
        DrawGame();
    }

    UnloadGame();        

    CloseWindow(); 


    return 0;
}

void InitGame(void){
    srand(time(NULL));

    int x_pos, y_pos;
    //Inicializa variaveis bullet
    for (int i = 0; i <MAX_BULLET; i++){
        bullet[i].speed = 8;
    }



    //Carregar os sprites as suas respectivas funções
    background = LoadTexture("assets/background.png");
    personagem = LoadTexture("assets/Unarmed_Idle_full.png");
    bulletTex = LoadTexture("assets/bullet.png");

    //inicializa as variáveis do player
    player.position = (Vector2){screenWidth - player.width, screenHeight - player.width};
    player.width = 40;
    player.height = 40; 
    player.vida = 3;
    player.speed = 7;
    player.backpack.madeira = 0;
    player.backpack.pedra = 0;

    for (int i = 0; i <MAX_BULLET; i++){
        x_pos = GetRandomValue(0, screenWidth);

        y_pos = GetRandomValue(0, screenHeight);

        bullet[i].position = (Vector2){x_pos, y_pos};
        bullet[i].ativo = true;

    }
    
}

void DrawGame(void){
    BeginDrawing();
    ClearBackground(RAYWHITE);

    //Desenha o background
    int backgroundWidth = background.width;
    int backgroundHeight = background.height;
    for (int y = 0; y < screenHeight; y += backgroundHeight){
        for (int x = 0; x < screenWidth; x += backgroundWidth){
            DrawTexture(background, x, y, WHITE);
        }
    }

    int rectangleSize = 40;

    //Desenhar boneco
    DrawRectangle(player.position.x, player.position.y, player.width, player.height, RED);

    for (int i = 0; i < MAX_BULLET; i++)
    {
        if (bullet[i].ativo == true){
            DrawRectangle(bullet[i].position.x, bullet[i].position.y, 40, 40, ORANGE);
        }
    }
    



    EndDrawing();
}

void UpdateGame(void){
    //inicialização do tempo para aumentar dificuldade
    float deltaTime = GetFrameTime();
    tempo_decorrido += deltaTime; 

    

    //  movimentacao do jogador inclusive na diagonal
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)){
        player.position.x -= player.speed;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)){
        player.position.x += player.speed;
    }
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)){
        player.position.y -= player.speed;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)){
        player.position.y += player.speed;
    }

    // limitando a movimentação a tela
    if (player.position.x < 0){ // limitando a esquerda da tela
        player.position.x = 0;
    }
    if (player.position.x > screenWidth - player.width){ // limite a direita da tela
        player.position.x = screenWidth - player.width;
    }
    if (player.position.y < 0){ // limiite para cima
        player.position.y = 0;
    }
    if (player.position.y > screenHeight - player.height){
        player.position.y = screenHeight - player.height;
    }

    for (int i = 0; i < MAX_BULLET; i++)
    {
        if (bullet[i].ativo){
            bullet[i].position.x += bullet[i].speed;
            bullet[i].position.y += bullet[i].speed;
        }
    }
    


}

void UnloadGame(void){
    UnloadTexture(background);
    UnloadTexture(personagem);
}