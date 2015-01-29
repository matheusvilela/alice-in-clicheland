#ifndef BOSS_H
#define BOSS_H
#include "enemy.h"

class Boss : public Enemy{
 private:
	
	/*Usadas para o ataque de curta distancia de Bob*/
	Magic *sra1;
	Magic *sra2;
	Magic *sra3;
	Magic *sra4;

	/*Magia de uso geral*/
	Magic *generalMagic;

	/*Magia usada durante caminhamento*/
	Magic *wm1;
	Magic *wm2;
	Magic *wm3;

	vector<coord> waypoints;
	coord currentPoint;

 public:
	Boss(char* path, int width, int height, int x = 0, int y = 0, int blood_points = 3, int vel = 3, NpcState npc_state = halt, int atk = 1) : Enemy(path, width, height, x, y, 0, blood_points, vel, npc_state, atk, 0, 0) {
	sra1 = NULL;
	sra2 = NULL;
	sra3 = NULL;
	sra4 = NULL;
	generalMagic = NULL;
	wm1 = NULL;
	wm2 = NULL;
	wm3 = NULL;

	waypoints.push_back(coord(10*32, 4*32));
	waypoints.push_back(coord(2*32, 7*32));
	waypoints.push_back(coord(17*32, 7*32));
	waypoints.push_back(coord(10*32, 7*32));
	waypoints.push_back(coord(10*32, 11*32));
	currentPoint = coord(10*32, 4*32);
	}
	~Boss(){}

	bool gotoCurrent(){
		if (currentPoint.y > position.y){
			updatePosition(0, 1);
			move(_down);
		}
		else if (currentPoint.y < position.y){
			updatePosition(0, -1);
			move(_up);
		}
		
		if (currentPoint.x > position.x){
			updatePosition(1, 0);
			move(_right);
		}
		else if (currentPoint.x < position.x){
			updatePosition(-1, 0);
			move(_left);
		}
			
		if (currentPoint == position){
			move(_stop);
			return true;
		}
		return false;
	}

	void controlMe(){
		Hero* alice = (Hero*)(*entities)[0];
		Hero* heroi2 = (Hero*)(*entities)[1];
		Enemy::controlMe();
		int prob;
		/*IA*/
		if(!blocked && blood_points > 0){
			/*Se alice esta congelada, vai atras dela!*/
			if(alice->isFrozen()){
				follow(*alice);
			}
			else if(heroi2->isFrozen()){
				follow(*heroi2);
			}
			/*Senão segue pontos aleatórios*/
			else{
				stopWalk();
				if(gotoCurrent()){
					prob = rand()%5;
					currentPoint = waypoints[prob];
				}
			}
			/*Se esta alinhado, fogo! (ou gelo =) )*/
			prob = rand()%2;
			if(prob == 0){
				if((alice->getPosition().x <= position.x+32 && alice->getPosition().x >= position.x-32) && generalMagic == NULL){
          play_sample(fireSound ,55, 128, 1000, 0);
					if(alice->getPosition().y > position.y){updateState(f_parado); generalMagic = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x + 5, getPosition().y + getHeight(), _down, 3, name, 200); generalMagic->insertWorld();}
					else {updateState(t_parado); generalMagic = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x+ 30 -15 - 5, getPosition().y-1, _up, 3, name, 200); generalMagic->insertWorld();}
				}
				else if((alice->getPosition().y <= position.y+32 && alice->getPosition().y >= position.y-32) && generalMagic == NULL){
          play_sample(fireSound ,55, 128, 1000, 0);
					if(alice->getPosition().x > position.x){updateState(d_parado); generalMagic = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x + 33, getPosition().y + getHeight()/2 - 5, _right, 3, name, 200); generalMagic->insertWorld();}
					else {updateState(e_parado); generalMagic = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x - 16, getPosition().y + getHeight()/2 - 5, _left, 3, name, 200); generalMagic->insertWorld();}
				}
				if((heroi2->getPosition().x <= position.x+32 && heroi2->getPosition().x >= position.x-32) && generalMagic == NULL){
          play_sample(fireSound ,55, 128, 1000, 0);
					if(heroi2->getPosition().y > position.y){updateState(f_parado); generalMagic = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x + 5, getPosition().y + getHeight(), _down, 3, name, 200); generalMagic->insertWorld();}
					else {updateState(t_parado); generalMagic = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x+ 30 -15 - 5, getPosition().y-1, _up, 3, name, 200); generalMagic->insertWorld();}
				}
				else if((heroi2->getPosition().y <= position.y+32 && heroi2->getPosition().y >= position.y-32) && generalMagic == NULL){
          play_sample(fireSound ,55, 128, 1000, 0);
					if(heroi2->getPosition().x > position.x){updateState(d_parado); generalMagic = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x + 33, getPosition().y + getHeight()/2 - 5, _right, 3, name, 200); generalMagic->insertWorld();}
					else {updateState(e_parado); generalMagic = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x - 16, getPosition().y + getHeight()/2 - 5, _left, 3, name, 200); generalMagic->insertWorld();}
				}
			}
			else{
				if((alice->getPosition().x <= position.x+32 && alice->getPosition().x >= position.x-32) && generalMagic == NULL){
          play_sample(iceSound ,55, 128, 1000, 0);
					if(alice->getPosition().y > position.y){updateState(f_parado); generalMagic = new Magic(ice, (char*)"sprites/gelo.dat", 15, 16, getPosition().x + 5, getPosition().y + getHeight(), _down, 1, name, 200); generalMagic->insertWorld();}
					else {updateState(t_parado); generalMagic = new Magic(ice, (char*)"sprites/gelo.dat", 15, 16, getPosition().x+ 30 -15 - 5, getPosition().y-1, _up, 3, name, 200); generalMagic->insertWorld();}
				}
				else if((alice->getPosition().y <= position.y+32 && alice->getPosition().y >= position.y-32) && generalMagic == NULL){
          play_sample(iceSound ,55, 128, 1000, 0);
					if(alice->getPosition().x > position.x){updateState(d_parado); generalMagic = new Magic(ice, (char*)"sprites/gelo.dat", 15, 16, getPosition().x + 33, getPosition().y + getHeight()/2 - 5, _right, 1, name, 200); generalMagic->insertWorld();}
					else {updateState(e_parado); generalMagic = new Magic(ice, (char*)"sprites/gelo.dat", 15, 16, getPosition().x - 16, getPosition().y + getHeight()/2 - 5, _left, 3, name, 200); generalMagic->insertWorld();}
				}
				if((heroi2->getPosition().x <= position.x+32 && heroi2->getPosition().x >= position.x-32) && generalMagic == NULL){
          play_sample(iceSound ,55, 128, 1000, 0);
					if(heroi2->getPosition().y > position.y){updateState(f_parado); generalMagic = new Magic(ice, (char*)"sprites/gelo.dat", 15, 16, getPosition().x + 5, getPosition().y + getHeight(), _down, 1, name, 200); generalMagic->insertWorld();}
					else {updateState(t_parado); generalMagic = new Magic(ice, (char*)"sprites/gelo.dat", 15, 16, getPosition().x+ 30 -15 - 5, getPosition().y-1, _up, 3, name, 200); generalMagic->insertWorld();}
				}
				else if((heroi2->getPosition().y <= position.y+32 && heroi2->getPosition().y >= position.y-32) && generalMagic == NULL){
          play_sample(iceSound ,55, 128, 1000, 0);
					if(heroi2->getPosition().x > position.x){updateState(d_parado); generalMagic = new Magic(ice, (char*)"sprites/gelo.dat", 15, 16, getPosition().x + 33, getPosition().y + getHeight()/2 - 5, _right, 1, name, 200); generalMagic->insertWorld();}
					else {updateState(e_parado); generalMagic = new Magic(ice, (char*)"sprites/gelo.dat", 15, 16, getPosition().x - 16, getPosition().y + getHeight()/2 - 5, _left, 3, name, 200); generalMagic->insertWorld();}
				}
			}
			/*Se alice chegar perto demais, ataque de curto alcance por todas as direções*/
			if((distance(*alice) < 70 || distance(*heroi2) < 70) && sra1 == NULL && sra2 == NULL && sra3 == NULL && sra4 == NULL){
        play_sample(fireSound ,55, 128, 1000, 0);
        play_sample(iceSound ,55, 128, 1000, 0);
				sra1 = new Magic(ice, (char*)"sprites/gelo.dat", 15, 16, getPosition().x + 5, getPosition().y + getHeight(), _down, 1, name, 200);
				sra1->insertWorld();
				sra2 = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x + 33, getPosition().y + getHeight()/2 - 5, _right, 3, name, 200);
				sra2->insertWorld();
				sra3 = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, getPosition().x - 16, getPosition().y + getHeight()/2 - 5, _left, 3, name, 200);
				sra3->insertWorld();
				sra4 = new Magic(ice, (char*)"sprites/gelo.dat", 15, 16, getPosition().x+ 30 -15 - 5, getPosition().y-1, _up, 1, name, 200);
				sra4->insertWorld();
			}

		}

		/*Deleta shortRangeAtk de Bob*/
		if(sra1!=NULL && sra1->getTtl() <= 0) {
			delete sra1;
			sra1 = NULL;
		}
		if(sra2!=NULL && sra2->getTtl() <= 0) {
			delete sra2;
			sra2 = NULL;
		}
		if(sra3!=NULL && sra3->getTtl() <= 0) {
			delete sra3;
			sra3 = NULL;
		}
		if(sra4!=NULL && sra4->getTtl() <= 0) {
			delete sra4;
			sra4 = NULL;
		}
		/*Deleta generalMagic*/
		if(generalMagic!=NULL && generalMagic->getTtl() <= 0) {
			delete generalMagic;
			generalMagic = NULL;
		}

	}
};

#endif
