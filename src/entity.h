//
//  entity.h
//  TJE_XCODE
//
//  Created by manu on 11/5/23.
//  Copyright © 2023 manu. All rights reserved.
//

/*#ifndef entity_h
#define entity_h*/

#pragma once
#include <math.h>
#include "mesh.h"
#include "string.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "animation.h"
#include "framework.h"


//#endif /* entity_h */

enum powerUps { NONE, MULTISHOT, MORECADENCE, IMMORTAL };
enum animation {IDLE, WALK, WALK_BACK, WALK_LEFT, WALK_RIGHT, SHOOT, DIE};

class Entity {

public:
    Entity(std::string name, Matrix44 model);         // Constructor
    ~Entity();     // Destructor
    
    // Some attributes
    std::string name;
    Matrix44 model;
    // Methods that should be overwritten
    // by derived classes
    virtual void render();
    virtual void update(float elapsed_time);
    
    Entity* parent;
    
    std::vector<Entity*> children;
    
    void addChild(Entity* child);
    void removeChild(Entity* child);
    void debugChildren();

    // Some useful methods
    Matrix44 getGlobalMatrix();
};

class EntityMesh : public Entity {

public:

    // Attributes of the derived class
    Mesh* mesh = nullptr;
    Texture* texture = nullptr;
    Shader* shader = nullptr;
    Vector4 color = Vector4(1, 1, 1, 1);
    //EntityMesh(std::string name, Matrix44 model, Mesh* mesh, Texture* texture, Shader* shader, Vector4 color);
    EntityMesh(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture);
    ~EntityMesh();
    //EntityMesh(std::string name, Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture);

    // Methods overwritten from base class
    void render();
    void update(float elapsed_time);
};

class InstancedEntityMesh : public EntityMesh {
    public:
    Mesh* mesh = nullptr;
    Texture* texture = nullptr;
    Shader* shader = nullptr;
    Vector4 color;
    std::vector<Matrix44> models;

    //InstancedEntityMesh(std::string name, Matrix44 model, Mesh* mesh, Texture* texture, Shader* shader, Vector4 color);
    InstancedEntityMesh(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture);
    ~InstancedEntityMesh();

    // Methods overwritten from base class
    void render();
    void update(float elapsed_time);
};

class EntityCollider : public EntityMesh {

public:

    bool isDynamic = false;

    EntityCollider(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture);
    ~EntityCollider();

    // New methods
    //bool checkPlayerCollisions(Vector3 position, Vector3 &colisions);
    void render();
    void update(float elapsed_time);
};


class EntityPowerUp : public EntityMesh {

public:

    float lifeTime;
    float lifeTimeTh;
    bool inWorld;
    float angle;
    float azimuth;
    float th = 0.5f;
    bool intermitent;
    powerUps effect;


    EntityPowerUp(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture, float lifeTime, powerUps effect);
    ~EntityPowerUp();
    void render();
    void update(float elapsed_time);
};

class EntityPlayer : public EntityMesh{
    public:
        // Attributes of the derived class
        int hp, maxHP;
        float speed, shootCd, cdPowerUp, cdCadLife, multiLife, immortalLife;
        bool isDead, hasMultishot, hasCdPower;
        int killCount;
        bool godMode;
        Vector3 velocity;
        int anim;
        int puNum;
    
        Animation* idle;
        Animation* walk;
        Animation* walkBack;
        Animation* walkLeft;
        Animation* walkRight;
        Animation* shot;
        Animation* die;
    
   
        float yaw;
        Camera* camera = nullptr;
        //EntityPlayer(std::string name, Matrix44 model, Mesh* mesh, Texture* texture, Shader* shader, Vector4 color, Camera* camera);
        EntityPlayer(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture, Camera* camera);
        ~EntityPlayer();
        // Methods overwritten from base class
        //void shoot(Vector3 dir, float speed);
        void addPowerUp(EntityPowerUp* pu);
        void render();
        void update(float elapsed_time);
};

enum behaviour {NOTHING = 0, WANDER, ATTACK};

class EntityAI : public EntityCollider {
public:
    // Attributes of the derived class
    int hp, maxhp;
    behaviour currentBehaviour;
    float speed;
    Vector3 velocity = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 move_dir = Vector3(0.0f, 0.0f, 0.0f);
    float wanderChange;
    float cdShot, shotCdTime, dispersion;
    bool hasBeenAttacked;

    float yaw;
    //EntityAI(std::string name, Matrix44 model, Mesh* mesh, Texture* texture, Shader* shader, Vector4 color, Camera* camera);
    EntityAI(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture, int hp, float speed, float cdShot, float dispersion);
    ~EntityAI();
    // Methods overwritten from base class
    //void shoot(Vector3 dir, float speed);
    void render();
    void behaviourUpdate();
    bool canSeePlayer();
    void update(float elapsed_time);
    void setYaw(Vector3 moveDir, float elapsed_time);
};

class EntityBoss : public EntityAI {
public:
    int numBulletsShoot;
    bool isHurt;
    float hurtFrames;
    float HPbar;

    EntityBoss(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture, int hp, float speed, float cdShot, float dispersion, int numBulletsShoot );
    ~EntityBoss();
    
    void render();
    void update(float elapsed_time);
};

class EntityProjectile : public EntityCollider{
    
    public:
        
        float speed;
        float dmg;
        Vector3 dir;
        float lifeTime = 4.f;
        bool isEnemy;
        //Vector3 position;
        Vector3 velocity = Vector3(0.0f,0.0f,0.0f);
        
        EntityProjectile(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture, float speed, float dmg, Vector3 dir, bool isEnemy);
        ~EntityProjectile();
        
        void render();
        void update(float elapsed_time);
};

/*class EntityEnemy : public EntityMesh{
    public:
        // Attributes of the derived class
        float speed = 1.0f;
        EntityEnemy(std::string name, Matrix44 model, Mesh* mesh, Texture* texture, Shader* shader, Vector4 color);
        // Methods overwritten from base class
        void render();
        void update(float elapsed_time);
};*/

/*

//CREAR ENTITY PLAYER TAMBIEN
 entity colider
 entity enemy

 
 
 class EntityGUIelement : Entitty Mesh{}
 
 is3d
 update3Dpos
 
 render
 update(){
 
    Vector2 mouse_position = INput:mouse_position;
    
    if(button_id !: mouse_position &&
        mouse_pos.x > (position.x - size.x * 0.5) &&
        mouse_pos.x < (position.x + size.x * 0.5) &&
         mouse_pos.y > (position.y - size.x * 0.5) &&
         mouse_pos.y < (position.y + size.x * 0.5)){
    color = Vector4(1,0,0,1);
    if input::isMousePressed...
        StageManager::getInstance()->onButtonPressed()
 
 }
 }
*/
