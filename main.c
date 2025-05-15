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
    float speed;
    Vector2 position;
    Vector2 direction;
    struct Bullet * next;
} Bullet;

//Inicializa texturas
static Texture2D background;
static Texture2D personagem;
static Texture2D bulletTex;

// Variaveis Globais:
//---Total-------------------------------
static const int screenWidth = 1280;
static const int screenHeight = 720;


static float tempo_jogado = 0.0f;
static float cronometro_last_spawn = 0.0f;//quanto tempo passou desde a ultima bala disparada
static float intervalo = 2.5f; //intervalo que as balas surgem
static float intervalo_minimo = 0.3f; //não pode disparar mais rapido que isso
static float qtd_diminuir_por_s = 0.1f;//rampa de dificulda (quanto maior, mais rapidamente fica dificil)

//---Balas-------------------------------
static float bullet_speed = 300.0f;
static float bullet_speed_increase = 10.0f; //incremento de velocidade
static float bullet_size = 40;


static bool gameOver = false;
static bool pause = false;
static bool victory = false;


static Player player = { 0 };
static struct Bullet *bullet = NULL;



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

    struct Bullet *b = (struct Bullet*)malloc(sizeof(Bullet));
    if (b != NULL){
        b->speed = bullet_speed;
        int direction = GetRandomValue(0, 3);
        if (direction == 0){//norte
            b->direction.x = 0;
            b->direction.y = 1;
            b->position.y= -bullet_size; //nasce com o tamanho da bala para fora da tela
            b->position.x = GetRandomValue(0, screenWidth - bullet_size);
        } else if (direction == 1){//sul
            b->direction.x = 0;
            b->direction.y = -1;
            b->position.y= screenHeight;
            b->position.x = GetRandomValue(0, screenWidth - bullet_size);
        } else if (direction == 2){//leste
            b->direction.x = -1;
            b->direction.y = 0;
            b->position.x= screenWidth;
            b->position.y = GetRandomValue(0, screenHeight - bullet_size);
        } else if (direction == 3){//oeste
            b->direction.x = 1;
            b->direction.y = 0;
            b->position.x= -bullet_size;
            b->position.y = GetRandomValue(0, screenHeight - bullet_size);
        }
        b->next = bullet;
        bullet = b;
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

    Bullet *b = bullet;
    while (b !=NULL){
        DrawRectangleV(b->position, (Vector2){bullet_size, bullet_size}, ORANGE);
        b = b->next;
    }
    



    EndDrawing();
}

void UpdateGame(void){
    //inicialização do tempo para aumentar dificuldade
    float deltaTime = GetFrameTime();
    tempo_jogado += deltaTime;
    cronometro_last_spawn += deltaTime; 

    

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

    Bullet *bullet_atual = bullet; //ponteiro para a principal
    Bullet *bullet_anterior = NULL; //ponteiro para a bala anterior

    while (bullet_atual != NULL){
        bullet_atual->position.x += bullet_atual->direction.x * bullet_atual->speed * deltaTime; //determina a movimentação x da bala
        bullet_atual->position.y += bullet_atual->direction.y * bullet_atual->speed * deltaTime; // determina a movimentação y da bala
        bool saiu = false;
        if (bullet_atual->position.x < -bullet_size || bullet_atual->position.x >= screenWidth || bullet_atual->position.y < -bullet_size || bullet_atual->position.y >= screenHeight){
            saiu = true;
        }

        if (saiu == true){
            Bullet *bullet_morta = bullet_atual;
            if (bullet_anterior == NULL){
                bullet = bullet_atual->next;
            } else{
                bullet_anterior->next = bullet_atual->next;
            }
            bullet_atual = bullet_atual->next;
            free(bullet_morta);
        } else{
            bullet_anterior = bullet_atual;
            bullet_atual = bullet_atual->next;
        }
        
    }

    if (cronometro_last_spawn >= intervalo){
        cronometro_last_spawn = 0.0f;
        if (intervalo > intervalo_minimo){
            intervalo -= qtd_diminuir_por_s;
            if (intervalo < intervalo_minimo){
                intervalo = intervalo_minimo;
            }
        }

        Bullet *new = (Bullet *)malloc(sizeof(Bullet));
        if (new != NULL){
            new->speed = bullet_speed + bullet_speed_increase * tempo_jogado;
            int direcao = GetRandomValue(0, 3);
            if (direcao == 0){//norte
                new->direction.x = 0;
                new->direction.y = 1;
                new->position.y= -bullet_size; //nasce com o tamanho da bala para fora da tela
                new->position.x = GetRandomValue(0, screenWidth - bullet_size);
            } else if (direcao == 1){//sul
                new->direction.x = 0;
                new->direction.y = -1;
                new->position.y= screenHeight;
                new->position.x = GetRandomValue(0, screenWidth - bullet_size);
            } else if (direcao == 2){//leste
                new->direction.x = -1;
                new->direction.y = 0;
                new->position.x= screenWidth;
                new->position.y = GetRandomValue(0, screenHeight - bullet_size);
            } else if (direcao == 3){//oeste
                new->direction.x = 1;
                new->direction.y = 0;
                new->position.x= -bullet_size;
                new->position.y = GetRandomValue(0, screenHeight - bullet_size);
            }
            new->next = bullet;
            bullet = new;
        }
    }



}

void UnloadGame(void){
    UnloadTexture(background);
    UnloadTexture(personagem);

    Bullet *bala = bullet;
    while (bala != NULL){
        Bullet *next = bala->next;
        free(bala);
        bala = next;
    }
}
