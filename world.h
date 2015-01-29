#ifndef WORLD_H
#define WORLD_H

#include <allegro.h>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>

#include <ctime>
#include "entity.h"
#include "screen.h"
#include "npc.h"
#include "enemy.h"
#include "hero.h"
#include "magic.h"
#include "item.h"
#include "world.h"
#include "hero2.h"

using namespace std;

class World{
	int width, height;
	int tile_size;
	vector<BITMAP*> tiles;
	int ***mat;
	coord screen_pos;
	bool eventHandler;
	
	//Campos do mapa
	int next_id;
	map<int,Entity*> entities; //Armazena todas as entidades ativas
	vector<Entity*> visibleEntities;	//Entidades visiveis
	
 public:
	World(char* name, char *tiles_path, int width, int height, int tilesNumber, coord screen = coord(0,0), int tile_size = 32){
		screen_pos.x = screen.x;
		screen_pos.y = screen.y;
		next_id = 0;
		tiles.resize(tilesNumber);
		this->height = height;
		this->width = width;
		this->tile_size = tile_size;
		mat = new int**[4];
		for(int h = 0; h < 4; h++){
			mat[h] = new int*[height];
			for(int i = 0; i < height; i++){
				mat[h][i] = new int[width];
			}
		}
		eventHandler = true;
		/*Load nas tiles*/
		char nome[50];
		for(int i = 0; i < tilesNumber; i++){
			sprintf(nome, "tileset/%s/tile%d.bmp", tiles_path, i);
			tiles[i] = load_bitmap(nome, NULL);
		}
		/*Load no cenario*/
		ifstream file;
		file.open(name);
	  
		for (int h = 0; h < 4; h++)
			for(int i = 0; i < height; i++)
				for(int j = 0; j < width; j++)
					file >> mat[h][i][j];
		file.close();
		
	}
	
	~World(){
		for (int i = 0; i < height; i++)
			delete[] mat[i];
		delete[] mat;
	}

	bool &getEventHandler(){return eventHandler;}
	map<int,Entity*>* getEntities() { return &entities; }
	int **getObstacles() { return mat[2]; }
	int *getNumEntities() { return &next_id; }

	void setScreenPos(coord c) {
		screen_pos.x = c.x;
		screen_pos.y = c.y;
	}
	coord getScreenPos(){ return screen_pos; }

	void insertEntity(Entity* ent) {
		ent->setId(next_id);
		entities[next_id++] = ent;
	}
	
	static bool sortFunction (Entity* i, Entity* j) {
		if(i->getY() + i->getHeight() == j->getY() + j->getHeight()) {
			return (i->getX() + i->getWidth() < j->getX() + j->getWidth());
		}
		else return (i->getY() + i->getHeight() < j->getY() + j->getHeight());
	}
	
	void resetEntities() {
		for(map<int,Entity*>::iterator it = entities.begin(); it!=entities.end(); it++) {
			it->second->reset();
		}
	}
	
	//Conferir!!	
	void selectVisibleEntities(BITMAP* buffer, coord origin, int width, int height) {
		visibleEntities.clear();
		for(map<int,Entity*>::iterator it = entities.begin(); it!=entities.end(); it++) {
			if(it->second->getX() + it->second->getWidth() >= origin.x && it->second->getX() <= origin.x+width && it->second->getY() + it->second->getHeight() >= origin.y && it->second->getY()	<= origin.y+height)
				visibleEntities.push_back(it->second);
		}
	}
	
	//Conferir!!
	void drawEntities(BITMAP* buffer, coord origin) {
		sort(visibleEntities.begin(), visibleEntities.end(), sortFunction);
		for(vector<Entity*>::iterator it = visibleEntities.begin(); it!=visibleEntities.end(); it++) {
			if(((*it)->isAlive())) (*it)->drawSprite(buffer, origin);
		}
	}
	
	void drawDialogs(BITMAP* buffer, coord origin) {
		for(vector<Entity*>::iterator it1 = visibleEntities.begin(); it1!= visibleEntities.end(); it1++) {
			if((*it1)->getClass() == _TalkableNpc) {
				((TalkableNpc*)(*it1))->drawDialog(buffer, origin);
			}
			else if((*it1)->getClass() == _InvestigateNpc) {
				((InvestigateNpc*)(*it1))->drawDialog(buffer, origin);
			}
		}
	}
	
	void handleCollision(){
		for(vector<Entity*>::iterator it1 = visibleEntities.begin(); it1!= visibleEntities.end(); it1++) {
			for(vector<Entity*>::iterator it2 = it1 + 1; it2!=visibleEntities.end(); it2++){
				if((*it1)->isAlive() && (*it2)->isAlive() && ((*it1)->collide(*(*it2), coord(0, 1)) || (*it1)->collide(*(*it2), coord(1, 0)) || (*it1)->collide(*(*it2), coord(-1, 0)) || (*it1)->collide(*(*it2), coord(0, -1)))){
					//Colisao entre Magia e Inimigo
					if((*it1)->getClass() == _Magic && (*it2)->getClass() == _Enemy){
						if(((Magic*)*it1)->getOwner() != _Enemy) {
							if(((Magic*)(*it1))->getType() == ice) ((Enemy*)(*it2))->freeze(35*((Magic*)(*it1))->getLv());
							else if(((Magic*)(*it1))->getType() == fire) ((Enemy*)(*it2))->burn(*(*it1), ((Magic*)(*it1))->getLv());
							((Magic*)*it1)->destroy();
						}
					}
					if((*it2)->getClass() == _Magic && (*it1)->getClass() == _Enemy){
						if(((Magic*)*it2)->getOwner() != _Enemy) {
							if(((Magic*)(*it2))->getType() == ice) ((Enemy*)(*it1))->freeze(35*((Magic*)(*it2))->getLv());
							else if(((Magic*)(*it2))->getType() == fire) ((Enemy*)(*it1))->burn(*(*it2), ((Magic*)(*it2))->getLv());
							((Magic*)*it2)->destroy();
						}
					}
					//Colisao entre heroi e inimigo
					if((*it1)->getClass() == _Hero && (*it2)->getClass() == _Enemy){
						((Hero*)(*it1))->hit(*(*it2), ((Enemy*)(*it2))->getAtkPower());
					}
					if((*it2)->getClass() == _Hero && (*it1)->getClass() == _Enemy){
						((Hero*)(*it2))->hit(*(*it1), ((Enemy*)(*it1))->getAtkPower());
					}
					//Colisao entre heroi e item
					if((*it1)->getClass() == _Hero && (*it2)->getClass() == _Item){
						((Hero*)(*it1))->catchItem(((Item*)(*it2))->getType(), ((Item*)(*it2))->getTtl());
						((Item*)(*it2))->itemGot();
					}
					if((*it2)->getClass() == _Hero && (*it1)->getClass() == _Item){
						((Hero*)(*it2))->catchItem(((Item*)(*it1))->getType(), ((Item*)(*it2))->getTtl());
						((Item*)(*it1))->itemGot();
					}
					//Colisao entre inimigo e inimigo
					if((*it1)->getClass() == _Enemy && (*it2)->getClass() == _Enemy){
						if(((Enemy*)(*it1))->goingBack() && !(((Enemy*)(*it2))->goingBack())){ ((Enemy*)(*it2))->alsogoBack((Enemy&)**it1);}
						else if(((Enemy*)(*it2))->goingBack() && !(((Enemy*)(*it1))->goingBack())){ ((Enemy*)(*it1))->alsogoBack((Enemy&)**it2);}
					}
					//Colisao entre Magia e hero
					if((*it1)->getClass() == _Magic && (*it2)->getClass() == _Hero){
						if(((Magic*)*it1)->getOwner() != _Hero) {
							if(((Magic*)(*it1))->getType() == ice) ((Hero*)(*it2))->freeze(35*((Magic*)(*it1))->getLv());
							if(((Magic*)(*it1))->getType() == fire) ((Hero*)(*it2))->hit(*(*it1), ((Magic*)(*it1))->getLv());
							((Magic*)*it1)->destroy();
						}
					}
					if((*it2)->getClass() == _Magic && (*it1)->getClass() == _Hero){
						if(((Magic*)*it2)->getOwner() != _Hero) {
							if(((Magic*)(*it2))->getType() == ice) ((Hero*)(*it1))->freeze(35*((Magic*)(*it2))->getLv());
							if(((Magic*)(*it2))->getType() == fire) ((Hero*)(*it1))->hit(*(*it2), ((Magic*)(*it2))->getLv());
							((Magic*)*it2)->destroy();
						}
					}
				}
			}
		}
	}
	
	bool inBorder(Entity& ent, Direction dir) {
		switch(dir) {
			case _up:
				if (ent.getY() <= 0) return true; 
				break;
			case _down:
				if(ent.getY() + ent.getHeight() >= height*tile_size-1) return true;
				break;
			case _left:
				if(ent.getX() <= 0) return true;
				break;
			case _right:
				if(ent.getX() + ent.getWidth() >= width*tile_size) return true;
				break;
		}
		return false;
	}
	bool inCell(Entity& ent, int x, int y, Direction dir) {
    switch(dir) {
      case _up:
        if(ent.getY() == y*tile_size && (ent.getX() >= x*tile_size && ent.getX() <= x*tile_size+tile_size)) return true;
        break;
      case _down:
        if(ent.getY() + ent.getHeight() == y*tile_size+tile_size && (ent.getX() >= x*tile_size && ent.getX() <= x*tile_size+tile_size)) return true;
        break;
      case _left:
        if(ent.getX() == x*tile_size && (ent.getY() >= y*tile_size && ent.getY() <= y*tile_size+tile_size)) return true;
        break;
      case _right:
        if(ent.getX() + ent.getWidth() == x*tile_size+tile_size && (ent.getY() >= y*tile_size && ent.getY() <= y*tile_size+tile_size)) return true;
        break;
    }
    return false;
  }
	
	int getWidth() {return width;}
	int getHeight() {return height;}
	int getRealWidth() {return tile_size*width;}
	int getRealHeight() {return tile_size*height;}
	
	int getTileID(int i, int j, int h){
		return mat[h][i][j];
	}
	
	void draw(BITMAP *buffer, coord origin, int view_width, int view_height, int h){
		int end_i, end_j;
		
		view_width /= tile_size;
		view_height /= tile_size;
		
		if (view_height+origin.y/tile_size < height)
			end_i = view_height + 1;
		else
			end_i = view_height;
		
		if (view_width+origin.x/tile_size < width)
			end_j = view_width + 1;
		else
			end_j = view_width;
		
		int rest_i = origin.y%tile_size;
		int rest_j = origin.x%tile_size;
		
		
		origin.y /= tile_size;
		origin.x /= tile_size;		
		for(int i = 0; i < end_i; i++){
			for (int j = 0; j < end_j; j++){
				//printf("%d,%d,%d | %d,%d,%d | %d %d\n", h, i+origin.y, j+origin.x, 4, height, width, end_i, end_j);
				if (i+origin.y < height && j+origin.x < width && mat[h][i+origin.y][j+origin.x] != 0)
					//draw_sprite(buffer, (BITMAP*)tiles[mat[h][i+origin.y][j+origin.x]].dat, j*tile_size - rest_j, i*tile_size - rest_i);
					draw_sprite(buffer, (BITMAP*)tiles[mat[h][i+origin.y][j+origin.x]], j*tile_size - rest_j, i*tile_size - rest_i);
			}
		}
	}
};

#endif
