#include "raylib.h"
#include <stdio.h>


#include <math.h>

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
} Player;


typedef struct Bullet{
    float speed;
    Vector2 position;
    Vector2 direction;
    //int damage;
    //bool ativo;
    struct Bullet * next;
} Bullet;

//Inicializa texturas
static Texture2D background;
static Texture2D personagem;
static Texture2D bullet;

// Variaveis Globais:
static const int screenWidth = 1280;
static const int screenHeight = 720;

//

static bool gameOver = false;
static bool pause = false;
static bool victory = false;

static Player player = { 0 };
//static Bullet bullet[tamanho] = { 0 }; //ajustar

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

    //Carregar os sprites as suas respectivas funções
    background = LoadTexture("assets/background.png");
    personagem = LoadTexture("assets/Unarmed_Idle_full.png");
    bullet = LoadTexture("assets/bullet.png");

    //inicializa as variáveis do player
    player.position = (Vector2){screenWidth - 40, screenHeight - 50};
    player.vida = 3;
    player.speed = 7;
    player.backpack.madeira = 0;
    player.backpack.pedra = 0;
    
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
    DrawRectangle(player.position.x, player.position.y, 40, 40, RED);



    EndDrawing();
}

void UpdateGame(void){
    //inicialização do tempo para aumentar dificuldade
    float deltaTime = GetFrameTime();
    tempo_decorrido += deltaTime; 



    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)){
        player.position.x -= player.speed;
    } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)){
        player.position.x += player.speed;
    } else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)){
        player.position.y -= player.speed;
    } else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)){
        player.position.y += player.speed;
    }
}

void UnloadGame(void){
    UnloadTexture(background);
    UnloadTexture(personagem);
}