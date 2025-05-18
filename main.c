#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
    int width;
    int height;
    float resultado_tempo;
    bool colisao;
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
static Texture2D vida;

// Variaveis Globais:
//---Total-------------------------------
static const int screenWidth = 1280;
static const int screenHeight = 720;


static float tempo_jogado = 0.0f;
static float cronometro_last_spawn = 0.0f;//quanto tempo passou desde a ultima bala disparada
static float intervalo = 2.5f; //intervalo que as balas surgem
static float intervalo_minimo = 0.3f; //não pode disparar mais rapido que isso
static float qtd_diminuir_por_s = 0.1f;//rampa de dificulda (quanto maior, mais rapidamente fica dificil)

// PISCAR 
int frameCount = 0;

//---Balas-------------------------------
static float bullet_speed = 300.0f;
static float bullet_speed_increase = 10.0f; //incremento de velocidade
static float bullet_size = 40;


static bool gameOver = false;
static bool pause = false;
static bool victory = false;
static bool game_start = false;
static Texture2D homescreen;


static Player player = { 0 };
static struct Bullet *bullet = NULL;

static Music homescreen_music;
static Music game_music;



static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game



int main(void)
{
   
    InitWindow(screenWidth, screenHeight, "Across the Bullets");
    InitAudioDevice();
    InitGame();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        UpdateGame();
        DrawGame();
    }
    CloseAudioDevice();
    UnloadGame();  
    CloseWindow(); 


    return 0;
}

void InitGame(void){
    srand(time(NULL));
    Image imgstart = LoadImage("assets/home_8bit.jpeg");
    ImageResize(&imgstart, screenWidth, screenHeight);
    homescreen = LoadTextureFromImage(imgstart);
    UnloadImage(imgstart);

    game_start = false;

    //carrega as musicas
    homescreen_music = LoadMusicStream("assets/audio/music/acrosstheuniverse.mp3");
    game_music = LoadMusicStream("assets/audio/music/paint_it_black.mp3");

    PlayMusicStream(homescreen_music);

    //Carregar os sprites as suas respectivas funções
    background = LoadTexture("assets/background.png");
    personagem = LoadTexture("assets/Unarmed_Idle_full.png");
    bulletTex = LoadTexture("assets/bullet.png");
    vida = LoadTexture("assets/vida.png");

    //inicializa as variáveis do player
 
    player.width = 40;
    player.height = 40; 
    player.position = (Vector2){(screenWidth/2) - player.width/2, (screenHeight/2) - player.height/2};
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
    if (game_start == false){
        ClearBackground(RAYWHITE);
        DrawTexture(homescreen, 0, 0 , WHITE);

        // TEXTO PÁGINA INCIAL
        const char* linha1 = "Pressione [ENTER]";
        const char* linha2 = "para começar!";

        int tamanho_tex = 30;

        int textWidth1 = MeasureText(linha1, tamanho_tex);
        int textWidth2 = MeasureText(linha2, tamanho_tex);

        int x1 = (screenWidth - textWidth1) / 2;
        int x2 = (screenWidth - textWidth2) / 2;
        
        int posicaoYbase = screenHeight - 300;
        int espacamento = tamanho_tex + 5;

        // PISCAR TEXTO
        frameCount++;

        if ((frameCount / 45) % 2 == 0){ // quanto menor for o dividendo => mais rápido o texto
            DrawText(linha1, x1, posicaoYbase, tamanho_tex, BLACK);
            DrawText(linha2, x2, posicaoYbase + espacamento, tamanho_tex, BLACK);
        }


    } else{

        //Desenha o background
        int backgroundWidth = background.width;
        int backgroundHeight = background.height;
        for (int y = 0; y < screenHeight; y += backgroundHeight){
            for (int x = 0; x < screenWidth; x += backgroundWidth){
                DrawTexture(background, x, y, WHITE);
            }
        }


        //Desenha a vida
        int coracaoX = 10;
        int tamanho_coracao = 30;
        for (int i = 1 ; i <= player.vida; i++){
            DrawTexture(vida, coracaoX, 10, WHITE);
            coracaoX += tamanho_coracao + 10;
        }



        //Desenhar boneco
        DrawRectangle(player.position.x, player.position.y, player.width, player.height, RED);

        //Desenha o tempo
        int minutos = (int)tempo_jogado / 60;
        int segundos = (int)tempo_jogado % 60;
        DrawText(TextFormat("%02d:%02d", minutos, segundos), screenWidth - screenWidth/10 , 10, 30 , BLACK);

        // DrawText(TextFormat("%0.02f", (float)tempo_jogado/60), screenWidth - screenWidth/18 , 10, 30 , BLACK);
        

        Bullet *b = bullet;
        while (b !=NULL){
            DrawRectangleV(b->position, (Vector2){bullet_size, bullet_size}, ORANGE);
            b = b->next;
        }
        
        if (gameOver) {
        const char* texto = "GAME OVER";
        int tamanho_tex = 60;
        int textWidth_texto = MeasureText(texto, tamanho_tex);
        int x = (screenWidth - textWidth_texto) / 2;
        int y = screenHeight / 2 - tamanho_tex / 2;
        DrawText(texto, x, y, tamanho_tex, RED);
        }
    }

    EndDrawing();
}

void UpdateGame(void){



    if (game_start ==false){
        UpdateMusicStream(homescreen_music);
        if (IsKeyPressed(KEY_ENTER)){
            game_start= true;
            StopMusicStream(homescreen_music);
            PlayMusicStream(game_music);
            SetMusicVolume(game_music, 0.2f);
        }
        return;
    }

    UpdateMusicStream(game_music);
    //inicialização do tempo para aumentar dificuldade
    if (!gameOver){
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
        Rectangle playerRec = {player.position.x, player.position.y, player.width, player.height};

        while (bullet_atual != NULL){
            bullet_atual->position.x += bullet_atual->direction.x * bullet_atual->speed * deltaTime; //determina a movimentação x da bala
            bullet_atual->position.y += bullet_atual->direction.y * bullet_atual->speed * deltaTime; // determina a movimentação y da bala

            
            Rectangle bulletRec = {bullet_atual->position.x , bullet_atual->position.y , bullet_size , bullet_size};
            if (CheckCollisionRecs(playerRec , bulletRec)){
                player.vida--;
                Bullet *bullet_morta = bullet_atual;
                if (bullet_anterior == NULL){
                    bullet = bullet_atual->next;
                } else{
                    bullet_anterior->next = bullet_atual->next;
                }
                bullet_atual = bullet_atual->next;
                free(bullet_morta);
                if (player.vida <= 0){
                    gameOver = true;
                }
                continue;
            }

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

    } return;

   


}

void UnloadGame(void){
    UnloadTexture(background);
    UnloadTexture(personagem);
    UnloadTexture(homescreen);
    UnloadTexture(vida);
    UnloadMusicStream(homescreen_music);
    UnloadMusicStream(game_music);

    Bullet *bala = bullet;
    while (bala != NULL){
        Bullet *next = bala->next;
        free(bala);
        bala = next;
    }

    

}