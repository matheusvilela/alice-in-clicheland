#ifndef MAGIC_H
#define MAGIC_H
#include "entity.h"

enum MagicType {fire, ice, heal, thunder, invisibility};

class Magic : public Entity {
private:
	WichClass owner;
	MagicType type;
	Direction dir;
	int ttl;
	bool fade;
	int level;
	bool atk_control, atk2_control;
	
	
public: 
	Magic(MagicType type, char* path, int width, int height, int x, int y, Direction dir, int level = 1, WichClass owner = _Hero, int ttl = 70, int h = 0, int v = 0, int vel = 1) : Entity(path, width, height, x, y, h, v, vel) {
		this->type = type;
		this->dir = dir;
		this->ttl = ttl;
		this->level = level;
		this->owner = owner;
		fade = false;
		name = _Magic;
		atk_control = atk2_control = false;		
	}
	
	~Magic(){ }
	
	int getLv() {return level;}
	WichClass getOwner() {return owner;}
	int getTtl() const { return ttl; }
	bool getFade() const { return fade; }
	MagicType getType() {return type;}
	
	void destroy() { if(ttl > 5) ttl = 5; fade = true;}
	void updatePosition() {
		coord c(0, 0);
		ttl--;
		if(!fade) {
			switch (dir) {
				case _down:
					position.y += 1;
					break;
				case _right:
					position.x += 1;
					break;
				case _left:
					position.x -= 1;
					break;
				case _up:
					position.y -= 1;
					break;
				default:
					allegro_message("You should not be here.");
			}
			//Verifica colisoes com todas as outras entidades
			for(map<int, Entity*>::iterator it=entities->begin(); it!=entities->end(); it++) {
				if(isAlive() && it->second != this && it->second->getClass() != owner && /**/ it->second->getClass() != _Item /**/ && it->second->isAlive() && collide(*(it->second), c)) {
					ttl = 5;
				}
			}
			if((position.y/32 >= 0 && position.y/32 < obst_height && position.x/32 >= 0 && position.x/32 < obst_width && obstacles[(position.y)/32][(position.x)/32]) ||
				 (position.y/32 >= 0 && position.y/32 < obst_height && (position.x+width)/32 >= 0 &&(position.x+width)/32 < obst_width && obstacles[(position.y)/32][(position.x+width)/32]) ||
				 ((position.y+height)/32 >= 0 && (position.y+height)/32 < obst_height && position.x/32 >= 0 && position.x/32 < obst_width && obstacles[(position.y+height)/32][(position.x)/32]) ||
				 ((position.y+height)/32 >= 0 && (position.y+height)/32 < obst_height && (position.x+width)/32 >= 0 && (position.x+width)/32 < obst_width && obstacles[(position.y+height)/32][(position.x+width)/32])) ttl = 5;
			if(ttl == 5) fade = true;
		}
	}
	void controlMe(){
		updatePosition();
		detectAtk();
		if(timer%5 == 0 && !getFade()) updateState((EntityState)((getState()+1)%2));
		else if(getFade()) updateState(f_andando2);
	}
	
	void detectAtk() {
		Entity *heroi = (*entities)[0];
		Entity *heroi2 = (*entities)[1];
		coord hero_coord = heroi->getPosition();
		coord hero2_coord = heroi2->getPosition();
		if(key[KEY_A] && !atk_control){
			switch(heroi->getState()){
				case f_atk1:
				case f_atk2:
				case f_atk3:
				case f_parado:
				case f_andando1:
				case f_andando2:
					if((position.x >= (hero_coord.x - 11 - 20) && (position.x + width) <= (hero_coord.x - 11 + 30 + 20)) && (position.y >= hero_coord.y && position.y <= hero_coord.y + heroi->getHeight() + 16)){
						play_sample(absorbSound ,100, 128, 1000, 0);
						destroy();
					}
					break;
				case d_atk1:
				case d_atk2:
				case d_atk3:
				case d_parado:
				case d_andando1:
				case d_andando2:
					if(position.y >= (hero_coord.y - 20) && (position.y + height) <= (hero_coord.y + heroi->getHeight() + 20) && (position.x >= hero_coord.x - 11 && position.x <= hero_coord.x - 11 + 30 + 16)){
						play_sample(absorbSound ,100, 128, 1000, 0);
						destroy();
					}
					break;
				case e_atk1:
				case e_atk2:
				case e_atk3:
				case e_parado:
				case e_andando1:
				case e_andando2:
					if(position.y >= (hero_coord.y - 20) && (position.y + height) <= (hero_coord.y + heroi->getHeight() + 20) && (position.x + width <= hero_coord.x	&& position.x + width >= hero_coord.x - 11 - 16)){
						play_sample(absorbSound ,100, 128, 1000, 0);
						destroy();
					}
					break;
				case t_atk1:
				case t_atk2:
				case t_atk3:
				case t_parado:
				case t_andando1:
				case t_andando2:
					if((position.x >= (hero_coord.x - 11 - 20) && (position.x + width) <= (hero_coord.x - 11 + 30 + 20)) && (position.y <= hero_coord.y && position.y >= hero_coord.y - heroi->getHeight() - 14)){
						play_sample(absorbSound ,100, 128, 1000, 0);
						destroy();
					};
					break;
			}
			atk_control = true;
		}
		if(key[KEY_D] && heroi2->isAlive() && !atk2_control){
			switch(heroi2->getState()){
				case f_atk1:
				case f_atk2:
				case f_atk3:
				case f_parado:
				case f_andando1:
				case f_andando2:					
					if((position.x >= (hero2_coord.x - 11 - 20) && (position.x + width) <= (hero2_coord.x - 11 + 30 + 20)) && (position.y >= hero2_coord.y && position.y <= hero2_coord.y + heroi2->getHeight() + 16)){
						play_sample(absorbSound ,100, 128, 1000, 0);
						destroy();
					}
					break;
				case d_atk1:
				case d_atk2:
				case d_atk3:
				case d_parado:
				case d_andando1:
				case d_andando2:					
					if(position.y >= (hero2_coord.y - 20) && (position.y + height) <= (hero2_coord.y + heroi2->getHeight() + 20) && (position.x >= hero2_coord.x - 11 && position.x <= hero2_coord.x - 11 + 30 + 16)){
						play_sample(absorbSound ,100, 128, 1000, 0);
						destroy();
					}
					break;
				case e_atk1:
				case e_atk2:
				case e_atk3:
				case e_parado:
				case e_andando1:
				case e_andando2:					
					if(position.y >= (hero2_coord.y - 20) && (position.y + height) <= (hero2_coord.y + heroi2->getHeight() + 20) && (position.x + width <= hero2_coord.x	&& position.x + width >= hero2_coord.x - 11 - 16)){
						play_sample(absorbSound ,100, 128, 1000, 0);
						destroy();
					}
					break;
				case t_atk1:
				case t_atk2:
				case t_atk3:
				case t_parado:
				case t_andando1:
				case t_andando2:					
					if((position.x >= (hero2_coord.x - 11 - 20) && (position.x + width) <= (hero2_coord.x - 11 + 30 + 20)) && (position.y <= hero2_coord.y && position.y >= hero2_coord.y - heroi2->getHeight() - 14)){
						play_sample(absorbSound ,100, 128, 1000, 0);
						destroy();
					}
					break;
			}
			atk2_control = true;
		}
		if(!key[KEY_D] && atk2_control) atk2_control = false;
		if(!key[KEY_A] && atk_control) atk_control = false;
	}
	
};
#endif
