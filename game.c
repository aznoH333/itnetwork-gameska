/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
//------------------------------------------------------------------------------------
// * utility functions *
//------------------------------------------------------------------------------------
bool checkBoxCollisions(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2){
    return x1 + w1 > x2 &&
           x1 < x2 + w2 &&
           y1 + h1 > y2 &&
           y1 < y2 + h2;
}

float lerp(float a, float b, float w){
    return ((a - b) * w) + a;
}

int min(int a, int b){
    if (a < b){
        return a;
    }
    return b;
}


//-------------------------------------------------------------------
// * random vars *
//-------------------------------------------------------------------

const int screenWidth = 450;
const int screenHeight = 700;
const int windowWidth = 1280;
const int windowHeight = 720;
const float screenZoom = 2.0f;
const int inGameWidth = (int)(screenWidth / screenZoom);
const int inGameHeight = (int)(screenHeight / screenZoom);
int playerX = 0;
int playerY = 0;
int playerLevel = 1;
float backgroundSpeed = 0.0f;
int killedThisLife = 0;
const float MAX_BACKGROUND_SPEED = 1.5f;
int movedBackgrounds = 0;
int currentBackground = 0;



//------------------------------------------------------------------------------------
// * Sprite loading *
//------------------------------------------------------------------------------------



Texture2D lampir;
Texture2D samuel;
Texture2D player;
Texture2D playerLeft;
Texture2D playerRight;
Texture2D bullet;
Texture2D powSprite;
Texture2D mudBackground;
Texture2D sandBackground;
Texture2D grassBackground;

Sound shootPlayerSound;
Sound enemyHitSound;
Sound explosionSound;
Sound enemyShootSound;
Sound bonusSound;
Sound gameOverSound;
Music music;

int playerLives = 3;
#define EXPLOSION_COUNT 7
Texture2D explosionSprites[EXPLOSION_COUNT];
Texture2D enemyBullet;
RenderTexture2D renderTexture;

void loadSprites(){
    lampir = LoadTexture("sprites/lampir.png");
    samuel = LoadTexture("sprites/samuel.png");
    player = LoadTexture("sprites/player_0.png");
    playerLeft = LoadTexture("sprites/player_1.png");
    playerRight = LoadTexture("sprites/player_2.png");
    bullet = LoadTexture("sprites/bullet.png");
    powSprite = LoadTexture("sprites/pow.png");
    explosionSprites[0] = LoadTexture("sprites/sprite_3.png");
    explosionSprites[1] = LoadTexture("sprites/sprite_4.png");
    explosionSprites[2] = LoadTexture("sprites/sprite_5.png");
    explosionSprites[3] = LoadTexture("sprites/sprite_6.png");
    explosionSprites[4] = LoadTexture("sprites/sprite_0.png");
    explosionSprites[5] = LoadTexture("sprites/sprite_1.png");
    explosionSprites[6] = LoadTexture("sprites/sprite_2.png");
    mudBackground = LoadTexture("sprites/mudBack.png");
    sandBackground = LoadTexture("sprites/sandBack.png");
    grassBackground = LoadTexture("sprites/grassBack.png");
    enemyBullet = LoadTexture("sprites/enemy_bullet.png");
    
    // render texture
    renderTexture = LoadRenderTexture(screenWidth, screenHeight);
    
    // sounds
    shootPlayerSound = LoadSound("sounds/laser.wav");
    enemyHitSound = LoadSound("");
    explosionSound = LoadSound("sounds/explosion.wav");
    enemyShootSound = LoadSound("");
    bonusSound = LoadSound("sounds/bonus.wav");
    gameOverSound = LoadSound("sounds/gameOver.wav");
    music = LoadMusicStream("sounds/hudba.mp3");
}

void unloadSprites(){
    UnloadTexture(lampir);
    UnloadTexture(samuel);
    UnloadTexture(player);
    UnloadTexture(playerLeft);
    UnloadTexture(playerRight);
    UnloadTexture(bullet);
    UnloadTexture(powSprite);
    for (int i = 0; i < EXPLOSION_COUNT; i++){
        UnloadTexture(explosionSprites[i]);
    }
    UnloadTexture(mudBackground);
    UnloadTexture(grassBackground);
    UnloadTexture(sandBackground);
    UnloadTexture(enemyBullet);
    
    // sounds
    UnloadSound(shootPlayerSound);
    UnloadSound(enemyHitSound);
    UnloadSound(explosionSound);
    UnloadSound(enemyShootSound);
    UnloadSound(bonusSound);
    UnloadSound(gameOverSound);
    UnloadMusicStream(music);
    
    // render texture
    UnloadRenderTexture(renderTexture);
}



//-------------------------------------------------------------------
// * objects *
//-------------------------------------------------------------------
struct Object{
    int x;
    int y;
    int width;
    int height;
    int team;
    bool exists;
    int type;
    int health;
    int internalTimer;
    int variable1;
    int variable2;
    int variable3;
    int enemyType;
    int ai;
};

struct Object initDefaultObject(){
    struct Object obj;
    
    obj.x = 0;
    obj.y = 0;
    obj.width = 0;
    obj.height = 0;
    obj.team = 0;
    obj.exists = true;
    obj.type = 0;
    obj.health = 0;
    obj.internalTimer = 0;
    obj.variable1 = 0;
    obj.variable2 = 0;
    obj.variable3 = 0;
    obj.enemyType = 0;
    obj.ai = 0;
    
    return obj;
}

#define MAX_OBJECTS 250
struct Object objects[MAX_OBJECTS];
#define TEAM_PLAYER 0
#define TEAM_ENEMIES 1
#define TEAM_PARTICLE 2

#define TYPE_BULLET 0
#define ENEMY_SAMUEL 1
#define POW_PARTICLE 2
#define EXPLOSION_PARTICLE 3
#define ENEMY_LAMPIR 4

void updateObjects(){
    for (int i = 0; i < MAX_OBJECTS; i++){
        struct Object* obj = &objects[i];
        
        if (obj->exists){
            switch(obj->type){
                case TYPE_BULLET:
                    updateBullet(obj);
                    break;
                case ENEMY_SAMUEL:
                    updateSamuel(obj);
                    break;
                case POW_PARTICLE:
                    updatePow(obj);
                    break;
                case EXPLOSION_PARTICLE:
                    updateExplosion(obj);
                    break;
                
            }
            
            
            // collision
            for (int j = 0; j < MAX_OBJECTS; j++){
                struct Object* other = &objects[j];
                if (i != j && other->exists){
                    if (checkBoxCollisions(obj->x, obj->y, obj->width, obj->height, other->x, other->y, other->width, other->height)){
                        switch(obj->type){
                            case TYPE_BULLET:
                                bulletCollide(obj, other);
                                break;
                        }
                    }
                
                }
            }
            
        }
    }
}

int nextObjectIndex = 0;
void addObject(struct Object obj){
    int i = 0;
    while(objects[nextObjectIndex].exists || i > MAX_OBJECTS){
        nextObjectIndex++;
        nextObjectIndex %= MAX_OBJECTS;
        i++;
    }
    objects[nextObjectIndex] = obj;
    
}

//-------------------------------------------------------------------
// * explosion *
//-------------------------------------------------------------------
void updateExplosion(struct Object* this){
    this->internalTimer--;
    Texture2D* spr = &explosionSprites[(int)floor((21 - this->internalTimer) / 3)];
    this->y += backgroundSpeed / 4;

    if (this->internalTimer == 0){
        this->exists = false;
    }
    Vector2 v;
    v.x = this->x;
    v.y = this->y;
    DrawTextureEx(*spr, v, 0.0f, 0.2f, WHITE);
    
}

struct Object initExplosion(int x, int y){
    struct Object out = initDefaultObject();
    PlaySound(explosionSound);
    out.x = x;
    out.y = y;
    
    out.team = TEAM_PARTICLE;
    out.internalTimer = 21;
    out.type = EXPLOSION_PARTICLE;
    
    return out;
}


//-------------------------------------------------------------------
// * pow *
//-------------------------------------------------------------------
struct Object initPow(int x, int y){
    struct Object obj = initDefaultObject();
    
    obj.x = x;
    obj.y = y;
    obj.team = TEAM_PARTICLE;
    obj.type = POW_PARTICLE;
    obj.internalTimer = 20;
    
    return obj;
}

void updatePow(struct Object* this){
    this->internalTimer--;
    
    this->y += backgroundSpeed / 4;
    if (this->internalTimer == 0){
        this->exists = false;
    }
    
    DrawTexture(powSprite, this->x, this->y, WHITE);
}

//-------------------------------------------------------------------
// * bullets *
//-------------------------------------------------------------------
void updateBullet(struct Object* this){
    if (this->team == 0){
        this->y -= 5;
    }else {
        this->y += 3;
    }
    
    
    if (this->y <= 0){
        this->exists = false;
    }
    
    // draw
    if (this->team == TEAM_PLAYER){
        DrawTexture(bullet, this->x, this->y, WHITE);
    }else {
        DrawTexture(enemyBullet, this->x, this->y, WHITE);
    }
}

struct Object initBullet(int x, int y, int team){
    struct Object out = initDefaultObject();
    
    out.x = x;
    out.y = y;
    out.width = 16;
    out.height = 16;
    out.exists = true;
    out.type = TYPE_BULLET;
    out.team = team;
    
    return out;
}

void bulletCollide(struct Object* this, struct Object* other){
    if (other->team == TEAM_ENEMIES && this->team == TEAM_PLAYER && other->type != TYPE_BULLET){
        this->exists = false;
        other->health -= 10;
        addObject(initPow(this->x, this->y - 10));
        other->variable3 = 5;
    }
}

//-------------------------------------------------------------------
// * enemies *
//-------------------------------------------------------------------


struct Object initEnemy(int x , int enemyType, int aiType, float healthMultiplier){
    struct Object output = initDefaultObject();
    
    output.x = x;
    output.y = -64;
    output.type = ENEMY_SAMUEL;
    output.team = TEAM_ENEMIES;
    output.exists = true;
    output.enemyType = enemyType;
    output.ai = aiType;
    
    switch (enemyType){
        case ENEMY_SAMUEL:
            output.width = 25;
            output.height = 40;
            output.health = (int)(120 * healthMultiplier);
            break;
        case ENEMY_LAMPIR:
            output.width = 30;
            output.height = 49;
            output.health = (int)(300 * healthMultiplier);
            break;
    }
    
    return output;
}

//-------------------------------------------------------------------
// * samuel *
//-------------------------------------------------------------------
int enemiesKilled = 0;

const int AI_DEFAULT = 0;
const int AI_DIVE = 1;
const int AI_SHOOT = 2;
const int AI_SHOOT_DIVE = 3;
const int AI_SNIPER = 4;
void updateSamuel(struct Object* this){
    bool isLampir = this->enemyType == ENEMY_LAMPIR;
    
    
    // smrt
    if (this->health <= 0){
        this->exists = false;
        if (!isLampir){
            addObject(initExplosion(this->x, this->y));
        }else {
            initBigExplosion(this->x, this->y);
        }
        killedEnemy();
    }else if (this->y > inGameHeight){
        this->exists = false;
    }
    if (this->y < 100 || this->ai != AI_SNIPER){
        this->y += 1;
    }
    
    if ((this->ai == AI_DIVE || this->ai == AI_SHOOT_DIVE) && this->y > 120){
        this->y += 2;
    }
        
    if (this->ai == AI_DEFAULT || this->ai == AI_SHOOT || this->ai == AI_SNIPER || this->y < 100){
        if ((this->y % 80 == 0 && GetRandomValue(0, 9) <= 7) || (this->ai == AI_SNIPER && this->internalTimer % 100 == 0)){
        if (this->x < playerX - 10 || this->x > playerX + 26){
            this->variable1 = (this->x < playerX) * 2 - 1;
        }
        
        this->variable2 = 10;
        
        }

        if (this->variable1 != 0 && this->y % this->variable2 == 0){
            this->x += this->variable1;
            this->variable2 -= this->variable2 > 1;
        }
    
    }
    this->internalTimer++;
    if (this->internalTimer % 120 == 0 && (this->ai == AI_SNIPER || this->ai == AI_SHOOT || this->ai == AI_SHOOT_DIVE)){
        addObject(initBullet(this->x + 6, this->y + 6, TEAM_ENEMIES));
    }
    
    
    Color c = WHITE;
    // color
    if (this->variable3 > 0){
        c.r = RED.r;//(unsigned char) lerp((float)c.r, (float)RED.r, this->variable3 / 10);
        c.g = RED.g;
        c.b = RED.b;
        
        this->variable3--;
    }
    // draw
    
    Texture2D* spr = &samuel;
    if (isLampir){
        spr = &lampir;
    }
    DrawTexture(*spr, this->x, this->y, c);
}




//-------------------------------------------------------------------
// * player *
//-------------------------------------------------------------------
struct Player{
    int x;
    int y;
    int fireRate;
    int fireCooldown;
    int projectileCount;
    int invinciblity;
    int deadTimer;
};

int playerHealth = 3;

const int BOUNDRY_WIDTH = 10;
const int BOUNDRY_HEIGHT = 100;

void updatePlayer(struct Player* data){
    // setup vals
    
    bool movingLeft = false;
    bool movingRight = false;
    
    
    // movement
    if (data->deadTimer == 0){    
        if (data->x > BOUNDRY_WIDTH && IsKeyDown(KEY_LEFT)){
            data->x -= 2;
            movingLeft = true;
        }
        else if (data->x < inGameWidth - BOUNDRY_WIDTH - 16 && IsKeyDown(KEY_RIGHT)){
            data->x += 2;
            movingRight = true;
        }
        
        if (data->y < inGameHeight - BOUNDRY_WIDTH - 16 && IsKeyDown(KEY_DOWN)){
            data->y += 1;
        }else if (data->y > inGameHeight - BOUNDRY_HEIGHT && IsKeyDown(KEY_UP)){
            data->y -= 1;
        }
    
        
        playerX = data->x;
        playerY = data->y;
        data->projectileCount = playerLevel;
        // shooting
        if (IsKeyDown(KEY_SPACE) && data->fireCooldown == 0){
            if (data->projectileCount == 1 || data->projectileCount == 3){
                addObject(initBullet(data->x, data->y, 0));
            }
            if (data->projectileCount == 2 || data->projectileCount == 3){
                addObject(initBullet(data->x - 3, data->y + 2, 0));
                addObject(initBullet(data->x + 3, data->y + 2, 0));
                            
            }
            
            data->fireCooldown = data->fireRate;
            PlaySound(shootPlayerSound);
        }
        data->fireCooldown -= data->fireCooldown > 0;
    }
    
    
    // collisions
    for (int i = 0; i < MAX_OBJECTS; i++){
        struct Object* obj = &objects[i];
        
        if (data->deadTimer == 0 && obj->exists && checkBoxCollisions(data->x, data->y, 16, 16, obj->x, obj->y, obj->width, obj->height)){
            switch (obj->team){
                case TEAM_ENEMIES:
                    if (data->invinciblity == 0){
                        data->deadTimer++;
                        addObject(initExplosion(data->x - 10, data->y - 10));
                    }
                    break;
                    
            }
        }
    }
    
    
    
    // drawing
    Texture2D* sprite = &player;
    if (movingLeft){
        sprite = &playerLeft;
    }else if (movingRight){
        sprite = &playerRight;
    }
    data->deadTimer += data->deadTimer != 0;
    data->invinciblity -= data->invinciblity > 0;
    if (data->invinciblity % 4 < 2 && data->deadTimer == 0){
        DrawTexture(*sprite, data->x, data->y, WHITE);
    }
    
}

struct Player initPlayer(){
    struct Player output;
    
    output.y = inGameHeight - 40;
    output.x = inGameWidth / 2 - 8;
    output.fireRate = 10;
    output.fireCooldown = 0;
    output.projectileCount = 1;
    output.invinciblity = 180;
    output.deadTimer = 0;
    playerLevel = 1;
    killedThisLife = 0;
    return output;
}

//-------------------------------------------------------------------
// * big explosion *
//-------------------------------------------------------------------
int explosionTimer = 0;
int explosionX = 0;;
int explosionY = 0;
void initBigExplosion(int x, int y){
    explosionTimer = 45;
    explosionX = x;
    explosionY = y;
}

void updateExplosions(){
    explosionTimer -= explosionTimer > 0;
    
    if (explosionTimer % 3 == 1){
        addObject(initExplosion(GetRandomValue(-10, 10) + explosionX, GetRandomValue(-10, 10) + explosionY));
    }
}

//-------------------------------------------------------------------
// * enemy management *
//-------------------------------------------------------------------
int upgradeTimer = 0;
void upgrade(){
    upgradeTimer = 60;
    PlaySound(bonusSound);
    if (playerLevel < 3){
        playerLevel++;
    }else{
        playerLives++;
    }
}
int enemySpawnTimer = 0;

void updateEnemyManagement(){
    enemySpawnTimer--;
    if (enemySpawnTimer <= 0){
        
        int enemyType = ENEMY_SAMUEL;
        if (GetRandomValue(0, 100) < min(enemiesKilled, 90)){
            enemyType = ENEMY_LAMPIR;
        }
        
        int aiType = GetRandomValue(0, (enemiesKilled > 10) + (enemiesKilled > 30) + (enemiesKilled > 40) + (enemiesKilled > 50) + (enemiesKilled > 60));
        
        
        float healthMultiplier = 1.0f;
        //healthMultiplier += (sin(enemiesKilled) + 1) * 0.2f;
        
        addObject(initEnemy(GetRandomValue(0, inGameWidth - 32), enemyType, aiType, healthMultiplier));
        enemySpawnTimer = 40 + (sin(enemiesKilled) * 10) + (80 * (enemiesKilled < 20)) + (40 * (enemiesKilled < 60)) + (40 * (enemiesKilled < 120));
    }
}   

void killedEnemy(){
    enemiesKilled++;
    killedThisLife++;
    
    if ((playerLevel == 1 && killedThisLife % 10 == 0) ||
        (playerLevel == 2 && killedThisLife % 35 == 0) ||
        (playerLevel >= 3 && killedThisLife % 90 == 0)){
        upgrade();
    }
    
    if (enemiesKilled % 30 == 0){
        changeBackground();
    }
}

//-------------------------------------------------------------------
// * reset *
//-------------------------------------------------------------------

void reset(){
    enemySpawnTimer = 0;
    enemiesKilled = 0;
    playerLives = 3;
    playerLevel = 1;
    killedThisLife = 0;
    currentBackground = 0;
    movedBackgrounds = 0;
    backgroundSpeed = 0;
    
    for (int i = 0; i < MAX_OBJECTS; i++){
        objects[i].exists = false;
    }
}

void gameOver(){
    DrawText("KONEC HRY", 50, 200, 20, WHITE);
    DrawText("STISKNI R PRO RESTART", 50, 300, 1, WHITE);
    
    if (IsKeyDown(KEY_R)){
        reset();
    }
}


//-------------------------------------------------------------------
// * hud *
//-------------------------------------------------------------------

#define ONE 2
#define SCORE_COUNTER_SIZE 10

void drawHud(){
    
    const char* str = "ZIVOTY : ";
    const char num[ONE];
    const char scoreCounter[SCORE_COUNTER_SIZE];
    sprintf(num, "%i", playerLives);
    sprintf(scoreCounter, "%i00", enemiesKilled);
    DrawText( str , 5, 30, 1, WHITE);
    DrawText( num , 60, 30, 1, WHITE);
    DrawText( scoreCounter , 180, 30, 1, WHITE);
    
    upgradeTimer-= upgradeTimer > 0;
    
    if (upgradeTimer % 8 > 4){
        DrawText("BONUS!", 100, 50, 1, WHITE);
    }
}

//------------------------------------------------------------------------------------
// * Background *
//------------------------------------------------------------------------------------
float backgroundOffset = 0.0f;
int fadeTimer = 0;
void updateBackground(){
    backgroundSpeed += (backgroundSpeed < (MAX_BACKGROUND_SPEED * movedBackgrounds) + 3.5f) * 0.005f;
    backgroundOffset += backgroundSpeed;
    if (backgroundOffset > 400){
        backgroundOffset -= 400;
    }
    Texture2D* spr;
    switch (currentBackground){
        case 0: spr = &mudBackground; break;
        case 1: spr = &grassBackground; break;
        case 2: spr = &sandBackground; break;
            
            
    }
    
    // fading
    fadeTimer -= fadeTimer > 0;
    if (fadeTimer == 60){
        currentBackground++;
        currentBackground %= 3;
        movedBackgrounds++;
    }
    
    Color c = WHITE;
    unsigned char dist = (abs(fadeTimer - 60.0f) / 60.0f) * 255;
    c.r = dist;
    c.g = dist;
    c.b = dist;
    
    
    
    DrawTexture(*spr, 0, backgroundOffset, c);
    DrawTexture(*spr, 0, backgroundOffset - 400, c);
    
    
}

void changeBackground(){
    fadeTimer = 120;
}




//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------


int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    
    const Color BACKGROUND_COLOR = {10, 0, 0};
    struct Player playerObject = initPlayer();

    InitWindow(windowWidth, windowHeight, "Educanet Blaster");
    InitAudioDevice();
    
    loadSprites();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    ToggleFullscreen();
    //--------------------------------------------------------------------------------------
    Camera2D cam = {};
    cam.zoom = screenZoom;
    float scalingFactor = screenWidth /(float)(GetScreenWidth());
    int renderTextureOffset = ((GetScreenWidth()) / 2) - (screenWidth / 2);
    
    PlayMusicStream(music);
    
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateMusicStream(music);
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginTextureMode(renderTexture);
            BeginMode2D(cam);
            ClearBackground(BACKGROUND_COLOR);
            updateBackground();
            updateExplosions();
            if (playerObject.deadTimer == 120){
                playerObject = initPlayer();
                playerLives--;
                if (playerLives == 0){
                    PlaySound(gameOverSound);
                }
            }else if (playerLives > 0){
                updatePlayer(&playerObject);
            }else {
                gameOver();
            }
            updateObjects();
            updateEnemyManagement();
            drawHud();
            
            EndMode2D();
        EndTextureMode();
        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);
            
            Rectangle r = { 0, 0, (float)(renderTexture.texture.width), (float)(-renderTexture.texture.height) };
            Rectangle r2 = { renderTextureOffset, 0, (float)(GetScreenWidth()) * scalingFactor, (float)(GetScreenHeight()) };
            Vector2 v = {0, 0};
            DrawTexturePro(
                renderTexture.texture,
                r,
                r2,
                v,
                0,
                WHITE);
        
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    unloadSprites();
    return 0;
}