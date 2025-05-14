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
    Inventario backpack;
} Player;


typedef struct Bullet{
    float speed;
    Texture2D texture;
    Vector2 position;
    int damage;
    bool ativo;
} Bullet;


static Texture2D background;
static Texture2D personagem;

// Variaveis Globais:
static const int screenWidth = 1280;
static const int screenHeight = 720;
static int player_speed = 7;

static bool gameOver = false;
static bool pause = false;
static bool victory = false;

static Player player = { 0 };
//static Bullet bullet[tamanho] = { 0 }; //ajustar

static int destroyedBulletsCount = 0;

static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)


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
    personagem = LoadTexture("assets/Ruhtra.png");

    player.position = (Vector2){screenWidth - 40, screenHeight - 50};
    
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
    if (IsKeyDown(KEY_A)){
        player.position.x -= player_speed;
    } else if (IsKeyDown(KEY_D)){
        player.position.x += player_speed;
    } else if (IsKeyDown(KEY_W)){
        player.position.y -= player_speed;
    } else if (IsKeyDown(KEY_S)){
        player.position.y += player_speed;
    }
}

void UnloadGame(void){
    UnloadTexture(background);
    UnloadTexture(personagem);
}