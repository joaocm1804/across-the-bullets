#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#define MAX_CHAR_NOME 3
#define ESCALA_HITBOX 0.65f
#define MAX_BARREIRA 5
#define BARREIRA_TAMANHO 95
#define VIDA_TAMANHO 55
#define TAMANHO_MADEIRA 30



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
    Inventario backpack;
    int vida;
    int speed;
    int width;
    int height;
    bool colisao;

    Texture2D player_right[6];
    Texture2D player_left[6];
    Texture2D player_up[4];
    Texture2D player_down[6];
    int frame_atual;
    int direction;
    float time_anime;
} Player;


typedef struct Bullet{
    float speed;
    Vector2 position;
    Vector2 direction;
    struct Bullet * next;
    Texture2D texture;
} Bullet;


typedef struct ExtraLife {
    Vector2 position;
    bool ativo;
    float tempo_dps_respawn;
    float tempo_ativo;
} ExtraLife;


typedef struct Barreira {
    Vector2 position;
    bool ativa;
    float tempo_de_vida;
} Barreira;


// -----------------------------------------------------------


// VARIAVEIS GLOBAIS ------------------------------------------
// Tela -------------------------------------------------------
static const int screenWidth = 1280;
static const int screenHeight = 720;
int frameCount = 0; // PISCAR
// ------------------------------------------------------------

// Inicializa texturas ----------------------------------------
static Texture2D background;
static Texture2D vida;
static Texture2D homescreen;
static Texture2D leaderboard_screen;
static Texture2D bulletTexNorte, bulletTexSul, bulletTexLeste, bulletTexOeste;
static Texture2D imgmadeira;
static Texture2D imglifeextra;
static Texture2D qtdmadeira;
static Texture2D tela_instrucoes;
// ------------------------------------------------------------

// Inicializa musicas -----------------------------------------
static Music homescreen_music;
static Music game_music;
static Music gameover_music;
static Sound sound_atingiu;
static Sound sound_gameover;
static Sound vida_sound;
static Sound wall_construiu;
static Sound wall_quebrou;
// ------------------------------------------------------------

// Tempo ------------------------------------------------------
static float tempo_jogado = 0.0f;
static float cronometro_last_spawn = 0.0f; //quanto tempo passou desde a ultima bala disparada
static float intervalo = 2.5f; //intervalo que as balas surgem
static float intervalo_minimo = 0.30f; //não pode disparar mais rapido que isso
static float qtd_diminuir_por_s = 0.20f; //rampa de dificulda (quanto maior, mais rapidamente fica dificil)

// ------------------------------------------------------------

//---Balas-----------------------------------------------------
static float bullet_speed = 375.0f;
static float bullet_speed_increase = 2.5f; //incremento de velocidade
static float bullet_size = 40;
// ------------------------------------------------------------

//---Booleanos-------------------------------------------------
static bool gameOver = false;
static bool game_start = false;
static bool pontuacao_salva = false;
static bool leaderboard = false;
static bool instrucoes = false;
static bool gameover_music_init = false;
static bool gameover_sound_init = false;
static bool vida_sound_init = false;
static bool wall_construiu_init = false;
static bool wall_quebrou_init = false;
//------------------------------------------------------------
static Player player = { 0 };
static struct Bullet *bullet = NULL;
User *ranking = NULL;
static char nome_player[MAX_CHAR_NOME + 1 ] = {0};
static int nome_len = 0;
double colisaoTime = -10; 


ExtraLife extralife = {0};
const float tempo_de_respawn = 10.0f;
const int extralifeTamanho  = 55;

Barreira barreira[MAX_BARREIRA];

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
void carregarRanking(User **ranking);


//-------------------------------------------------------------


int main(void)
{
   
    InitWindow(screenWidth, screenHeight, "Across the Bullets");    // Inicializa a janela tema
    InitAudioDevice();                                              // Inicializa o sistema de audios
    InitGame();                                                     // Inicializa os elementos do jogo
    SetTargetFPS(60);                                               // Definição de uma taxa mínima de FPS.


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
    Image imgstart = LoadImage("assets/home-atheb.jpg"); 
    ImageResize(&imgstart, screenWidth, screenHeight);
    homescreen = LoadTextureFromImage(imgstart);
    UnloadImage(imgstart);

    Image img_instruct = LoadImage("assets/tela-instrucoes.jpg"); 
    ImageResize(&img_instruct, screenWidth, screenHeight);
    tela_instrucoes = LoadTextureFromImage(img_instruct);
    UnloadImage(img_instruct);

    //--------------------------------------------------------------------------------------------------
    background = LoadTexture("assets/background.png");
    vida = LoadTexture("assets/vida.png");

    Image img;

    img = LoadImage("assets/bullet_norte.png");
    ImageResize(&img, 60, 60);             
    bulletTexNorte = LoadTextureFromImage(img);
    UnloadImage(img);                                         

    img = LoadImage("assets/bullet_sul.png");
    ImageResize(&img, 60, 60);
    bulletTexSul = LoadTextureFromImage(img);
    UnloadImage(img);

    img = LoadImage("assets/bullet_leste.png");
    ImageResize(&img, 60, 60);                              
    bulletTexLeste = LoadTextureFromImage(img);
    UnloadImage(img);

    img = LoadImage("assets/bullet_oeste.png");
    ImageResize(&img, 60, 60);
    bulletTexOeste = LoadTextureFromImage(img);
    UnloadImage(img);
    //--------------------------------------------------------------------------------------------------
    Image imgleaderboard = LoadImage("assets/leaderboard_screen.png");
    ImageResize(&imgleaderboard, screenWidth, screenHeight);
    leaderboard_screen = LoadTextureFromImage(imgleaderboard);
    UnloadImage(imgleaderboard);


    Image imgwood = LoadImage("assets/wood_barreira.png");
    ImageResize(&imgwood, BARREIRA_TAMANHO, BARREIRA_TAMANHO);
    imgmadeira = LoadTextureFromImage(imgwood);
    UnloadImage(imgwood);

    img = LoadImage("assets/healer.png");
    ImageResize(&img , VIDA_TAMANHO , VIDA_TAMANHO);
    imglifeextra = LoadTextureFromImage(img);
    UnloadImage(img);

    Image imgshield = LoadImage("assets/wood_shield.png");
    ImageResize(&imgshield, TAMANHO_MADEIRA, TAMANHO_MADEIRA); // Redimensiona para um tamanho adequado
    qtdmadeira = LoadTextureFromImage(imgshield);
    UnloadImage(imgshield);

    // -------------------------------------------------------------------------------------------------

    // Carrega as musicas -----------------------------------------------------------------------------
    homescreen_music = LoadMusicStream("assets/audio/music/acrosstheuniverse.mp3");
    game_music = LoadMusicStream("assets/audio/music/paint_it_black.mp3");
    gameover_music = LoadMusicStream("assets/audio/music/iylmn.mp3");
    PlayMusicStream(homescreen_music); // inicializa musica da home
    sound_atingiu = LoadSound("assets/audio/effects/explosion.wav");
    sound_gameover = LoadSound("assets/audio/effects/synth.wav");
    vida_sound = LoadSound("assets/audio/effects/powerUp.wav");
    wall_construiu = LoadSound("assets/audio/effects/wall_construiu.wav");
    wall_quebrou = LoadSound("assets/audio/effects/wall_quebrou.wav");
    SetSoundVolume(sound_gameover, 0.4f);
    SetSoundVolume(sound_atingiu, 0.5f);
    SetSoundVolume(vida_sound, 0.4f);
    SetSoundVolume(wall_construiu, 0.4f);
    SetSoundVolume(wall_quebrou, 0.4f);


    // -------------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------------

    // PLAYER ------------------------------------------------------------------------------------------
    // Variaveis do Player -----------------------------------------------------------------------------
    player.width = 60;
    player.height = 60; 
    player.position = (Vector2){(screenWidth/2) - player.width/2, (screenHeight/2) - player.height/2};
    player.vida = 3;
    player.speed = 7;
    player.backpack.madeira = 5;
    // -------------------------------------------------------------------------------------------------
    for (int i =0; i < 6; i++){
        char caminho[44];
        sprintf(caminho, "assets/player/right/playerdireita_%02d.png", i+1);
        img = LoadImage(caminho);
        ImageResize(&img, player.width+5, player.height+5);
        player.player_right[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    for (int i =0; i < 6; i++){
        char caminho[45];
        sprintf(caminho, "assets/player/left/player-esquerda_%02d.png", i+1);
        img = LoadImage(caminho);
        ImageResize(&img, player.width+5, player.height+5);
        player.player_left[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    for (int i =0; i < 4; i++){
        char caminho[39];
        sprintf(caminho, "assets/player/up/player-cima_%02d.png", i+1);
        img = LoadImage(caminho);
        ImageResize(&img, player.width+5, player.height+5);
        player.player_up[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    for (int i =0; i < 6; i++){
        char caminho[42];
        sprintf(caminho, "assets/player/down/player-baixo_%02d.png", i+1);
        img = LoadImage(caminho);
        ImageResize(&img, player.width+5, player.height+5);
        player.player_down[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }

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
            b->texture = bulletTexNorte;
        } else if (direction == 1){//sul
            b->direction.x = 0;
            b->direction.y = -1;
            b->position.y= screenHeight;
            b->position.x = GetRandomValue(0, screenWidth - bullet_size);
            b->texture = bulletTexSul;
        } else if (direction == 2){//leste
            b->direction.x = -1;
            b->direction.y = 0;
            b->position.x= screenWidth;
            b->position.y = GetRandomValue(0, screenHeight - bullet_size);
            b->texture = bulletTexLeste;
        } else if (direction == 3){//oeste
            b->direction.x = 1;
            b->direction.y = 0;
            b->position.x= -bullet_size;
            b->position.y = GetRandomValue(0, screenHeight - bullet_size);
            b->texture = bulletTexOeste;
        }
        b->next = bullet;
        bullet = b;
    }    
    // -------------------------------------------------------------------------------------------------
    for (int i = 0; i < MAX_BARREIRA; i++) {
        barreira[i].ativa = false;
        barreira[i].tempo_de_vida = 0.0f;
        barreira[i].position = (Vector2){0, 0};
    }   
}


void DrawGame(void){
    BeginDrawing();
    // RANKING -----------------------------------------------------------------------------------------
    if (leaderboard == true){
        ClearBackground(RAYWHITE);
        DrawTexture(leaderboard_screen, 0, 0, WHITE);
        printarLeaderboard();
        
    // -------------------------------------------------------------------------------------------------   

    }else if (instrucoes == true){
        static int frameCount = 0;
        ClearBackground(RAYWHITE);
        DrawTexture(tela_instrucoes, 0, 0, WHITE);
        const char *mensagem2 = "Pressione [ENTER] para voltar ao menu";
        int fontSize2 = 20;
        int LarguraTexto2 = MeasureText(mensagem2, fontSize2);
        frameCount++;
        if ((frameCount / 30) % 2 == 0){
            DrawText(mensagem2, screenWidth / 2 - LarguraTexto2 / 2, screenHeight - 100, fontSize2, WHITE);
        }
    // HOME --------------------------------------------------------------------------------------------
    }else if (game_start == false){
        ClearBackground(RAYWHITE);
        DrawTexture(homescreen, 0, 0 , WHITE);

        // TEXTO PÁGINA INCIAL
        const char* linha1 = "Pressione [ENTER]";
        const char* linha2 = "para começar!";


        // DEFINICÃO DE CARACTERES DO TEXTO INICIAL
        int tamanho_tex = 30;
        int textWidth1 = MeasureText(linha1, tamanho_tex);
        int textWidth2 = MeasureText(linha2, tamanho_tex);

        //DEFINIÇÃO DAS POSIÇÕES NA TELA DO TEXTO INICIAL
        int x1 = (screenWidth - textWidth1) / 2;
        int x2 = (screenWidth - textWidth2) / 2;
        
        int posicaoYbase = screenHeight - 300;
        int espacamento = tamanho_tex + 5;

        frameCount++; // piscar o texto

        if ((frameCount / 45) % 2 == 0){ // quanto menor for o dividendo => mais rápido o texto
            DrawText(linha1, x1, posicaoYbase, tamanho_tex, BLACK);
            DrawText(linha2, x2, posicaoYbase + espacamento, tamanho_tex, BLACK);
        }
        const int margem = 20;
        const int tamanho_tex_2 = 20;
        const char *cmd1 = "[L] - Leaderboard";
        const char *cmd2 = "[G] - Como Jogar"; 

        int textwidth3 = MeasureText(cmd1, tamanho_tex_2);
        int textwitdh4 = MeasureText(cmd2, tamanho_tex_2);

        int x3 = screenWidth - margem - textwidth3; // canto direito
        int x4 = screenWidth - margem - textwitdh4;

        int y2 = screenHeight - margem - tamanho_tex_2;  // linha de baixo
        int y1 = y2 - tamanho_tex_2 - 5;                 // linha de cima

        DrawText(cmd1, x3, y1, tamanho_tex_2, BLACK);
        DrawText(cmd2, x4, y2, tamanho_tex_2, BLACK);
    // -------------------------------------------------------------------------------------------------     
    
    // GAME --------------------------------------------------------------------------------------------
    } else{

        // DESENHA O BACKGROUND
        int backgroundWidth = background.width;
        int backgroundHeight = background.height;
        for (int y = 0; y < screenHeight; y += backgroundHeight){
            for (int x = 0; x < screenWidth; x += backgroundWidth){
                DrawTexture(background, x, y, WHITE);
            }
        }

       if (!gameOver) {
            double tempoAtual = GetTime();

            bool deveDesenhar = true;

            if (tempoAtual - colisaoTime < 0.8) { // pisca por 0.8s após colisão
                if (((int)(tempoAtual * 8)) % 2 == 0) {
                    deveDesenhar = false; // neste frame, não desenha (dá efeito de piscar)
                }
            }

            if (deveDesenhar) {
                // ESCOLHE TEXTURA DE ACORDO COM A DIREÇÃO
                Texture2D tex;
                if (player.direction == 0){
                    tex = player.player_right[player.frame_atual];
                } else if (player.direction == 1){
                    tex = player.player_left[player.frame_atual];
                } else if (player.direction == 2){
                    tex = player.player_up[player.frame_atual];
                } else {
                    tex = player.player_down[player.frame_atual];
                }

                DrawTexture(tex, (int)player.position.x, (int)player.position.y, WHITE);
            }

            // DESENHA AS BALAS
            Bullet *b = bullet;
                while (b !=NULL){
                DrawTextureV(b->texture, b->position, RAYWHITE);

                // //codigo para verificar tamanho da hitbox
                // float neww = b->texture.width  * ESCALA_HITBOX;
                // float newh = b->texture.height * ESCALA_HITBOX;
                // // //variaveis pra centralizar a bala na hitbox
                // float centralizarX = (b->texture.width  - neww) * 0.5f;
                // float centralizarY = (b->texture.height - newh) * 0.5f;

                // Rectangle hitbox = {b->position.x + centralizarX, b->position.y + centralizarY, neww,newh};

                
                // DrawRectangleLinesEx(hitbox, 1, RED);//desenho para teste da hitbox
                b = b->next;
                }
                

            for (int i = 0; i < MAX_BARREIRA; i++) {
                if (barreira[i].ativa) {
                    DrawTextureV(imgmadeira, barreira[i].position, WHITE);    
                }
            }

            if (extralife.ativo){
                DrawTextureV(imglifeextra, extralife.position, WHITE);
            }

        }


     


        // DESENHA AS VIDAS
        int coracaoX = 10;
        int tamanho_coracao = 30;
        for (int i = 1 ; i <= player.vida; i++){
            DrawTexture(vida, coracaoX, 10, WHITE);
            coracaoX += tamanho_coracao + 10;
        }


        int tamanho_madeira = 30;
        int espacamento = 10;
        int madeiraX = 10;
        int madeiraY = 50;

        for (int i = 0; i < player.backpack.madeira; i++) {
            DrawTexture(qtdmadeira, madeiraX, madeiraY, WHITE);
            madeiraX += tamanho_madeira + espacamento;
        }
        // DESENHA O TEMPO DE JOGO
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

// FUNÇÃO PARA REINICIAR O JOGO
void reiniciar(void){
    Bullet *bala = bullet;
    while (bala != NULL){
        Bullet *next = bala->next;
        free(bala);
        bala = next;
    }
    bullet = NULL;
    
    //Redefinição de variaveis para o valor padrão de jogo
    tempo_jogado = 0.0f;
    cronometro_last_spawn = 0.0f;
    intervalo = 2.5f;
    player.vida = 3;
    player.backpack.madeira = 5;
    nome_len = 0;
    nome_player[0] = '\0';
    gameOver=false;
    game_start=false;
    pontuacao_salva = false;
    player.position.x = screenWidth/2 - 20; 
    player.position.y = screenHeight/2 - 20;
    extralife.ativo = false;
    gameover_music_init = false;
    gameover_sound_init = false;
    extralife.tempo_dps_respawn = 0.0f;
    for (int i = 0; i <MAX_BARREIRA;i++){
        barreira[i].ativa = false;
    }


    StopMusicStream(gameover_music);
    PlayMusicStream(homescreen_music);

    

}

// CONTROLE DE ESTADO ATUAL DO JOGO
void UpdateGame(void){
    // Enquanto jogo não for inicializado, musica inicial continuará tocando
    if (game_start ==false){
        UpdateMusicStream(homescreen_music);
        if (IsKeyPressed(KEY_L)){
            leaderboard = true;
            instrucoes = false;
        }if (IsKeyPressed(KEY_G)){
            instrucoes = true;
            leaderboard = false;
        }
        if (IsKeyPressed(KEY_ENTER)){
            if (leaderboard ==true){
                leaderboard = false;
            } else if(instrucoes == true){
                instrucoes = false;
            }
            else{
            game_start= true;
            StopMusicStream(homescreen_music);
            PlayMusicStream(game_music);
            SetMusicVolume(game_music, 0.5f);
            }
        }
        return;
    }

    UpdateMusicStream(game_music);
    // Algoritimo para definição do ranking de players
    if (gameOver == true){
        StopMusicStream(game_music);

        if (gameover_music_init == false){
            PlaySound(sound_gameover);
            PlayMusicStream(gameover_music);
            SetMusicVolume(gameover_music, 0.7f);
            gameover_music_init = true;

        }

        UpdateMusicStream(gameover_music);
        int tecla = GetKeyPressed();
        if (tecla>0 && pontuacao_salva == false){                      
            if (tecla == KEY_BACKSPACE){                                // Condição para remoção de caracteres pelo backspace 
                if (nome_len > 0){                                  
                    nome_len--;
                    nome_player[nome_len] = '\0';
                }
            }
            else if (tecla == KEY_ENTER && nome_len == MAX_CHAR_NOME){  // Condição para prosseguir com o registro do ranking 
                int minutos = (int)tempo_jogado/60;
                int segundos = (int)tempo_jogado%60;
                carregarRanking(&ranking);   
                add_inicio(&ranking, nome_player, minutos, segundos);   // Função que insere o nome e o tempo de jogo no inicio do ranking.
                ordenar(&ranking);                                      // Função que ordena os dados inseridos no ranking
                salvarRanking(&ranking);                                // Função que salva o ranking formatado após a ordenação
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


    if (IsKeyPressed(KEY_SPACE) && player.backpack.madeira > 0){
        for (int i = 0; i < MAX_BARREIRA; i++) {
            if (!barreira[i].ativa) {
                wall_construiu_init = true;
                if (wall_construiu_init){
                    PlaySound(wall_construiu);
                    wall_construiu_init = false;
                }
                barreira[i].ativa = true;
                barreira[i].position = (Vector2){
                    player.position.x + player.width / 2 - BARREIRA_TAMANHO / 2,
                    player.position.y + player.height / 2 - BARREIRA_TAMANHO / 2
                };
                barreira[i].tempo_de_vida = 10.0f;
                player.backpack.madeira--;
                break;
            }
        }
    }



    //inicialização do tempo para aumentar dificuldade
    float deltaTime = GetFrameTime();
    tempo_jogado += deltaTime;
    cronometro_last_spawn += deltaTime; 
    
    
    //  movimentacao do jogador inclusive na diagonal
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)){
        player.position.x += player.speed;
        player.direction = 0;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)){
        player.position.x -= player.speed;
        player.direction = 1;
    }
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)){
        player.position.y -= player.speed;
        player.direction = 2;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)){
        player.position.y += player.speed;
        player.direction = 3;
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

    player.time_anime +=GetFrameTime();
    if (player.time_anime >= 0.1f){
        player.time_anime = 0;
        player.frame_atual++;
        int frame_max;
        if (player.direction == 0){
            frame_max = 6;
        } else if (player.direction == 1){
            frame_max = 6;
        } else if (player.direction == 2){
            frame_max = 4;
        } else{
            frame_max = 4;
        }
        if (player.frame_atual >= frame_max){
            player.frame_atual = 0;
        }
    }
    Bullet *bullet_atual = bullet; //ponteiro para a principal
    Bullet *bullet_anterior = NULL; //ponteiro para a bala anterior
    Rectangle playerRec = {player.position.x, player.position.y, player.width, player.height};

    while (bullet_atual != NULL){
        bullet_atual->position.x += bullet_atual->direction.x * bullet_atual->speed * deltaTime; //determina a movimentação x da bala
        bullet_atual->position.y += bullet_atual->direction.y * bullet_atual->speed * deltaTime; // determina a movimentação y da bala

        float neww = bullet_atual->texture.width  * ESCALA_HITBOX;
        float newh = bullet_atual->texture.height * ESCALA_HITBOX;

        float centralizarX = (bullet_atual->texture.width  - neww) / 2.0f;
        float centralizarY = (bullet_atual->texture.height - newh) / 2.0f;
        
        Rectangle bulletRec = {bullet_atual->position.x +centralizarX, bullet_atual->position.y +centralizarY, neww , newh};

        bool bloqueada = false;

        for (int i = 0; i < MAX_BARREIRA; i++) {
            if (barreira[i].ativa) {
                Rectangle barreiraRec = {barreira[i].position.x, barreira[i].position.y, BARREIRA_TAMANHO, BARREIRA_TAMANHO};

                if (CheckCollisionRecs(barreiraRec, bulletRec)) {
                    wall_quebrou_init = true;
                    if (wall_quebrou_init){
                        PlaySound(wall_quebrou);
                        wall_quebrou_init = false;
                    }
                    // Bala atinge barreira -> barreira some, bala também
                    barreira[i].ativa = false;
                    bloqueada = true;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_BARREIRA; i++) {
            if (barreira[i].ativa) {
                barreira[i].tempo_de_vida -= deltaTime;
                if (barreira[i].tempo_de_vida <= 0) {
                    wall_quebrou_init = true;
                    if (wall_quebrou_init){
                        PlaySound(wall_quebrou);
                        wall_quebrou_init = false;
                    }
                    barreira[i].ativa = false;
                }
            }
        }


        if (bloqueada) {
            Bullet *bullet_morta = bullet_atual;
            if (bullet_anterior == NULL){
                bullet = bullet_atual->next;
            } else{
                bullet_anterior->next = bullet_atual->next;
            }
            bullet_atual = bullet_atual->next;
            free(bullet_morta);
            continue;
        }

        // COLISÕES ENTRE PLAYER E BALAS
        if (CheckCollisionRecs(playerRec , bulletRec)){         // Condição para checagem de colisões entre as hitboxes.
            PlaySound(sound_atingiu);
            player.vida--;                                      // Para cada checagem verdadeira, a vida do player diminue
            colisaoTime = GetTime();                            // Tempo em que ocorre a colisao
            Bullet *bullet_morta = bullet_atual;                // Cria um ponteiro temporario para armazenar balas que colidiram com o player
            if (bullet_anterior == NULL){                       // Caso a bala seja a primeira da lista, o ponteiro será redirecionado para a próxima da lista
                bullet = bullet_atual->next;
            } else{                                             // Caso não seja a primeira, o ponteiro anterior irá apontar para a posição atual da lista
                bullet_anterior->next = bullet_atual->next;
            }
            bullet_atual = bullet_atual->next;                  // Avanca o loop
            free(bullet_morta);
            if (player.vida <= 0){                              // Caso as vidas do player acabem, o jogo finaliza.
                gameOver = true;
            }
            continue;
        }

        // Condição para respawn de balas
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
                new->texture = bulletTexNorte;
            } else if (direcao == 1){//sul
                new->direction.x = 0;
                new->direction.y = -1;
                new->position.y= screenHeight;
                new->position.x = GetRandomValue(0, screenWidth - bullet_size);
                new->texture = bulletTexSul;
            } else if (direcao == 2){//leste
                new->direction.x = -1;
                new->direction.y = 0;
                new->position.x= screenWidth;
                new->position.y = GetRandomValue(0, screenHeight - bullet_size);
                new->texture = bulletTexLeste;
            } else if (direcao == 3){//oeste
                new->direction.x = 1;
                new->direction.y = 0;
                new->position.x= -bullet_size;
                new->position.y = GetRandomValue(0, screenHeight - bullet_size);
                new->texture = bulletTexOeste;
            }
            new->next = bullet;
            bullet = new;
        }
    }

    extralife.tempo_dps_respawn += deltaTime;

    if (!extralife.ativo && extralife.tempo_dps_respawn >= tempo_de_respawn){
        extralife.position.x = GetRandomValue(0 , screenWidth - extralifeTamanho);
        extralife.position.y = GetRandomValue(0, screenHeight - extralifeTamanho);
        extralife.ativo = true;
        extralife.tempo_dps_respawn = 0.0f;
        extralife.tempo_ativo = 0.0f;
    }

    if (extralife.ativo){
        extralife.tempo_ativo += deltaTime;

        Rectangle vida_extra = { extralife.position.x , extralife.position.y , extralifeTamanho , extralifeTamanho};
        if (CheckCollisionRecs(playerRec, vida_extra)){
            vida_sound_init = true;
            if (vida_sound_init){
                PlaySound(vida_sound);
                vida_sound_init = false;
            }
            if (player.vida < 3){
                player.vida ++;
            }
            extralife.ativo = false;
        }

        if (extralife.tempo_ativo >= 15.0f){
            extralife.ativo = false;
        }

    }

}


void UnloadGame(void){
    UnloadTexture(background);
    UnloadTexture(homescreen);
    UnloadTexture(vida);
    UnloadTexture(qtdmadeira);
    UnloadTexture(imgmadeira);
    UnloadTexture(imglifeextra);
    UnloadTexture(bulletTexNorte);
    UnloadTexture(bulletTexSul);
    UnloadTexture(bulletTexLeste);
    UnloadTexture(bulletTexOeste);
    UnloadMusicStream(homescreen_music);
    UnloadMusicStream(game_music);
    UnloadMusicStream(gameover_music);
    UnloadSound(sound_atingiu);
    UnloadSound(sound_gameover);
    UnloadSound(vida_sound);
    UnloadSound(wall_quebrou);
    UnloadSound(wall_construiu);

    for (int i = 0; i < 6; i++)
    {
        UnloadTexture(player.player_right[i]);
        UnloadTexture(player.player_left[i]);
        UnloadTexture(player.player_down[i]);
    }
    for (int i = 0; i < 4; i++)
    {
        UnloadTexture(player.player_up[i]);
    }

    Bullet *bala = bullet;
    while (bala != NULL){
        Bullet *next = bala->next;
        free(bala);
        bala = next;
    }

}

//---RANKING FUNCOES-------------------------------------------------------------------------------------------

// Função adicionar scores no ranking   
void add_inicio(User **head, char nome[], int minuto, int segundo){
    User *new = (User*)malloc(sizeof(User));                            // Criação do nó na lista USER
    strcpy(new->nome, nome);                                            // Armazena o argumento nome da função
    new->minuto = minuto;                                               // Armazena os minutos do usuário
    new->segundo = segundo;                                             // Armazena os segundos do usuário
    new->next = *head;                                                  // Avança para o próximo nó
    *head = new;                                                        // o nó head agora é igual ao do new

}


// Função para ordenar os players do ranking pelo tempo de jogo
void ordenar(User **head){
    if (*head == NULL || (*head)->next == NULL){                        // Caso o ranking esteja vazio, a função encerra.
        return;
    }
    int trocou = 1;
    while (trocou){
        User *aux = *head;
        trocou = 0;

        // Bubblesort para posicionar os minutos de jogo por ordem descrescente
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


void limparRanking(User **head) {
    User *atual = *head;
    while (atual != NULL) {
        User *prox = atual->next;
        free(atual);
        atual = prox;
    }
    *head = NULL;
}



void carregarRanking(User **head){

    limparRanking(head);
    FILE *ranking = fopen("ranking.txt", "r");
    
    if (ranking == NULL) {
        return;
    }

    char nome[4];
    int minuto, segundo;

    while (fscanf(ranking, "%3s %d:%d", nome, &minuto, &segundo) == 3) {
        add_inicio(head, nome, minuto, segundo); 
    }

    fclose(ranking);
}
//--------------------------------------------------------------------------------------------------------