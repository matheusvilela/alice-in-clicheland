#ifndef GRID_H
#define GRID_H
#include "entity.h"
#include "hero.h"
#include "npc.h"
#include "world.h"

class Screen{

 private:
	coord position;//upper left da screen
	int width, height;
	World *world;
	BITMAP *buffer;
	
 public:
	Screen(World &world, int width = 640, int height = 480){
		position.x = world.getScreenPos().x;
		position.y = world.getScreenPos().y;
		this->world = &world;
		this->width = width;
		this->height = height;
		Entity::setNewMap(world.getEntities(), world.getObstacles(), world.getWidth(), world.getHeight(), world.getNumEntities());
		buffer = create_bitmap(width, height);
	}
	
	~Screen(){};
	
	void setWorld(World &world) {
		//Armazena a ScreenPosition do world atual
		//Reseta as posicoes e bloodpoints das entidades
		this->world->setScreenPos(position);
		this->world->resetEntities();
		
		//Seta o novo mundo para a Screen
		this->world = &world;
		position.x = this->world->getScreenPos().x;
		position.y = this->world->getScreenPos().y;
		Entity::setNewMap(world.getEntities(), world.getObstacles(), world.getWidth(), world.getHeight(), world.getNumEntities());
	}
	World* getWorld(){ return world; }
	void clearBuffer(){
		clear_bitmap(buffer);
	}
	
	void buffer2screen(){
		blit(buffer, screen, 0, 0, 0, 0,width,height); //Desenha o buffer na tela
	}
	
	void updatePosition(Entity &character){
		if (position.x >= 0 && position.x + width <= world->getRealWidth() && character.getPosition().x >= width/2 && character.getPosition().x + width/2 <= world->getRealWidth())
			position.x = character.getPosition().x - width/2;
		if (position.y >= 0 && position.y + height <= world->getRealHeight() && character.getPosition().y >= height/2 && character.getPosition().y + height/2 <= world->getRealHeight())
			position.y = character.getPosition().y - height/2;
	}
	
	void selectVisible(){
		world->selectVisibleEntities(buffer, position, width, height);
	}
	
	void drawInterface(Hero *heroi, Hero2 *heroi2, bool multiplayer){
		heroi->drawStatus(buffer, position);
		if(multiplayer) heroi2->drawStatus(buffer, position);
	}
	
	void draw(Hero *heroi, Hero2 *heroi2, bool multiplayer){
		world->draw(buffer, position, width, height, 0);
		world->draw(buffer, position, width, height, 1);
		world->draw(buffer, position, width, height, 2);
		world->drawEntities(buffer, position);
		world->draw(buffer, position, width, height, 3);
		drawInterface(heroi, heroi2, multiplayer);
		world->drawDialogs(buffer, position);
	}
};
#endif
