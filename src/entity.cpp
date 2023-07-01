//
//  entity.cpp
//  TJE_XCODE
//
//  Created by manu on 17/5/23.
//  Copyright © 2023 manu. All rights reserved.
//

#include "entity.h"
#include "input.h"
#include "world.h"
#include "game.h"
#include "framework.h"

#include "random.h"

// Container to store EACH collision
struct sCollisionData {
    Vector3 colPoint;
    Vector3 colNormal;
};

Entity::Entity(std::string name, Matrix44 model){
    this->name = name;
    this->model = model;
};
Entity::~Entity() {
    
};


void Entity::addChild(Entity* child){
    children.push_back(child);
    child->parent = this;
}

void Entity::removeChild(Entity* child){
    auto it = std::find(children.begin(), children.end(), child);
    if (it == children.end()) {
        std::cout << "Child not in list" << std::endl;
        return;
    }
    
    children.erase(it);
    child->parent = nullptr;
}

void Entity::render(){
    for(int i = 0; i < children.size(); i++)
        children[i]->render();
}
void Entity::update(float elapsed_time){
    for(int i = 0; i < children.size(); i++)
    children[i]->update(elapsed_time);
}

Matrix44 Entity::getGlobalMatrix(){
    if(parent)
        return model * parent->getGlobalMatrix();
    return model;
}

void Entity::debugChildren(){
    for(int i = 0; i < children.size(); i++)
        std::cout << i << ' ';
}

/*EntityMesh::EntityMesh(std::string name, Matrix44 model, Mesh* mesh, Texture* texture, Shader* shader, Vector4 color):Entity(name, model){
    this->mesh =  mesh;
    this->texture = texture;
    this->shader = shader;
    this->color = color;
}*/


EntityMesh::EntityMesh(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture) : Entity("Default", model)
{
    this->mesh = mesh;
    this->texture = texture;
    this->shader = shader;
}

/*EntityMesh::EntityMesh(std::string name, Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture) : Entity(name, Matrix44())
{
    this->mesh = mesh;
    this->texture = texture;
    this->shader = shader;
}*/

void EntityMesh::render(){
    // Get the last camera that was activated
    Camera* camera = Camera::current;

    // Enable shader and pass uniforms
    shader->enable();
    shader->setUniform("u_model", model);
    shader->setUniform("u_viewproj", camera->viewprojection_matrix);
    shader->setTexture("u_texture", texture, 0);


    // Render the mesh using the shader
    mesh->render( GL_TRIANGLES );

    // Disable shader after finishing rendering
    shader->disable();

}

void EntityMesh::update(float elapsed_time){
    
}


/*InstancedEntityMesh::InstancedEntityMesh(std::string name, Matrix44 model, Mesh* mesh, Texture* texture, Shader* shader, Vector4 color):EntityMesh(name, model, mesh, texture, shader, color){
    this->mesh =  mesh;
    this->texture = texture;
    this->shader = shader;
    this->color = color;
    models = std::vector<Matrix44>();
}
*/
InstancedEntityMesh::InstancedEntityMesh(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture):EntityMesh(model, mesh, shader, texture)
{
    models = std::vector<Matrix44>();
}

void InstancedEntityMesh::render(){
    // Get the last camera that was activated
    Camera* camera = Camera::current;

    // Enable shader and pass uniforms
    shader->enable();
    for(int i = 0; i < this->models.size(); i++){
        shader->setUniform("u_model", this->models[i]);
        shader->setUniform("u_viewproj", camera->viewprojection_matrix);
        shader->setTexture("u_texture", texture, 0);
        
        // Render the mesh using the shader
        mesh->render( GL_TRIANGLES );
    }


    // Disable shader after finishing rendering
    shader->disable();

}

void InstancedEntityMesh::update(float elapsed_time){
    
}

/*EntityPlayer::EntityPlayer(std::string name, Matrix44 model, Mesh* mesh, Texture* texture, Shader* shader, Vector4 color, Camera* camera):EntityMesh(name, model, mesh, texture, shader, color){
    this->camera = camera;
    
}*/

EntityPlayer::EntityPlayer(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture, Camera* camera):EntityMesh(model, mesh, shader, texture){
    this->camera = camera;
    this->hp = 750;
    this->yaw = 0.f;
    this->isDead = false;
    this->godMode = false;
    killCount = 0;
    velocity = Vector3(0.0f, 0.0f, 0.0f);
    speed = 1500.0f;
    shootCd = 0.f;
}

void shoot(Matrix44 model, float speed, float dispersion, bool isEnemy){
    Mesh* mesh;
    Shader* shader;
    Texture* texture;

    //texture = Texture::Get("data/texture.tga");
    texture = World::get_instance()->projectileTexture;

    // example of loading Mesh from Mesh Manager
    //mesh = Mesh::Get("data/projectile.obj");
    mesh = World::get_instance()->projectileMesh;

    // example of shader loading using the shaders manager
    //shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
    shader = World::get_instance()->shader;
    
    float dmg = 0.0f;
    Vector3 dir = model.frontVector();
    if (dispersion)
    {
        dir.x += random(dispersion, -dispersion / 2);
        dir.z += random(dispersion, -dispersion / 2);
    }
    model.translate(0.0f, 0.0f, 51.f);
    
    EntityProjectile* bullet = new EntityProjectile(model, mesh, shader, texture, speed, dmg, dir, isEnemy);
    World::world->get_instance()->root->addChild(bullet);
    if (random() > 0.5f) Audio::PlayS("data/audio/363698__jofae__retro-gun-shot.mp3");
    else Audio::PlayS("data/audio/mixkit-game-gun-shot-1662.mp3");
}

void multishot(Matrix44 model, float speed, int bulletsShoot, float dispersion, bool isEnemy){
    
    Mesh* mesh;
    Shader* shader;
    Texture* texture;

    //texture = Texture::Get("data/texture.tga");
    texture = World::get_instance()->projectileTexture;

    // example of loading Mesh from Mesh Manager
    //mesh = Mesh::Get("data/projectile.obj");
    mesh = World::get_instance()->projectileMesh;

    // example of shader loading using the shaders manager
    //shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
    shader = World::get_instance()->shader;
    
    float dmg = 0.0f;
    Vector3 dir = model.frontVector();

    /*if (dispersion) //multishot sin dispersion
    {
        dir.x += random(dispersion, -dispersion / 2);
        dir.z += random(dispersion, -dispersion / 2);
    }*/
    model.translate(0.0f, 0.0f, 51.f);
    int range = bulletsShoot / 2;
    
    for (int i = -range; i <= range; i++)
    {
        Vector3 newDir = dir - Vector3(i * dispersion, 0, i * dispersion);
        newDir.normalize();
        EntityProjectile* bullet = new EntityProjectile(model, mesh, shader, texture, speed, dmg, newDir, isEnemy);
        World::world->get_instance()->root->addChild(bullet);
    }
    if (random() > 0.5f) Audio::PlayS("data/audio/363698__jofae__retro-gun-shot.mp3");
    else Audio::PlayS("data/audio/mixkit-game-gun-shot-1662.mp3");
}

void youDie(Entity* entity, EntityProjectile* p){
    if (p->isEnemy){
        if(EntityPlayer* e = dynamic_cast<EntityPlayer*>(entity)){
            if (!e->godMode){
                e->isDead = true;
                Audio::Play("data/audio/videogame-death-sound-43894.mp3");
            }
                
            //std::cout << "u suck" << std::endl;
            else{

            }
        }
    }
    else{
        if (EntityBoss* b = dynamic_cast<EntityBoss*>(entity))
        {
            b->hp--;
            b->hasBeenAttacked = true;
            b->isHurt = true;
            b->color = Vector4(1, 0, 0, 1);
            World::get_instance()->root->removeChild(p);
            std::cout << b->hp << std::endl;
            if (b->hp == 0)
            {
                World::get_instance()->root->removeChild(entity);
                PlayStage* stage = ((PlayStage*)Game::instance->current_stage);
                stage->enemyNum--;
                World::get_instance()->player->killCount++;
                Audio::Play("data/audio/expl6.wav");
            }
            else{
                Audio::Play("data/audio/hitmarker_2.mp3");
            }
        }
        else
        {
            World::get_instance()->root->removeChild(entity);
            World::get_instance()->root->removeChild(p);
            PlayStage* stage = ((PlayStage*)Game::instance->current_stage);
            stage->enemyNum--;
            World::get_instance()->player->killCount++;
            Audio::Play("data/audio/expl6.wav");
        }
        
    }
}

Vector3 getMouseToWorld(Vector3 mouse_pos){
    //get floor plane
    Entity* plane = World::get_instance()->root->children[4];
    EntityMesh* e = dynamic_cast<EntityMesh*>(plane);
    //e->mesh->collision_model->rayCollision();//testRayCollision(e->model, Vector3 (0,0,0), Vector3 (0,0,0), Vector3 (0,0,0), Vector3 (0,0,0));


    Vector3 up = Vector3(0, 1, 0);
    return up;
}

void EntityPlayer::render(){
    // Get the last camera that was activated
    Camera* camera = Camera::current;

    // Enable shader and pass uniforms
    shader->enable();
    shader->setUniform("u_color", color);
    shader->setUniform("u_model", model);
    shader->setUniform("u_viewproj", camera->viewprojection_matrix);
    shader->setTexture("u_texture", texture, 0);


    // Render the mesh using the shader
    mesh->render( GL_TRIANGLES );

    // Disable shader after finishing rendering
    shader->disable();

}

bool checkCollisions(const Vector3& target_pos, std::vector<sCollisionData>& collisions, EntityMesh* entity, float radiusSphere) {
    Vector3 center = target_pos + Vector3(0.f, 1.25f, 0.f);
    float sphereRadius = radiusSphere;
    Vector3 colPoint, colNormal;

    // For each collider entity “e” in root:
    //for(auto e:World::world->get_instance()->root->children){
    for (int i = 0; i < World::world->get_instance()->root->children.size(); i++) {
        if (EntityCollider* e = dynamic_cast<EntityCollider*>(World::world->get_instance()->root->children[i])) {
            Mesh* mesh = e->mesh;
            if (EntityProjectile* p = dynamic_cast<EntityProjectile*>(e)) {
                if (mesh->testSphereCollision(e->model, center, sphereRadius, colPoint, colNormal)) {
                    youDie(entity, p);
                }
            }
            else
            {
                if (mesh->testSphereCollision(e->model, center, sphereRadius, colPoint, colNormal)) {
                    collisions.push_back({ colPoint, colNormal.normalize() });
                }
            }
            // End loop
        }
    }
    return !collisions.empty();
}

Vector3 lookingAt()
{
    Vector2 mouse_pos = Input::mouse_position;
    return Vector3((mouse_pos.x / Game::instance->window_width) * 8200 - 4100, 51.f, (mouse_pos.y / Game::instance->window_height) * 6000 - 3000);;
}

void EntityPlayer::update(float elapsed_time){
    Vector3 position = model.getTranslation();
    std::vector <sCollisionData> collisions;

    yaw += this->model.getYawRotationToAimTo(lookingAt());


    
    Vector3 move_dir = Vector3(0.0f, 0.0f, 0.0f);
    
    if (Input::isKeyPressed(SDL_SCANCODE_W)) {
        //model.translate(0.0f, 0.0f, -1.0f * move_speed);
        move_dir = Vector3(0.0f + move_dir.x, 0.0f, -1.0f + move_dir.z);
        
        //velocity = move_dir * speed;
        //position = position + velocity * elapsed_time;
        //model.setTranslation(position.x, 51.0f, position.z);
    }
    if (Input::isKeyPressed(SDL_SCANCODE_S)) {
        //model.translate(0.0f, 0.0f, 1.0f * move_speed);
        move_dir = Vector3(0.0f + move_dir.x, 0.0f, 1.0f + move_dir.z);
        //velocity = move_dir * speed;
        //position = position + velocity * elapsed_time;
        //model.setTranslation(position.x, 51.0f, position.z); //el 51 es hardcodeado por la mesh del cubo (se
    }
    if (Input::isKeyPressed(SDL_SCANCODE_A)) {
        //model.translate(-1.0f * move_speed, 0.0f, 0.0f);
        move_dir = Vector3(-1.0f + move_dir.x, 0.0f, 0.0f + move_dir.z);
        //velocity = move_dir * speed;
        //position = position + velocity * elapsed_time;
        //model.setTranslation(position.x, 51.0f, position.z); //el 51 es hardcodeado por la mesh del cubo (se
    }
    if (Input::isKeyPressed(SDL_SCANCODE_D)) {
        //model.translate(1.0f * move_speed, 0.0f, 0.0f);
        move_dir = Vector3(1.0f + move_dir.x, 0.0f, 0.0f + move_dir.z);
        //velocity = move_dir * speed;
        //position = position + velocity * elapsed_time;
        //model.setTranslation(position.x, 51.0f, position.z); //el 51 es hardcodeado por la mesh del cubo (se
    }
    if (Input::wasKeyPressed(SDL_SCANCODE_X)) {
        this->godMode = !this->godMode;
        if (godMode)
            std::cout << "God Mode Activated" << std::endl;
        if (!godMode)
            std::cout << "God Mode Deactivated" << std::endl;
    }
    
    move_dir.normalize();
    velocity = move_dir * speed;
    position.y = 51.0f; //el 51 es hardcodeado por la mesh del cubo (se tiene en cuenta el centro de la mesh)
    if (checkCollisions(position + velocity * elapsed_time, collisions, this, 12.5f)) {
        //std::cout << position.x << " " << position.y << " " << position.z << std::endl;
        for (const sCollisionData& collisions : collisions) {
            //Vector3& velocity = velocity;
            Vector3 newDir = velocity.dot(collisions.colNormal) * collisions.colNormal;
            velocity.x -= newDir.x;
            velocity.z -= newDir.z;
        }
    }
    position = position + velocity * elapsed_time;
    
    velocity -= velocity * elapsed_time;
    
    model.setTranslation(position.x, position.y, position.z); // position.y = 51 harcoceado
    
    //std::cout << model.getTranslation().x << " " << model.getTranslation().y << " " << model.getTranslation().z << std::endl;
    /*if (Input::mouse_position.y < Game::instance->window_height/2){
        model.rotate(yaw, Vector3(0.0f, -1.0f, 0.0f));
    }
    if (Input::mouse_position.y >= Game::instance->window_height/2){
        model.rotate(yaw, Vector3(0.0f, 1.0f, 0.0f));
    }*/
    model.rotate(yaw, Vector3(0.0f, 1.0f, 0.0f));

    shootCd += elapsed_time;
    //camera->lookAt(camera->eye, camera->center, camera->up);
    if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) {
        if (shootCd > .3f)
        {
            shoot(model, 4000.0f, 0, false);
            shootCd = 0.f;
        }
    }
}

EntityAI::EntityAI(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture, int hp, float speed, float cdShot, float dispersion)
    :EntityMesh(model, mesh, shader, texture) {
    this->hp = hp;
    this->maxhp = hp;
    this->speed = speed;
    this->currentBehaviour = NOTHING;
    this->cdShot = cdShot;
    this->dispersion = dispersion;
    this->yaw = 0.f;
    this->shotCdTime = 0.f;
    this->wanderChange = 30.f;
    this->hasBeenAttacked = false;
    this->hitCd = 0.f;
}

void EntityAI::render()
{
    // Get the last camera that was activated
    Camera* camera = Camera::current;

    // Enable shader and pass uniforms
    shader->enable();
    //shader->setUniform("u_color", color);
    shader->setUniform("u_model", model);
    shader->setUniform("u_viewproj", camera->viewprojection_matrix);
    shader->setTexture("u_texture", texture, 0);


    // Render the mesh using the shader
    mesh->render(GL_TRIANGLES);

    // Disable shader after finishing rendering
    shader->disable();
}

bool EntityAI::canSeePlayer()
{
    Vector3 target = World::get_instance()->player->model.getTranslation() - this->model.getTranslation();
    target = target.normalize();
    Vector3 eye = this->model.frontVector();
    eye = eye.normalize();
    return (eye.dot(target) >= .5f) ? true : false;
}

void EntityAI::behaviourUpdate()
{
    if (canSeePlayer() || hasBeenAttacked)
        currentBehaviour = ATTACK;
    else currentBehaviour = WANDER;
}

Vector3 get_center_dir(Vector3 pos)
{
    return Vector3(-pos.x, 0, -pos.z).normalize();
}

void takeAction(EntityAI* entity, Vector3 position, float elapsed_time)
{
    bool isBoss = (dynamic_cast<EntityBoss*>(entity) ? true : false);
    if (EntityBoss* b = dynamic_cast<EntityBoss*>(entity)) if(b->isHurt) b->hurtFrames += elapsed_time;
    switch (entity->currentBehaviour)
    {
    case ATTACK:
        entity->move_dir = World::get_instance()->player->model.getTranslation() - entity->model.getTranslation();
        entity->shotCdTime += elapsed_time;
        if (entity->shotCdTime > entity->cdShot)
        {
            if (EntityBoss* b = dynamic_cast<EntityBoss*>(entity))
            {
                multishot(b->model, 2500.f, b->numBulletsShoot, b->dispersion, true);
            }
            else
                shoot(entity->model, 3000.f, entity->dispersion, true);
            entity->shotCdTime = 0.f;
        }
        if(isBoss){
            entity->yaw += entity->model.getYawRotationToAimTo
            (
             World::get_instance()->player->model.getTranslation() + World::get_instance()->player->velocity * Vector3(0.5f, 0.5f, 0.5f)
             );
        }
        else{
            entity->yaw += entity->model.getYawRotationToAimTo
            (
             World::get_instance()->player->model.getTranslation()
             );
        }
        //std::cout << entity->move_dir.length() << std::endl;
        if (isBoss)
        {
            //if (entity->move_dir.length() < 4000.f)
            if (entity->move_dir.length() < 4000.f)
                entity->move_dir = Vector3(0.f, 0.f, 0.f);
        }
        else
        {
            if (entity->move_dir.length() < 2500.f)
                entity->move_dir = Vector3(0.f, 0.f, 0.f);
        }
        break;
    case WANDER:
        //entity->speed = entity->speed - 800;
        if (entity->wanderChange > 2.f)
        {
            Vector3 centerDir = get_center_dir(entity->model.getTranslation());
            entity->move_dir = Vector3(centerDir.x + get_random_dir(), 0.f, centerDir.y + get_random_dir());
            entity->wanderChange = .0f;
        }
        entity->yaw += entity->model.getYawRotationToAimTo(position + entity->move_dir);
        //entity->speed = entity->speed + 800;
        break;
    default:
        break;
    }
    entity->move_dir.normalize();
}

void checkCollisions(EntityAI* entity, Vector3 position, float elapsed_time)
{
    std::vector <sCollisionData> collisions;
    if(entity == dynamic_cast<EntityAI*>(entity)){
        if (checkCollisions(position + entity->velocity * elapsed_time, collisions, entity, 12.5f)) {
            //std::cout << position.x << " " << position.y << " " << position.z << std::endl;
            for (const sCollisionData& collisions : collisions) {
                //Vector3& velocity = velocity;
                Vector3 newDir = entity->velocity.dot(collisions.colNormal) * collisions.colNormal;
                entity->velocity.x -= newDir.x;
                entity->velocity.z -= newDir.z;
            }
        }
    }
    else if(entity == dynamic_cast<EntityBoss*>(entity)){
        if (checkCollisions(position + entity->velocity * elapsed_time, collisions, entity, 62.5f)) {
            //std::cout << position.x << " " << position.y << " " << position.z << std::endl;
            for (const sCollisionData& collisions : collisions) {
                //Vector3& velocity = velocity;
                Vector3 newDir = entity->velocity.dot(collisions.colNormal) * collisions.colNormal;
                entity->velocity.x -= newDir.x;
                entity->velocity.z -= newDir.z;
            }
        }
    }
}

void EntityAI::update(float elapsed_time)
{
    wanderChange += elapsed_time;
    Vector3 position = model.getTranslation();
    // yaw = degree between player and enemy; acos or asin? but that's inneficient
    behaviourUpdate();
    takeAction(this, position, elapsed_time);

    velocity = move_dir * speed;
    position.y = 51.0f; //el 51 es hardcodeado por la mesh del cubo (se tiene en cuenta el centro de la mesh)

    checkCollisions(this, position, elapsed_time);
    
    position = position + velocity * elapsed_time;
    
    velocity -= velocity * elapsed_time;

    model.setTranslation(position.x, position.y, position.z); // position.y = 51 harcoceado
    model.rotate(yaw, Vector3(0.0f, 1.0f, 0.0f));
    //color = Vector4(1, 1, 1, 1);
}


EntityBoss::EntityBoss(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture, int hp, float speed, float cdShot, float dispersion, int numBulletsShoot)
    :EntityAI(model, mesh, shader, texture, hp, speed, cdShot, dispersion) {
        
    this->numBulletsShoot = numBulletsShoot;
    color = Vector4(1, 1, 1, 1);
    this->isHurt = false;
    this->hurtFrames = 0.f;
}

void EntityBoss::render(){
    // Get the last camera that was activated
    Camera* camera = Camera::current;

    // Enable shader and pass uniforms
    shader->enable();
    if (isHurt)
    {
        if (hurtFrames > .2f)
        {
            color = Vector4(1, 1, 1, 1);
            isHurt = false;
            hurtFrames = 0.f;
        }
    }
    shader->setUniform("u_color", color);
    shader->setUniform("u_model", model);
    shader->setUniform("u_viewproj", camera->viewprojection_matrix);
    shader->setTexture("u_texture", texture, 0);


    // Render the mesh using the shader
    mesh->render(GL_TRIANGLES);

    // Disable shader after finishing rendering
    shader->disable();
}

EntityCollider::EntityCollider(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture):EntityMesh(model,mesh,shader,texture){
}

void EntityCollider::render(){
    // Get the last camera that was activated
    Camera* camera = Camera::current;

    // Enable shader and pass uniforms
    shader->enable();
    shader->setUniform("u_model", model);
    shader->setUniform("u_viewproj", camera->viewprojection_matrix);
    shader->setTexture("u_texture", texture, 0);


    // Render the mesh using the shader
    mesh->render( GL_TRIANGLES );

    // Disable shader after finishing rendering
    shader->disable();
}

void EntityCollider::update(float elapsed_time){
}

EntityProjectile::EntityProjectile(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture, float speed, float dmg, Vector3 dir, bool isEnemy):EntityCollider(model, mesh, shader, texture){
    
    this->speed = speed;
    this->dmg = dmg;
    this->dir = dir;
    this->isEnemy = isEnemy;
}

void EntityProjectile::render(){
    
    // Get the last camera that was activated
    Camera* camera = Camera::current;

    // Enable shader and pass uniforms
    shader->enable();
    shader->setUniform("u_color", color);
    shader->setUniform("u_model", model);
    shader->setUniform("u_viewproj", camera->viewprojection_matrix);
    shader->setTexture("u_texture", texture, 0);


    // Render the mesh using the shader
    mesh->render( GL_TRIANGLES );

    // Disable shader after finishing rendering
    shader->disable();
}

struct sImpactData {
    Vector3 impPoint;
    Vector3 impNormal;
};

bool checkImpacts(const Vector3& target_pos,
    std::vector<sImpactData>& impacts, bool isEnemy) {
    Vector3 center = target_pos + Vector3(0.f, 1.25f, 0.f);
    float sphereRadius = 3.f;
    Vector3 impPoint, impNormal;

    // For each collider entity “e” in root:
    //for(auto e:World::world->get_instance()->root->children){
    for (int i = 0; i < World::world->get_instance()->root->children.size(); i++) {
        if (EntityProjectile* b = dynamic_cast<EntityProjectile*>(World::world->get_instance()->root->children[i]))
            if (b->isEnemy && isEnemy) continue;
        if (EntityCollider* e = dynamic_cast<EntityCollider*>(World::world->get_instance()->root->children[i])) {
            Mesh* mesh = e->mesh;

            if (mesh->testSphereCollision(e->model, center, sphereRadius, impPoint, impNormal)) {
                return true;
            }
        }
        // End loop
    }
    return false;
}

void EntityProjectile::update(float elapsed_time){
    
    Vector3 position = model.getTranslation();
    
    dir.normalize();
    velocity =  dir * speed;
    
    float height = model.getTranslation().y;
    position.y = height;
    
    std::vector<sImpactData> impacts;
    if (checkImpacts(position + velocity * elapsed_time, impacts, this->isEnemy)) {
        
        World::world->get_instance()->root->removeChild(this);
        
    } else {
        if (this->lifeTime < elapsed_time){
            World::world->get_instance()->root->removeChild(this);
        }
    }
    position = position + velocity * elapsed_time;
    model.setTranslation(position.x, position.y, position.z);
}

/*void EntityProjectile::update(float elapsed_time){
    Vector3 velocity = dir * speed;
    Vector3 new_position = model.getTranslation() + velocity * elapsed_time;
    model.setTranslation(new_position.x, new_position.y, new_position.z);
    
}*/


/*if (checkPlayerCollisions(position + player->velocity * seconds_elapsed, &collisions)

   for (const sCollisionsDara& collisions: collisions)
       //if normal is pointing upwards it means we are on the floor
       float up_factor = collision.colNormal.dot(Vector(0,1,0))
       if (up_factor > 0.8)
           isOnFloor = true
       else
           //move along wall when colliding
           Vector3 newDir = player->velocity.dot(collision.colNormal) * collision.colNormal;
           player->velocity.x -= newDir.x;
           player->velocity.z -= newdir.z*/

