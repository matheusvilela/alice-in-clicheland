#ifndef HERO_H
#define HERO_H
#include "entity.h"
#include "magic.h"
#include "item.h"

class Hero : public Entity {
 private:
	int blood_points, total_blood_points;
	int energy, total_energy;
	int stamina, total_stamina;
	bool tired;
	int atk_power;
	
	map <MagicType, bool> currentMagics;
	MagicType current; 
	int magicLv[5][2];
	int total_xp;
	
	bool atk, begin_atk;
	int action;//Acao da heroina: atacar, soltar magia
	int atk_counter;
	Magic *inUse;
	bool depress_key;
	bool depress_key_magic;
	int go_back;
	Direction go_back_dir;
	
	bool frozen;
	int freeze_ttl;

	DATAFILE *thunderSprite;
	DATAFILE *healSprite;
	DATAFILE *hearts;
	DATAFILE *magics;
	
 public:
	Hero(char* path, int width, int height, int x = 0, int y = 0, int h = 0, int v = 0, int vel = 2) : Entity (path, width, height, x, y, h, v, vel) {
		hearts = load_datafile("sprites/heartcontainer.dat");
		magics = load_datafile("sprites/magics.dat");
		healSprite = load_datafile("sprites/heal.dat");
		thunderSprite = load_datafile("sprites/raio.dat");
		for(int i = 0; i < 5; i++){
			for(int j = 0; j < 2; j++){
				if(j == 0) magicLv[i][j] = 1;
				else magicLv[i][j] = 0;
			}
		}
		current = fire;
		atk = false;
		begin_atk = false;
		action = 0;
		atk_counter = 0;
		depress_key = false;
		depress_key_magic = false;
		atk_power = 1;
		
		go_back = 0;
		go_back_dir = _left;
		blood_points = 10;
		total_blood_points = 10;
		energy = 20;
		total_energy = 20;
		//stamina = 2000;
		//total_stamina = 2000;
		stamina = 20000;
		total_stamina = 20000;
		tired = false;
		total_xp = 30;
		name = _Hero;
		//Enable em todas as magias
		for(int i = 0; i < 5; i++) enableMagic((MagicType)i);
		inUse = NULL;
		frozen = false;
		freeze_ttl = 0;
	}
	~Hero() {}
	 
	 /*Nuh, essa doeu =(*/
	int getThunderLvl(){return magicLv[4][0];}
	
	int getAtkPower(){return atk_power;}
	
	bool isAtk(){return atk;}
	
	bool isAlive() {
		if(blood_points <= 0) return false;
		else return true;
	}
	bool isFrozen(){
		return frozen;
	}
	void totalHeal() { blood_points = total_blood_points; energy = total_energy; stamina = total_stamina; }
	
	void freeze(int time){
		if(!frozen){
			block();
			freeze_ttl = time;
			frozen = true;
		}
	}

	MagicType getCurrent(){return current;}
	
	void enableMagic(MagicType m) {
		currentMagics[m] = true;
	}
	
	void animateAtk() {
		begin_atk = true;
		if(atk_counter%4 == 3) {atk = false; begin_atk = false;} //Ataque terminou
		else {
			if((animationState >= f_parado && animationState <= f_andando2) || animationState == f_magia3 || (animationState >= f_atk1 && animationState <= f_atk3)) updateState((EntityState)(f_atk1+(atk_counter%4)));
			else if((animationState >= d_parado && animationState <= d_andando2) || animationState == d_magia3 || (animationState >= d_atk1 && animationState <= d_atk3)) updateState((EntityState)(d_atk1+(atk_counter%4)));
			else if((animationState >= e_parado && animationState <= e_andando2) || animationState == e_magia3 || (animationState >= e_atk1 && animationState <= e_atk3)) updateState((EntityState)(e_atk1+(atk_counter%4)));
			else if((animationState >= t_parado && animationState <= t_andando2) || animationState == t_magia3 || (animationState >= t_atk1 && animationState <= t_atk3)) updateState((EntityState)(t_atk1+(atk_counter%4)));			
		}
	}
	
	void animateMagic(MagicType m) {
		if(currentMagics.find(m) != currentMagics.end()) {
			begin_atk = true;
			if(atk_counter%4 == 3) { atk = false; begin_atk = false;}
			else {
				if((animationState >= f_parado && animationState <= f_andando2) || (animationState >= f_magia1 && animationState <= f_magia3) || animationState == f_atk3) updateState((EntityState)(f_magia1+(atk_counter%4)));
				else if((animationState >= d_parado && animationState <= d_andando2) || (animationState >= d_magia1 && animationState <= d_magia3) || animationState == d_atk3) updateState((EntityState)(d_magia1+(atk_counter%4)));
				else if((animationState >= e_parado && animationState <= e_andando2) || (animationState >= e_magia1 && animationState <= e_magia3) || animationState == e_atk3) updateState((EntityState)(e_magia1+(atk_counter%4)));
				else if((animationState >= t_parado && animationState <= t_andando2) || (animationState >= t_magia1 && animationState <= t_magia3) || animationState == t_atk3) updateState((EntityState)(t_magia1+(atk_counter%4)));			
			}
		} else cout << "Vc nao possui essa magia." << endl;
	}
	
	void createMagic(MagicType m) {
		char* path;
		if(m == ice) path = (char*)"sprites/gelo.dat";
		else if(m == fire) path = (char*)"sprites/fogo.dat";
		if(m == ice || m == fire){
			if(currentMagics.find(m)!=currentMagics.end()) {
				if((animationState >= f_parado && animationState <= f_andando2) || animationState == f_magia3 || animationState == f_atk3){ inUse = new Magic(current, (char*)path, 15, 16, getPosition().x + 5 - 11, getPosition().y + getHeight(), _down, magicLv[current][0], name); inUse->insertWorld();}
				else if((animationState >= d_parado && animationState <= d_andando2) || animationState == d_magia3 || animationState == d_atk3){ inUse = new Magic(current, (char*)path, 15, 16, getPosition().x+1 + 30 - 11, getPosition().y + getHeight()/2 - 5, _right, magicLv[current][0], name); inUse->insertWorld();}
				else if((animationState >= e_parado && animationState <= e_andando2) || animationState == e_magia3 || animationState == e_atk3){ inUse = new Magic(current, (char*)path, 15, 16, getPosition().x - 16 - 11, getPosition().y + getHeight()/2 - 5, _left, magicLv[current][0], name); inUse->insertWorld();}
				else if((animationState >= t_parado && animationState <= t_andando2) || animationState == t_magia3 || animationState == t_atk3){ inUse = new Magic(current, (char*)path, 15, 16, getPosition().x+ 30 -15 - 5 - 11, getPosition().y-1, _up, magicLv[current][0], name); inUse->insertWorld();}
			} else cout << "Vc nao possui essa magia." << endl;
		}
	}
	
	void hit(Entity& ent, int atk){
		if(go_back == 0){
      play_sample(hitSound ,100, 128, 1000, 0);
			blood_points -= atk;
			if(magicLv[current][0] > 1 || magicLv[current][1] > 0){
				if(magicLv[current][1] > 0) magicLv[current][1]--;
				else{magicLv[current][0]--; magicLv[current][1] = total_xp-1;}
			}
			go_back = 10;
			go_back_dir = backDirection(ent);
		}
	}
	void controlMe(){
		bool key_pressed = false;
		if(!blocked){
			if(key[KEY_X] && stamina > 0 && !tired) {
				velocity = 1;
				stamina-=2;
				if(stamina <= 0) {
					tired = true;
					stamina = 0;
				} 
			}
			else {
				velocity = 2;
				if(stamina < total_stamina){
					stamina+=2;
					if(stamina >= total_stamina/4){
						tired = false;
					}
					if(stamina > total_stamina) stamina = total_stamina;
				}
			}
			if(key[KEY_1]) current = fire;
			else if(key[KEY_2]) current = ice;
			else if(key[KEY_3]) current = heal;
			else if(key[KEY_4]) current = thunder;
			if(key[KEY_A] && !atk && !depress_key) {
				action = 1;
				atk = true;
				timer = 1;
				depress_key = true;
				key_pressed = true;
				play_sample(swordMiss ,100, 128, 1000, 0);
			}
			if(key[KEY_S] && !depress_key_magic && !atk && inUse == NULL && energy > 0) {
				if(current == ice || current == fire){
          createMagic(current);
          if(current == ice) play_sample(iceSound ,100, 128, 1000, 0);
          else play_sample(fireSound ,100, 128, 1000, 0);
        }
				if(current == heal) {
					blood_points += magicLv[heal][0]; if(blood_points > total_blood_points) blood_points = total_blood_points;
					play_sample(healSound, 255, 128, 1000, 0);
				}
				if(current == thunder){
          play_sample(thunderSound ,100, 128, 1000, 0);
        }
				action = 2;
				atk = true;
				timer = 1;
				key_pressed = true;
				depress_key_magic = true;
				energy--;
			}
			
			if(key[KEY_UP] && !atk) {
				updatePosition(0, -1);
				move(_up);
				key_pressed = true;
			}
			else if(key[KEY_DOWN] && !atk) {
				updatePosition(0, 1);
				move(_down);
				key_pressed = true;
			}
			if(key[KEY_RIGHT] && !atk) {
				updatePosition(1, 0);
				move(_right);
				key_pressed = true;
			}
			else if(key[KEY_LEFT] && !atk) {
				updatePosition(-1, 0);
				move(_left);
				key_pressed = true;
			}
			if(!atk && !key_pressed) {
				move(_stop);
			}
		}

		/*Trata freeze*/
		if (frozen) freeze_ttl--;
		if(frozen && freeze_ttl == 0){ unblock(); frozen = false;}
		
		if(atk && action == 1) { //Ataque
			animateAtk();
		}
		else if(atk && action == 2) { //Magia
			animateMagic(current);
		}
		if(inUse!=NULL && inUse->getTtl() <= 0) {
			delete inUse;
			inUse = NULL;
		}
		
		if(atk && begin_atk) {
			if(timer%5 == 0) atk_counter++;
		}
		else atk_counter = 0;
		
		if(!key[KEY_A] && depress_key) depress_key = false;
		if(!key[KEY_S] && depress_key_magic) depress_key_magic = false;
		
		if(go_back != 0){
			switch(go_back_dir){
				case _down: updatePosition(0, 5); break;
				case _right: updatePosition(5, 0); break;
				case _left: updatePosition(-5, 0); break;
				case _up: updatePosition(0, -5); break;
			}
			go_back--;
		}
	}

	void catchItem(itemType x, int ttl){
		if(ttl > 0){
			switch(x){
				case blood_rec:
					if(blood_points < total_blood_points) blood_points++;
					break;
				case energy_rec:
					if(energy < total_energy){ energy+=2; if(energy > total_energy) energy = total_energy;}
					break;
				case xp:
					if(magicLv[current][0]!=3 || magicLv[current][1]!=total_xp) {
						if(magicLv[current][0] == 3){magicLv[current][1]+=10; if(magicLv[current][1] > total_xp) magicLv[current][1] = total_xp;}
						else{
							magicLv[current][1] += 10;
							if(magicLv[current][1] >= total_xp){
								magicLv[current][0]++;
								magicLv[current][1] = magicLv[current][1]%total_xp;
							}
						}
/*						else if(++magicLv[current][1]%total_xp == 0){
							magicLv[current][0]++;
							magicLv[current][1] = 0;
						}
*/					}
					break;
			}
		}
	}
	
	void drawSprite(BITMAP* buffer, coord origin) { //Desenha o sprite
		draw_sprite(buffer, (BITMAP*)sprite[animationState].dat, position.x-tira_h-origin.x, position.y-tira_v-origin.y);
		if(atk && action == 2) {
			if(current == heal && inUse == NULL) {
				draw_sprite(buffer, (BITMAP*)healSprite[atk_counter%2].dat, position.x-tira_h-origin.x + 7, position.y-tira_v-origin.y + 2);
			}
			else if(current == thunder && inUse == NULL) {
				draw_sprite(buffer, (BITMAP*)thunderSprite[atk_counter%2].dat, position.x-tira_h-origin.x - 15, position.y-tira_v-origin.y - 8);
			}
		}
	}
	
	void drawStatus(BITMAP *buffer, coord origin){
		for(int i = 0; i < total_blood_points; i++){
			draw_sprite(buffer, (BITMAP*)hearts[0].dat, 5 + i*20, 15);
		}
		for(int i = 0; i < blood_points; i++){
			draw_sprite(buffer, (BITMAP*)hearts[1].dat, 5 + i*20, 15);
		}

		//Stamina
		if(stamina <= total_stamina/4) rectfill(buffer, 5, 45, 5 + (int)((200.0/((float)total_stamina))*(float)stamina), 50, makecol(242, 105, 53));
		else rectfill(buffer, 5, 45, 5 + (int)((200.0/((float)total_stamina))*(float)stamina), 50, makecol(253, 188, 89));
		rect(buffer, 5, 45, 205, 50, makecol(255, 255, 255));
		
		//Energy
		rectfill(buffer, 5, 35, 5 + (int)((200.0/((float)total_energy))*(float)energy), 43, makecol(0, 0, 255));
		rect(buffer, 5, 35, 205, 43, makecol(255, 255, 255));

		
		textprintf_ex(buffer, font, 205, 5, makecol(255, 255, 255), -1, "Lv: %d", magicLv[current][0]);
		rectfill(buffer, 210, 17, 240, 43, makecol(0, 0, 0));
		rect(buffer, 210, 17, 240, 43, makecol(255, 255, 255));
		draw_sprite(buffer, (BITMAP*)magics[current].dat, 215, 20);
		
		rectfill(buffer, 210, 45, 210 + ((30/total_xp)*magicLv[current][1]), 50, makecol(255, 255, 0));
		rect(buffer, 210, 45, 240, 50, makecol(255, 255, 255));
		
	}
	
};
#endif
