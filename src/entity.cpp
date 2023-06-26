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

#include <random>

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
}

void Entity::removeChild(Entity* child){
    children.pop_back();
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
}

void shoot(Matrix44 model, float speed, float dispersion){
    Mesh* mesh;
    Shader* shader;
    Texture* texture;
    
    texture = new Texture();
    texture->load("data/texture.tga");

    // example of loading Mesh from Mesh Manager
    mesh = Mesh::Get("data/projectile.obj");

    // example of shader loading using the shaders manager
    shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
    
    float dmg = 0.0f;
    Vector3 dir = model.frontVector();
    dir.x += dispersion;
    dir.z += dispersion;
    model.translate(0.f, 0.0f, 0.f);
    
    EntityProjectile* bullet = new EntityProjectile(model, mesh, shader, texture, speed, dmg, dir);
    World::world->get_instance()->root->addChild(bullet);
}

void EntityPlayer::render(){
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

bool checkCollisions(const Vector3& target_pos,
    std::vector<sCollisionData>& collisions) {
    Vector3 center = target_pos + Vector3(0.f, 1.25f, 0.f);
    float sphereRadius = .75f;
    Vector3 colPoint, colNormal;

    // For each collider entity “e” in root:
    //for(auto e:World::world->get_instance()->root->children){
    for (int i = 0; i < World::world->get_instance()->root->children.size(); i++) {
        if (EntityCollider* e = dynamic_cast<EntityCollider*>(World::world->get_instance()->root->children[i])) {
            Mesh* mesh = e->mesh;

            if (mesh->testSphereCollision(e->model, center,
                sphereRadius, colPoint, colNormal)) {
                collisions.push_back({ colPoint,
                    colNormal.normalize() });
            }
        }
        // End loop
    }
    return !collisions.empty();
}

void EntityPlayer::update(float elapsed_time){
    Vector3 position = model.getTranslation();
    std::vector <sCollisionData> collisions;

    yaw += Input::mouse_delta.x * elapsed_time * 15.0f;
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
    
    move_dir.normalize();
    velocity = velocity + move_dir * speed;
    position.y = 51.0f; //el 51 es hardcodeado por la mesh del cubo (se tiene en cuenta el centro de la mesh)
    if (checkCollisions(position + velocity * elapsed_time, collisions)) {
        //std::cout << position.x << " " << position.y << " " << position.z << std::endl;
        for (const sCollisionData& collisions : collisions) {
            //Vector3& velocity = velocity;
            Vector3 newDir = velocity.dot(collisions.colNormal) * collisions.colNormal;
            velocity.x -= newDir.x;
            velocity.z -= newDir.z;
        }
    }
    
    position = position + velocity * elapsed_time;
    velocity = velocity - velocity * elapsed_time * 50;
    
    model.setTranslation(position.x, position.y, position.z); // position.y = 51 harcoceado
    if (Input::mouse_position.y < Game::instance->window_height/2){
        model.rotate(yaw, Vector3(0.0f, -1.0f, 0.0f));
    }
    if (Input::mouse_position.y >= Game::instance->window_height/2){
        model.rotate(yaw, Vector3(0.0f, 1.0f, 0.0f));
    }
    //model.rotate(yaw, Vector3(0.0f, 1.0f, 0.0f));

    //camera->lookAt(camera->eye, camera->center, camera->up);
    if  (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
        shoot(model, 50.0f, 0);
    }
}

EntityAI::EntityAI(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture, int hp, float speed, float cdShot, float dispersion) :EntityMesh(model, mesh, shader, texture) {
    this->hp = hp;
    this->maxhp = hp;
    this->speed = speed;
    this->currentBehaviour = NOTHING;
    this->cdShot = cdShot;
    this->dispersion = dispersion;
    this->yaw = 0.f;
    this->shotCdTime = 0.f;
    this->wanderChange = 30.f;
}

void EntityAI::render()
{
    // Get the last camera that was activated
    Camera* camera = Camera::current;

    // Enable shader and pass uniforms
    shader->enable();
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
    Vector3 target = World::get_instance()->player->model.getTranslation();
    Vector3 eye = this->model.frontVector();
    return (eye.dot(target) >= .5f) ? true : false;
}

void EntityAI::behaviourUpdate()
{
    // Si esta bajo de vida intenta huir, si no comprueba si ve al jugador, si le ve entra en ataque si no simplemente se mueve por el mapa
    currentBehaviour = (this->hp / this->maxhp < 0.1) ? RETREAT : ((canSeePlayer()) ? ATTACK : WANDER);
}

int get_random_dir()
{
    static std::default_random_engine e;
    static std::uniform_int_distribution<int> dis(-5, 5); // range [-5, 5]
    return dis(e);
}

void EntityAI::update(float elapsed_time)
{
    wanderChange += elapsed_time;
    Vector3 position = model.getTranslation();
    std::vector <sCollisionData> collisions;
    
    // yaw = degree between player and enemy; acos or asin? but that's inneficient
    behaviourUpdate();
    if (currentBehaviour == ATTACK)
    {
        move_dir = World::get_instance()->player->model.getTranslation() - this->model.getTranslation();
        shotCdTime += elapsed_time;
        if (shotCdTime > cdShot)
        {
            //shoot(model, 50.f, dispersion);
            shotCdTime = 0.f;
        }
        Vector3 toTarget = normalize(World::get_instance()->player->model.getTranslation() - this->model.getTranslation());
        float angle = atan2(toTarget.z, toTarget.x);
        yaw += (angle - yaw) * elapsed_time * 50;
    }
    else if (currentBehaviour == RETREAT)
        move_dir = this->model.getTranslation() - World::get_instance()->player->model.getTranslation();
    else
    {
        move_dir = (wanderChange > 20.f) ? Vector3(get_random_dir(), 0.f, get_random_dir()) : move_dir;
        wanderChange = .0f;
    }

    move_dir.normalize();
    velocity = velocity + move_dir * speed;
    position.y = 51.0f; //el 51 es hardcodeado por la mesh del cubo (se tiene en cuenta el centro de la mesh)
    if (checkCollisions(position + velocity * elapsed_time, collisions)) {
        //std::cout << position.x << " " << position.y << " " << position.z << std::endl;
        for (const sCollisionData& collisions : collisions) {
            //Vector3& velocity = velocity;
            Vector3 newDir = velocity.dot(collisions.colNormal) * collisions.colNormal;
            velocity.x -= newDir.x;
            velocity.z -= newDir.z;
        }
    }
    position = position + velocity * elapsed_time;
    velocity = velocity - velocity * elapsed_time * 50;

    model.setTranslation(position.x, position.y, position.z); // position.y = 51 harcoceado
    model.rotate(yaw, Vector3(0.0f, 1.0f, 0.0f));
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

EntityProjectile::EntityProjectile(Matrix44 model, Mesh* mesh, Shader* shader, Texture* texture, float speed, float dmg, Vector3 dir):EntityCollider(model, mesh, shader, texture){
    this->speed = speed;
    this->dmg = dmg;
    this->dir = dir;
}

void EntityProjectile::render(){
    
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

struct sImpactData {
    Vector3 impPoint;
    Vector3 impNormal;
};

bool checkImpacts(const Vector3& target_pos,
    std::vector<sImpactData>& impacts) {
    Vector3 center = target_pos + Vector3(0.f, 1.25f, 0.f);
    float sphereRadius = .75f;
    Vector3 impPoint, impNormal;

    // For each collider entity “e” in root:
    //for(auto e:World::world->get_instance()->root->children){
    for (int i = 0; i < World::world->get_instance()->root->children.size(); i++) {
        if (EntityCollider* e = dynamic_cast<EntityCollider*>(World::world->get_instance()->root->children[i])) {
            Mesh* mesh = e->mesh;

            if (mesh->testSphereCollision(e->model, center,
                sphereRadius, impPoint, impNormal)) {
                impacts.push_back({ impPoint,
                    impNormal.normalize() });
            }
        }
        // End loop
    }
    return !impacts.empty();
}

void EntityProjectile::update(float elapsed_time){
    
    Vector3 position = model.getTranslation();
    
    dir.normalize();
    velocity = velocity + dir * speed;
    
    

    //Vector3 new_position = model.getTranslation() + velocity * elapsed_time;
    //model.setTranslation(position.x, position.y, position.z);
    //model.setTranslation(new_position.x, new_position.y, new_position.z);
    //velocity = velocity + dir * speed;
    float height = model.getTranslation().y;
    position.y = height;
    
    std::vector<sImpactData> impacts;
    if (checkImpacts(position + velocity * elapsed_time, impacts)) {
        
        World::world->get_instance()->root->removeChild(this);
        
    } else {
        
    }
    position = position + velocity * elapsed_time;
    //position = position + velocity * elapsed_time;
    /*
     Vector3 position = model.getTranslation();
     
     move_dir.normalize();
     velocity = velocity + move_dir * speed;
     
     position = position + velocity * elapsed_time;
     velocity = velocity - velocity * elapsed_time * 50;
     
     model.setTranslation(position.x, 51.0f, position.z); //el 51 es hardcodeado por la mesh del cubo (se tiene en cuenta el centro de la mesh)
     */
    
    
    //Vector3 new_position = model.getTranslation() + velocity * elapsed_time;
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

