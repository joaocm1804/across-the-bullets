#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#define MAX_CHAR_NOME 3


// STRUCTS --------------------------------------------------------
typedef struct User{
    char nome[4];
    int minuto;
    int segundo;
    struct User * next;
} User;

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
// -----------------------------------------------------------


// VARIAVEIS GLOBAIS ------------------------------------------
// Tela -------------------------------------------------------
static const int screenWidth = 1280;
static const int screenHeight = 720;
int frameCount = 0; // PISCAR
// ------------------------------------------------------------

// Inicializa texturas ----------------------------------------
static Texture2D background;
static Texture2D personagem;
static Texture2D bulletTex;
static Texture2D vida;
static Texture2D homescreen;
// ------------------------------------------------------------

// Inicializa musicas -----------------------------------------
static Music homescreen_music;
static Music game_music;
// ------------------------------------------------------------

// Tempo ------------------------------------------------------
static float tempo_jogado = 0.0f;
static float cronometro_last_spawn = 0.0f; //quanto tempo passou desde a ultima bala disparada
static float intervalo = 2.5f; //intervalo que as balas surgem
static float intervalo_minimo = 0.3f; //não pode disparar mais rapido que isso
static float qtd_diminuir_por_s = 0.1f; //rampa de dificulda (quanto maior, mais rapidamente fica dificil)
// ------------------------------------------------------------

//---Balas-----------------------------------------------------
static float bullet_speed = 300.0f;
static float bullet_speed_increase = 10.0f; //incremento de velocidade
static float bullet_size = 40;
// ------------------------------------------------------------

//---Booleanos-------------------------------------------------
static bool gameOver = false;
static bool pause = false;
static bool victory = false;
static bool game_start = false;
static bool pontuacao_salva = false;
static bool leaderboard = false;

//------------------------------------------------------------
static Player player = { 0 };
static struct Bullet *bullet = NULL;
User *ranking = NULL;
static char nome_player[MAX_CHAR_NOME + 1 ] = {0};
static int nome_len = 0;

//---FUNCOES PRINCIPAIS----------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
//-------------------------------------------------------------

//---FUNCOES AUXILIARES ---------------------------------------
void printarLeaderboard(void);
void add_inicio(User **ranking, char nome[], int min, int seg);
void ordenar(User **ranking);
void salvarRanking(User **ranking);

//-------------------------------------------------------------


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
    // MIDIAS -----------------------------------------------------------------------------------------
    // Imagens ----------------------------------------------------------------------------------------
    Image imgstart = LoadImage("assets/home_8bit.jpeg"); 
    ImageResize(&imgstart, screenWidth, screenHeight);
    homescreen = LoadTextureFromImage(imgstart);
    UnloadImage(imgstart);
    background = LoadTexture("assets/background.png");
    personagem = LoadTexture("assets/Unarmed_Idle_full.png");
    bulletTex = LoadTexture("assets/bullet.png");
    vida = LoadTexture("assets/vida.png");
    // -------------------------------------------------------------------------------------------------

    // Carrega as musicas -----------------------------------------------------------------------------
    homescreen_music = LoadMusicStream("assets/audio/music/acrosstheuniverse.mp3");
    game_music = LoadMusicStream("assets/audio/music/paint_it_black.mp3");
    PlayMusicStream(homescreen_music); // inicializa musica da home
    // -------------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------------

    // PLAYER ------------------------------------------------------------------------------------------
    // Variaveis do Player -----------------------------------------------------------------------------
    player.width = 40;
    player.height = 40; 
    player.position = (Vector2){(screenWidth/2) - player.width/2, (screenHeight/2) - player.height/2};
    player.vida = 3;
    player.speed = 7;
    player.backpack.madeira = 0;
    player.backpack.pedra = 0;
    // -------------------------------------------------------------------------------------------------

    // BALAS -------------------------------------------------------------------------------------------
    srand(time(NULL)); // balas aleatorias
    struct Bullet *b = (struct Bullet*)malloc(sizeof(Bullet));
    // Direcionamento das balas -----------------------------------------------------------------------
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
    // -------------------------------------------------------------------------------------------------
}

void DrawGame(void){
    BeginDrawing();
    // RANKING -----------------------------------------------------------------------------------------
    if (leaderboard == true){
        ClearBackground(RAYWHITE);
        printarLeaderboard();
    // -------------------------------------------------------------------------------------------------   

    // HOME --------------------------------------------------------------------------------------------
    }else if (game_start == false){
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

        frameCount++; // piscar o texto

        if ((frameCount / 45) % 2 == 0){ // quanto menor for o dividendo => mais rápido o texto
            DrawText(linha1, x1, posicaoYbase, tamanho_tex, BLACK);
            DrawText(linha2, x2, posicaoYbase + espacamento, tamanho_tex, BLACK);
        }
    // -------------------------------------------------------------------------------------------------     
    
    // GAME --------------------------------------------------------------------------------------------
    } else{

        //Desenha o background
        int backgroundWidth = background.width;
        int backgroundHeight = background.height;
        for (int y = 0; y < screenHeight; y += backgroundHeight){
            for (int x = 0; x < screenWidth; x += backgroundWidth){
                DrawTexture(background, x, y, WHITE);
            }
        }

        Bullet *b = bullet;
        while (b !=NULL){
            DrawRectangleV(b->position, (Vector2){bullet_size, bullet_size}, ORANGE);
            b = b->next;
        }

        DrawRectangle(player.position.x, player.position.y, player.width, player.height, RED);

        //Desenha a vida
        int coracaoX = 10;
        int tamanho_coracao = 30;
        for (int i = 1 ; i <= player.vida; i++){
            DrawTexture(vida, coracaoX, 10, WHITE);
            coracaoX += tamanho_coracao + 10;
        }

        //Desenha o tempo
        int minutos = (int)tempo_jogado / 60;
        int segundos = (int)tempo_jogado % 60;
        DrawText(TextFormat("%02d:%02d", minutos, segundos), screenWidth - screenWidth/10 , 10, 30 , BLACK);

        // DrawText(TextFormat("%0.02f", (float)tempo_jogado/60), screenWidth - screenWidth/18 , 10, 30 , BLACK);
        
        if (gameOver) {
           const char* go = "GAME OVER";
            int size = 60;
            int w = MeasureText(go, size);
            int x = (screenWidth - w)/2;
            int y = screenHeight/2 - size/2;
            DrawText(go, x, y, size, RED);

            // b) Se ainda não salvou, peça o nome
            if (!pontuacao_salva) {
                DrawText("Digite seu nome:", x, y + 80, 20, BLACK);
                DrawText(nome_player, x, y + 110, 30, BLACK);
            }
            // c) Se já salvou, oriente para ENTER voltar
            else {
                DrawText("Salvo! Pressione [ENTER] para voltar ao menu", 
                         x - 50, y + 80, 20, DARKGREEN);
            }
        }
    }
    EndDrawing();
    // -------------------------------------------------------------------------------------------------   
}

void reiniciar(void){
    Bullet *bala = bullet;
    while (bala != NULL){
        Bullet *next = bala->next;
        free(bala);
        bala = next;
    }
    bullet = NULL;

    tempo_jogado = 0.0f;
    cronometro_last_spawn = 0.0f;
    intervalo = 2.5f;
    player.vida = 3;
    nome_len = 0;
    nome_player[0] = '\0';
    gameOver=false;
    game_start=false;
    pontuacao_salva = false;

    StopMusicStream(game_music);
    PlayMusicStream(homescreen_music);

}

void UpdateGame(void){
    if (game_start ==false){
        UpdateMusicStream(homescreen_music);
        if (IsKeyPressed(KEY_L)){
            leaderboard = true;
        }
        else if (IsKeyPressed(KEY_ENTER)){
            if (leaderboard ==true){
                leaderboard = false;
            }else{
            game_start= true;
            StopMusicStream(homescreen_music);
            PlayMusicStream(game_music);
            SetMusicVolume(game_music, 0.1f);
            }
        }
        return;
    }

    UpdateMusicStream(game_music);

    if (gameOver == true){
        int tecla = GetKeyPressed();
        if (tecla>0 && pontuacao_salva == false){
            if (tecla == KEY_BACKSPACE){
                if (nome_len > 0){
                    nome_len--;
                    nome_player[nome_len] = '\0';
                }
            }
            else if (tecla == KEY_ENTER && nome_len == MAX_CHAR_NOME){
                int minutos = (int)tempo_jogado/60;
                int segundos = (int)tempo_jogado%60;
                add_inicio(&ranking, nome_player, minutos, segundos);
                ordenar(&ranking);
                salvarRanking(&ranking);
                pontuacao_salva = true;
            } else if(nome_len < MAX_CHAR_NOME && tecla <256){
                if (isalpha(tecla)){
                    nome_player[nome_len] = (char)toupper(tecla);
                    nome_len++;
                    nome_player[nome_len] = '\0';

                }
            }
        }

        if (pontuacao_salva == true && IsKeyPressed(KEY_ENTER)){
            reiniciar();
        }
        return;
    }





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

//---RANKING FUNCOES-------------------------------------------------------------------------------------------
void add_inicio(User **head, char nome[], int minuto, int segundo){
    User *new = (User*)malloc(sizeof(User));
    strcpy(new->nome, nome);
    new->minuto = minuto;
    new->segundo = segundo;
    new->next = *head;
    *head = new;

}

void ordenar(User **head){
    if (*head == NULL || (*head)->next == NULL){
        return;
    }
    int trocou = 1;
    while (trocou){
        User *aux = *head;
        trocou = 0;
        while (aux->next !=NULL){
            if (aux->minuto < aux->next->minuto || (aux->minuto == aux->next->minuto && aux->segundo < aux->next->segundo)){
                int temp;
                temp = aux->minuto;
                aux->minuto = aux->next->minuto;
                aux->next->minuto = temp;

                temp = aux->segundo;
                aux->segundo = aux->next->segundo;
                aux->next->segundo = temp;

                char tmp[4];
                strcpy(tmp, aux->nome);
                strcpy(aux->nome, aux->next->nome);
                strcpy(aux->next->nome, tmp);
                trocou = 1;
            
            }
            aux = aux->next;
        }
    }
}

void salvarRanking(User **head){
    FILE *ranking = fopen("ranking.txt", "w");
    if (ranking == NULL){
        perror("não conseguiu abrir o txt");
        return;
    }
    User *aux = *head;
    while (aux != NULL){
        fprintf(ranking, "%s %02d:%02d\n", aux->nome, aux->minuto, aux->segundo);
        aux = aux->next;
    } 
    fclose(ranking);
}

void printarLeaderboard(void){
    static int frameCount = 0;
    frameCount++; 

    FILE *leaderboard = fopen("ranking.txt", "r");
    if (leaderboard == NULL){
        DrawText("Ranking vazio :(", screenWidth/2 - 100, screenHeight/2, 25, RED);
        return;
    }

    int linha = 0;
    int espacamento = 30;

    DrawText("RANKING", screenWidth / 2 - 100, 100, 40, DARKGRAY);

    DrawText("POS", screenWidth / 2 - 200, 160, 20, GRAY);
    DrawText("NOME", screenWidth / 2 - 100, 160, 20, GRAY);
    DrawText("TEMPO", screenWidth / 2 + 50, 160, 20, GRAY);

    char nome[4];
    int min, seg;
    int pos = 1;

    while (fscanf(leaderboard, "%3s %d:%d", nome, &min, &seg) == 3 && pos <= 10){
        char tempo[10];
        sprintf(tempo, "%02d:%02d", min, seg);

        int y = 160 + espacamento * pos;

        DrawText(TextFormat("%2d.", pos), screenWidth / 2 - 200, y, 20, BLACK);
        DrawText(nome, screenWidth / 2 - 100, y, 20, BLACK);
        DrawText(tempo, screenWidth / 2 + 50, y, 20, BLACK);

        pos++;
    }

    fclose(leaderboard);
        
    const char *mensagem = "Pressione [ENTER] para voltar ao menu";
    int fontSize = 20;
    int larguraTexto = MeasureText(mensagem, fontSize);

    if ((frameCount / 30) % 2 == 0) {
        DrawText(mensagem, screenWidth / 2 - larguraTexto / 2, screenHeight - 100, fontSize, DARKGREEN);
    }
}
//--------------------------------------------------------------------------------------------------------