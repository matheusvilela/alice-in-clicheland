#ifndef ENTITY_H
#define ENTITY_H

#include <allegro.h>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <map>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

enum EntityState {f_parado, f_andando1, f_andando2, d_parado, d_andando1, d_andando2, e_parado, e_andando1, e_andando2, t_parado, t_andando1, t_andando2, f_atk1, f_atk2, f_atk3, d_atk1, d_atk2, d_atk3, e_atk1, e_atk2, e_atk3, t_atk1, t_atk2, t_atk3, f_magia1, f_magia2, f_magia3, d_magia1, d_magia2, d_magia3, e_magia1, e_magia2, e_magia3, t_magia1, t_magia2, t_magia3};
enum Direction {_stop, _up, _down, _right, _left};
enum WichClass {_Entity, _Hero, _Enemy, _Npc, _TalkableNpc, _Magic, _Item, _InvestigateNpc};


int frame_counter, timer;

SAMPLE *swordMiss, *healSound, *swordHitSound, *fireSound, *iceSound, *thunderSound, *absorbSound, *magic_hitSound, *hitSound;

#define talk_delay 50
int key_delay;

typedef struct Coord{
	int x;
	int y;

	Coord(){};
	Coord(int _x, int _y): x(_x), y(_y){};
	bool operator==(const Coord &c) const{
		return (this->x == c.x && this->y == c.y);
	}
	bool operator!=(const Coord &c) const{
		return !(*this == c);
	}
	int distance(Coord &c){
		return max(abs(this->x - c.x), abs(this->y - c.y));
	}
} coord;


class Entity {
 protected:
	static map<int,Entity*> *entities; //Armazena todas as entidades ativas do mapa
	static int **obstacles;
	static int obst_width, obst_height;
	static int *next_id;
	

	int id;
	WichClass name;
	int width, height;
	int tira_h, tira_v;
	coord position;
	coord initial_position;
	DATAFILE *sprite;
	EntityState animationState;
	bool blocked;
	int velocity;
	
 public:
	Entity(char* path, int width, int height, int x = 0, int y = 0, int h = 0, int v = 0, int vel = 1, EntityState es = f_parado) { //Construtor
		sprite = load_datafile(path);
		this->width = width;
		this->height = height;
		position.x = initial_position.x = x;
		position.y = initial_position.y = y;
		tira_h = h;
		tira_v = v;
		animationState = es;
		blocked = false;
		velocity = vel;
		name = _Entity;
	}

	virtual ~Entity() { //Destrutor
		unload_datafile(sprite);
		entities->erase(id);
	}

	
	void insertWorld() {
		id = (*next_id)++;
		(*entities)[id] = this;
	}
	
	void setId(int id) {
		this->id = id;
	}
	
	virtual void reset() {
		setPosition(initial_position);
	}
	coord getInitialPos(){return initial_position;}
	
	int getX() {return position.x;}
	int getY() {return position.y;}
	void setPosition(coord p) {
		position.x = p.x;
		position.y = p.y;
	}
	
	static void setNewMap(map<int,Entity*>* ent, int** obst, int w, int h, int* next) { entities = ent; obstacles = obst; obst_width = w; obst_height = h; next_id = next;}
	
	WichClass getClass() {return name;}
	coord getPosition() const { return position; }
	int getHeight() const { return height; }
	int getWidth() const { return width; }
	void block(){ blocked = true; }
	void unblock(){ blocked = false; }
	int distance(Entity &e){
		//distancia diagonal.
//		return max(abs(this->position.x - e.position.x), abs(this->position.y - e.position.y));
		return max(abs((this->position.x+this->width/2) - (e.position.x+e.width/2)), abs((this->position.y+this->height/2) - (e.position.y+e.height/2)));
	}
	
 	int distanceX(Entity &e){
		return abs((this->position.x+this->width/2) - (e.position.x+e.width/2));
	}
	int distanceY(Entity &e){
		return abs((this->position.y+this->height/2) - (e.position.y+e.height/2));
	}
	
	virtual bool isAlive(){return true;}
	
	bool updatePosition(int var_x, int var_y) { //Atualiza a posicao
		//Verifica colisoes com todas as outras entidades
		for(map<int, Entity*>::iterator it=entities->begin(); it!=entities->end(); it++) {
			if(isAlive() && it->second != this && it->second->isAlive() && /**/ it->second->getClass() != _Item /**/&& (it->second->getClass() != _Hero || getClass()!= _Hero) && collide(*(it->second), coord(var_x, var_y))) {
				return false;
			}
		}
		if(((position.y+var_y)/32 >= 0 && (position.y+var_y)/32 < obst_height && (position.x+var_x)/32 >= 0 && (position.x+var_x)/32 < obst_width && obstacles[(position.y+var_y)/32][(position.x+var_x)/32]) ||
			 ((position.y+var_y)/32 >= 0 && (position.y+var_y)/32 < obst_height && (position.x+var_x+width)/32 >= 0 && (position.x+var_x+width)/32 < obst_width && obstacles[(position.y+var_y)/32][(position.x+var_x+width)/32]) ||
			 ((position.y+var_y+height)/32 >= 0 && (position.y+var_y+height)/32 < obst_height && (position.x+var_x)/32 >= 0 && (position.x+var_x)/32 < obst_width && obstacles[(position.y+height+var_y)/32][(position.x+var_x)/32]) ||
			 ((position.y+var_y+height)/32 >= 0 && (position.y+var_y+height)/32 < obst_height && (position.x+var_x+width)/32 >= 0 && (position.x+var_x+width)/32 < obst_width && obstacles[(position.y+var_y+height)/32][(position.x+var_x+width)/32])) return false;
		position.x += var_x;
		position.y += var_y;
		return true;
	}

	void updateState (EntityState animationState) { //Atualiza o estado
		this->animationState = animationState;
	}

	EntityState getState() const{ return animationState; }

	bool collide(Entity& ent, coord p) {
		if(position.x + width + p.x >=	ent.position.x + ent.width && position.y + height + p.y >= ent.position.y + ent.height) {
			if(ent.position.x+ent.width >= position.x + p.x && ent.position.y+ent.height-height/2 >= position.y + p.y) return true;
			else return false;
		} else if(position.x+width+p.x >= ent.position.x + ent.width && position.y + height + p.y <= ent.position.y + ent.height) {
			if(ent.position.x+ent.width >= position.x + p.x && ent.position.y-(height-ent.height/2) <= position.y + p.y) return true;
			else return false;
		} else if(position.x + width + p.x <=	ent.position.x + ent.width && position.y + height + p.y >= ent.position.y + ent.height) {
			if(ent.position.x-width <= position.x + p.x && ent.position.y+ent.height-height/2 >= position.y + p.y) return true;
			else return false;		
		} else {
			if(ent.position.x-width <= position.x + p.x && ent.position.y-(height-ent.height/2) <= position.y + p.y) return true;
			else return false;				
		}
	}

		Direction backDirection(Entity& ent){
		if(collide(ent, coord(0, 1))) return _up;
		else if(collide(ent, coord(1, 0))) return _left;
		else if(collide(ent, coord(-1, 0))) return _right;
		else if(collide(ent, coord(0, -1))) return _down;
	}

	virtual void drawSprite(BITMAP* buffer, coord origin) { //Desenha o sprite
		draw_sprite(buffer, (BITMAP*)sprite[animationState].dat, position.x-tira_h-origin.x, position.y-tira_v-origin.y);
	}
		
	static void doControl(){
		for(map<int, Entity*>::iterator it=entities->begin(); it!=entities->end(); it++) {
			if(timer%(it->second->velocity) == 0) it->second->controlMe();
		}
	}
		
	/*Faz a animacao de um personagem andando*/
	void move(Direction dir){
		EntityState es;
		switch(dir) {
		 case _stop:
				es = getState();
				if(es == f_andando1 || es == f_andando2 || es == f_atk3 || es == f_magia3) updateState(f_parado);
				else if(es == d_andando1 || es == d_andando2 || es == d_atk3 || es == d_magia3) updateState(d_parado);
				else if(es == e_andando1 || es == e_andando2 || es == e_atk3 || es == e_magia3) updateState(e_parado);
				else if(es == t_andando1 || es == t_andando2 || es == t_atk3 || es == t_magia3) updateState(t_parado);
				break;
			case _up:
				if(frame_counter%2 == 0) updateState(t_andando1);
				else updateState(t_andando2);
				break;
			case _down:
				if(frame_counter%2 == 0) updateState(f_andando1);
				else updateState(f_andando2);
				break;
			case _right:
				if(frame_counter%2 == 0) updateState(d_andando1);
				else updateState(d_andando2);
				break;
			case _left:
				if(frame_counter%2 == 0) updateState(e_andando1);
				else updateState(e_andando2);
				break;
				
		}
	}

	virtual void controlMe(){}
	
};
map<int,Entity*>* Entity::entities;
int **Entity::obstacles;
int *Entity::next_id;
int Entity::obst_width;
int Entity::obst_height;
#endif
