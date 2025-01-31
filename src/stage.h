#pragma once

#include "input.h"
#include "world.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "audio.h"


enum stageId { TITLE, GAME, MENU, ENDING};
class UI;

class Stage{
public:
    
    float t = 0.0f;
    bool fin;
    bool free_cam;
    
    Matrix44 model;
    Camera* camera;
    Mesh* mesh;
    Texture* texture;
    Shader* shader;
    Audio* audio;
    
    Stage();
    ~Stage();
    //Stage* current_stage;
    virtual void render();
    virtual void update(float elapsed_time);
        
    //void onEnter(Stage* new_stage);
    //void onLeave(Stage* next_stage);
    
};

class TitleStage : public Stage {
    
public:
    int width;
    int height;
    bool intermitent;
    float th = 1.f;
    bool tutorial;
    
    //Menu Stuff
    //bool slot1;
    //bool slot2;
    //bool slot3;
    //std::vector<bool> slots;
    enum slots { slot1, slot2, slot3};
    int currentSlot;
    
    TitleStage();
    ~TitleStage();
    
    stageId getId();
    
    void render();
    void update(float elapsed_time);
    
};

class PlayStage : public Stage {
public:
    int currentDiff;
    std::vector<EntityAI*> enemies;
    std::vector<EntityProjectile*> projectiles;
    Mesh* enemyMesh = nullptr;
    Texture* enemyTexture = nullptr;
    EntityBoss* boss;
    Mesh* bossMesh = nullptr;
    Texture* bossTexture = nullptr;
    
    Mesh quad;
    UI* cdSlot;
    UI* msSlot;
    UI* gmSlot;
    UI* HPBar;
    UI* HP;

    int enemyNum;
    int waveNum;
    float spawnCd;
    float powerUpCd;
    bool soundEffPlayed;
    int lastPowerup;
    
    //UI stuff
    float th = 0.5f;
    float lifeTime;
    float cdLifeTime;
    float msLifeTime;
    float gmLifeTime;
    float lifeTimeTh;
    bool cdIntermitent;
    bool msIntermitent;
    bool gmIntermitent;
    Vector2 slot1;
    Vector2 slot2;
    Vector2 slot3;
    Vector2 currentSlot;
    bool slot1InUse;
    bool slot2InUse;
    bool slot3InUse;
    int bossLvl;
    bool isBossLvl;
    float HPtoUI;
    

    
    //color test
    Vector4 color;
    bool gradient;
    float gradientFactor;
    bool walkDownX;
    bool walkDownY;
    bool walkDownZ;
    bool walkUpZ;

    // Enemy Stats
    int hp, rand, bulletsShoot;
    float spd, cdShot, dispersion;
    Matrix44 model;
    Mesh* entityMesh;
    Entity* root;
    EntityAI* newEnemy;

    // Powerup
    int power;
    EntityPowerUp* powerUp;

    // render
    Mesh* nMesh;
    Shader* nShader;

    PlayStage();
    ~PlayStage();
    
    stageId getId();

    void loadNewLvl(float seconds_elapsed);
    void loadBossLvl(float seconds_elapsed);
    void loadPowerUp(float seconds_elapsed);

    
    void render();
    void update(float elapsed_time);
    
    void onKeyDown( SDL_KeyboardEvent event );
    void onMouseWheel(SDL_MouseWheelEvent event);
    void onResize(int width, int height);
};

class MenuStage : public Stage {
public:
    
    MenuStage();
    ~MenuStage();
    Mesh quad;
    UI* tutorial;
    Texture* texture = nullptr;
    
    stageId getId();
    
    void render();
    void update(float elapsed_time);
    
};

class EndStage : public Stage {
public:
    
    bool restart;
    bool retry;
    EndStage();
    ~EndStage();
    
    stageId getId();
    
    void render();
    void update(float seconds_elapsed);
};

class StageManager : public Stage {
    
    static StageManager* instance;
    
public:
    
    void changeStage(Stage* new_stage);

};
/**
 MenuStage
        width = game::instance->window_width;
 
 PlayStage
 

 
 
 playButton = new EntityGUIelement;
 
 
 //hacer un shader que el vertice lo aplicas en clip space
 background (quad) = new EntityGUIelement(
        Vector2(0.0 , 0.0),
        vector2(0.5, 0.5),
        Texture:Get("path")
 //hacer un shader con camara ortografica --->opcion 1
 //viewprojection * el vector
background = newENtityGUIelement(
        //en world gui_camera = new camera
        //gui_camera->setOrthographic
                Vecotr2(width/2 * height)
                Vector2(200,60
                Texture::GEt
 )
 
 //mouse input to rotate the cam
 if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed?
 {
     camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
     camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector( Vector3(-1.0f,0.0f,0.0f)));
 }

 //async input to move the camera around
 if(Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ) speed *= 10; //move faster with left shift
 if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
 if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f,-1.0f) * speed);
 if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
 if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f,0.0f, 0.0f) * speed);

 //to navigate with the mouse fixed in the middle
 if (mouse_locked)
     Input::centerMouse();
 
 */

