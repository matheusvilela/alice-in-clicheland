#ifndef ENEMY_H
#define ENEMY_H
#include "npc.h"
#include "hero.h"
#include "hero2.h"
#include "item.h"

class Enemy : public Npc {
 protected:
	int blood_points, total_blood_points;
	int atk_power;
	bool atk_control;
	bool mgk_control;
	bool atk2_control;
	bool mgk2_control;

	Direction isHit;
	int go_back;
	 
	bool frozen;
	int freeze_ttl;
	 
	bool fade;
	int fade_time;
	
	Item *drop;
	bool isDrop;

	Magic *inUse;
	int tipoIA;
	
 public:
	Enemy(char* path, int width, int height, int x = 0, int y = 0, int IAType = 0, int blood_points = 3, int vel = 3, NpcState npc_state = halt, int atk = 1, int h = 0, int v = 0) : Npc (path, width, height, x, y, vel, npc_state, h, v) {
		this->blood_points = total_blood_points = blood_points;
		atk_power = atk;
		atk_control = false;
		mgk_control = false;
		atk2_control = false;
		mgk2_control = false;
		isHit = _stop;
		go_back = 0;
		frozen = false;
		freeze_ttl = 0;
		name = _Enemy;
		fade = false;
		isDrop = false;
		inUse = NULL;
		tipoIA = IAType;
	}
	~Enemy() {}

	void reset() {
		setPosition(initial_position);
		blood_points = total_blood_points;
    if(inUse != NULL) {
      delete inUse;
      inUse = NULL;
    }
	}	
	
	int getAtkPower(){return atk_power;}

	bool goingBack(){if(go_back > 0) return true; else return false;}

	int getgoback(){return go_back;}

	void alsogoBack(Enemy& ent){
		isHit = backDirection(ent);
		go_back = ent.getgoback();
	}

	bool isAlive() {
		if(blood_points <= 0 && !fade) return false;
		else return true;
	}
		
	void dropItem(){
		int x = rand()%10 + 1;
		if(x <= 4){
			isDrop = true;
			drop = new Item(f_andando2, (char*)"sprites/recItens.dat", 13, 12, xp, 200, getPosition().x+width/2, getPosition().y+height/2);
			drop->insertWorld();
		}
		else if(x <= 7){
			isDrop = true;
			drop = new Item(f_parado, (char*)"sprites/recItens.dat", 13, 12, blood_rec, 200, getPosition().x+width/2, getPosition().y+height/2);
			drop->insertWorld();
		}
		else{
			isDrop = true;
			drop = new Item(f_andando1, (char*)"sprites/recItens.dat", 13, 12, energy_rec, 200, getPosition().x+width/2, getPosition().y+height/2);
			drop->insertWorld();
		}
	}
	
	void freeze(int time){
		block();
		freeze_ttl = time;
		frozen = true;
	}
	
	void burn(Entity& ent, int atk){
		if(go_back == 0){
			isHit = backDirection(ent);
			hit(atk);
			if(blood_points > 0) go_back = 5;
		}
	}	
	
	void hit(int atk){
		blood_points -= atk;
		if(blood_points <= 0){
			fade = true;
			fade_time = 10;
			updateState(f_atk1);
		}
	}
	
	void detectAtk(){
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
						isHit = _down;
						hit(((Hero*)(heroi))->getAtkPower());
						play_sample(swordHitSound ,100, 128, 1000, 0);
					}
					break;
				case d_atk1:
				case d_atk2:
				case d_atk3:
				case d_parado:
				case d_andando1:
				case d_andando2:
					if(position.y >= (hero_coord.y - 20) && (position.y + height) <= (hero_coord.y + heroi->getHeight() + 20) && (position.x >= hero_coord.x - 11 && position.x <= hero_coord.x - 11 + 30 + 16)){
						isHit = _right;
						hit(((Hero*)(heroi))->getAtkPower());
						play_sample(swordHitSound ,100, 128, 1000, 0);
					}
					break;
				case e_atk1:
				case e_atk2:
				case e_atk3:
				case e_parado:
				case e_andando1:
				case e_andando2:
					if(position.y >= (hero_coord.y - 20) && (position.y + height) <= (hero_coord.y + heroi->getHeight() + 20) && (position.x + width <= hero_coord.x	&& position.x + width >= hero_coord.x - 11 - 16)){
						isHit = _left;
						hit(((Hero*)(heroi))->getAtkPower());
						play_sample(swordHitSound ,100, 128, 1000, 0);
					}
					break;
				case t_atk1:
				case t_atk2:
				case t_atk3:
				case t_parado:
				case t_andando1:
				case t_andando2:
					if((position.x >= (hero_coord.x - 11 - 20) && (position.x + width) <= (hero_coord.x - 11 + 30 + 20)) && (position.y <= hero_coord.y && position.y >= hero_coord.y - heroi->getHeight() - 14)){
						isHit = _up;
						hit(((Hero*)(heroi))->getAtkPower());
						play_sample(swordHitSound ,100, 128, 1000, 0);
					};
					break;
			}
			atk_control = true;
		}	else if(key[KEY_S] && !mgk_control && ((Hero*)(heroi))->getCurrent() == thunder && ((Hero*)heroi)->isAtk()){
		  if(distance(*heroi) < 60){
		    switch(getState()){
		      case f_parado:
		      case f_andando1:
		      case f_andando2:
		        isHit = _up;
		        hit(((Hero*)(heroi))->getThunderLvl());
		        break;
		      case d_parado: 
		      case d_andando1:
		      case d_andando2:
		        isHit = _left;
		        hit(((Hero*)(heroi))->getThunderLvl());
		        break;		      
		      case e_parado:
		      case e_andando1:
		      case e_andando2:
		        isHit = _right;
		        hit(((Hero*)(heroi))->getThunderLvl());
		        break;		      
		      case t_parado:
		      case t_andando1:
		      case t_andando2:
		        isHit = _down;
		        hit(((Hero*)(heroi))->getThunderLvl());
		        break;
		    }
		    mgk_control = true;
		  }
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
						isHit = _down;
						hit(((Hero2*)(heroi2))->getAtkPower());
						play_sample(swordHitSound ,100, 128, 1000, 0);
					}
					break;
				case d_atk1:
				case d_atk2:
				case d_atk3:
				case d_parado:
				case d_andando1:
				case d_andando2:					
					if(position.y >= (hero2_coord.y - 20) && (position.y + height) <= (hero2_coord.y + heroi2->getHeight() + 20) && (position.x >= hero2_coord.x - 11 && position.x <= hero2_coord.x - 11 + 30 + 16)){
						isHit = _right;
						hit(((Hero2*)(heroi2))->getAtkPower());
						play_sample(swordHitSound ,100, 128, 1000, 0);
					}
					break;
				case e_atk1:
				case e_atk2:
				case e_atk3:
				case e_parado:
				case e_andando1:
				case e_andando2:					
					if(position.y >= (hero2_coord.y - 20) && (position.y + height) <= (hero2_coord.y + heroi2->getHeight() + 20) && (position.x + width <= hero2_coord.x	&& position.x + width >= hero2_coord.x - 11 - 16)){
						isHit = _left;
						hit(((Hero2*)(heroi2))->getAtkPower());
						play_sample(swordHitSound ,100, 128, 1000, 0);
					}
					break;
				case t_atk1:
				case t_atk2:
				case t_atk3:
				case t_parado:
				case t_andando1:
				case t_andando2:					
					if((position.x >= (hero2_coord.x - 11 - 20) && (position.x + width) <= (hero2_coord.x - 11 + 30 + 20)) && (position.y <= hero2_coord.y && position.y >= hero2_coord.y - heroi2->getHeight() - 14)){
						isHit = _up;
						hit(((Hero2*)(heroi2))->getAtkPower());
						play_sample(swordHitSound ,100, 128, 1000, 0);
					}
					break;
			}
			atk2_control = true;
		} else if(key[KEY_F] && heroi2->isAlive() && !mgk2_control && ((Hero2*)(heroi2))->getCurrent() == thunder && ((Hero2*)heroi2)->isAtk()){
			if(distance(*heroi2) < 60){
				switch(getState()){
					case f_parado:
					case f_andando1:
					case f_andando2:
						isHit = _up;
						hit(((Hero2*)(heroi2))->getThunderLvl());
						break;
					case d_parado: 
					case d_andando1:
					case d_andando2:
						isHit = _left;
						hit(((Hero2*)(heroi2))->getThunderLvl());
						break;		      
					case e_parado:
					case e_andando1:
					case e_andando2:
						isHit = _right;
						hit(((Hero2*)(heroi2))->getThunderLvl());
						break;		      
					case t_parado:
					case t_andando1:
					case t_andando2:
						isHit = _down;
						hit(((Hero2*)(heroi2))->getThunderLvl());
						break;
				}
				mgk2_control = true;
			}
		}
		
		if(!key[KEY_D] && atk2_control) atk2_control = false;
		if(!key[KEY_F] && mgk2_control) mgk2_control = false;
		
		if(!key[KEY_A] && atk_control) atk_control = false;
		if(!key[KEY_S] && mgk_control) mgk_control = false;
	 
		if(isHit != _stop && !((Hero*)heroi)->isAtk() && blood_points > 0){
			go_back = 10;
		}		
	}
	
	/*Segue herois e corre quando esta fraco*/
	void int1(){
		Hero* alice = (Hero*)(*entities)[0];
		Hero2* ecila = (Hero2*)(*entities)[1];
		if(distance(*alice) > 300 && (distance(*ecila) > 300 || !ecila->isAlive())) wandering();
		else if((distance(*alice) < 170 || (ecila->isAlive() && distance(*ecila) < 170)) && blood_points > 1) {
			if(distance(*ecila) < distance(*alice) && ecila->isAlive()) follow(*ecila);
			else follow(*alice); 			
		}
		else if(blood_points == 1 && (distance(*alice) < 170 || (ecila->isAlive() && distance(*ecila) < 170))) {
			if(distance(*alice) > distance(*ecila) && ecila->isAlive()) escape(*ecila);
			else escape(*alice); 
		}
	}
	
	/*Segue herois jogando magias*/
	void int2(){
		Hero* alice = (Hero*)(*entities)[0];
		Hero2* ecila = (Hero2*)(*entities)[1];
		int prob = rand()%100;
		if(distance(*alice) > 300 && (distance(*ecila) > 300 || !ecila->isAlive())) wandering();
		else if(distance(*alice) < 170 || (ecila->isAlive() && distance(*ecila) < 170)){
			if(distance(*ecila) < distance(*alice) && ecila->isAlive()) follow(*ecila);
			else follow(*alice); 
			if(inUse == NULL && prob <=1){
        play_sample(fireSound , 50, 128, 1000, 0);
				if((animationState >= f_parado && animationState <= f_andando2)){ inUse = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x + 5, getPosition().y + getHeight(), _down, 1, name, 200); inUse->insertWorld();}
				else if((animationState >= d_parado && animationState <= d_andando2)){ inUse = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x + 33, getPosition().y + getHeight()/2 - 5, _right, 1, name, 200); inUse->insertWorld();}
				else if((animationState >= e_parado && animationState <= e_andando2)){ inUse = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x - 16, getPosition().y + getHeight()/2 - 5, _left, 1, name, 200); inUse->insertWorld();}
				else if((animationState >= t_parado && animationState <= t_andando2)){ inUse = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x+ 30 -15 - 5, getPosition().y-1, _up, 1, name, 200); inUse->insertWorld();}
			}
		}
	}
	
	void controlMe(){
		if(go_back == 0){
			if(blood_points > 0){
				Npc::controlMe();
				detectAtk();
				/*IA*/
				if(!blocked){
					switch(tipoIA){
						case 1: int1(); break;
						case 2: int2(); break;
						default: break;
					}
				}
			}
		}
		else{
			switch(isHit){
				case _down: updatePosition(0, 5); break;
				case _right: updatePosition(5, 0); break;
				case _left: updatePosition(-5, 0); break;
				case _up: updatePosition(0, -5); break;
			}
			go_back--;
			if(go_back == 0) isHit = _stop;
		}

		/*Deleta magia do inimigo*/
		if(inUse!=NULL && inUse->getTtl() <= 0) {
			delete inUse;
			inUse = NULL;
		}
		
		/*Trata Freeze*/
		if (frozen) freeze_ttl--;
		if(freeze_ttl == 0) unblock();
		
		/*Enemy Fade*/
		if(fade){
			fade_time--;
			if(fade_time <= 0){
				fade = false;
				dropItem();
			}
		}
		/*Dropa item*/
		if(isDrop){
			drop->updateTtl();
			if(drop->getTtl() <= 0){isDrop = false; delete drop;}
		}
	}

};
#endif
