#ifndef EVENTS_H
#define EVENTS_H
#include "entity.h"
#include "world.h"
#include "boss.h"
#include "hero.h"
#include "hero2.h"
#include "npc.h"


bool goToPosition(Entity* e, coord c) {
	coord myPos = e->getPosition();

	if(myPos!=c) {
		if(myPos.y > c.y) {
			if(e->updatePosition(0,-1)) {
				e->move(_up);
			}
		}
		else if(myPos.y < c.y) {
			if(e->updatePosition(0,1)) {
				e->move(_down);
			}
		}
		if(myPos.x > c.x) {
			if(e->updatePosition(-1,0)) {
				e->move(_left);
			}
		}
		else if(myPos.x < c.x) {
			if(e->updatePosition(1,0)) {
				e->move(_right);
			}
		}
		return false;
	}
	else return true;
}

void bobEvent(World* world, map<int,Entity*> &entities, int personId) {
	int heroId = 0;
	int hero2Id = 1;
	static int fsm = -1;
	static TalkableNpc* bodyguard, *estatua;
	static Enemy* bob;
	static Magic *m1 = NULL, *m2 = NULL, *m3 = NULL;
	static MIDI *music;
	if(timer%2 == 0) {	
		if(fsm == -1){
			music = load_midi("music/lavos.mid");
			set_volume(-1, 100);
			play_midi(music, 1);
			fsm++;
		}
		//Alice vai ate Bob
		if(fsm == 0) {
			entities[heroId]->block();	
			entities[hero2Id]->block();
			((TalkableNpc*)(entities[personId]))->setMute(true);
			while(!goToPosition(entities[heroId], coord(10*32+12, 4*32+40)) | ( entities[hero2Id]->isAlive() && !goToPosition(entities[hero2Id], coord(9*32+12, 4*32+40))) ) return;
			entities[heroId]->move(_stop);
			entities[hero2Id]->move(_stop);			
			fsm++;
		}
		//Conversa de Bob com Alice
		if(fsm == 1) {
			((TalkableNpc*)(entities[personId]))->setBeginYourself(true);
			((TalkableNpc*)(entities[personId]))->setMute(false);
			key_delay = 0;
			fsm++;
			return;
		}
		//Bob falando
		if(fsm == 2) {
			((TalkableNpc*)(entities[personId]))->setBeginYourself(false);			
			while(((TalkableNpc*)(entities[personId]))->getTalking()) return;
			entities[heroId]->block();
			bodyguard = new TalkableNpc((char*)"sprites/banger_bodyguard.dat", 32, 32, 11*32+15, 15*32, 2, halt, t_parado, 1, "I knew there was something wrong! You are the great warrior ressurected by the leader of Sinisterland! Alice, the crystal may not stay on this monster's hand!");
			world->insertEntity(bodyguard);
			bodyguard->setMute(true);
			((TalkableNpc*)(entities[personId]))->setMute(true);
			fsm++;
		}
		//Bodyguard vai ate Bob
		if(fsm == 3) {
			while(!goToPosition((Entity*)bodyguard, coord(11*32+12, 6*32))) return;
			fsm++;
		}
		//Bodyguard comeca a falar
		if(fsm == 4) {
			bodyguard->setMute(false);
			bodyguard->setBeginYourself(true);
			key_delay = 0;
			fsm++;
			return;
		}
		//Bodyguard fala
		if(fsm == 5) {
			bodyguard->updateState(t_parado);
			entities[heroId]->updateState(t_parado);
			bodyguard->setBeginYourself(false);
			while(bodyguard->getTalking()) return;
			bodyguard->setMute(true);
			entities[heroId]->block();
			fsm++;
		}
		//Bob comeca a falar
		if(fsm == 6) {
			((TalkableNpc*)(entities[personId]))->setMute(false);
			((TalkableNpc*)(entities[personId]))->setBeginYourself(true);
			key_delay = 0;
			fsm++;
			return;
		}
		//Bob falando
		if(fsm == 7) {
			((TalkableNpc*)(entities[personId]))->setBeginYourself(false);			
			while(((TalkableNpc*)(entities[personId]))->getTalking()) return;
			entities[heroId]->block();
			fsm++;
			((TalkableNpc*)(entities[personId]))->setMute(true);
		}
		//Magias sao criadas
		if(fsm == 8) {
			m1 = new Magic(fire, (char*)"sprites/gelo.dat", 15, 16, bodyguard->getPosition().x + 60, bodyguard->getPosition().y, _left, 1, _Hero, 70, 0, 0, 3); m1->insertWorld();
			m2 = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, bodyguard->getPosition().x+10, bodyguard->getPosition().y+60, _up, 1, _Hero, 70, 0, 0, 3); m2->insertWorld();
			m3 = new Magic(fire, (char*)"sprites/fogo.dat", 15, 16, bodyguard->getPosition().x+10, bodyguard->getPosition().y-30, _down, 1, _Hero, 70, 0, 0, 3); m3->insertWorld();
			fsm++;
		}
		//Magias atingem bodyguard
		if(fsm == 9) {
			if(m1 != NULL && m1->getTtl() <= 0) {
				delete m1;	
				m1 = NULL;
			}
			if(m2 != NULL && m2->getTtl() <= 0) {
				delete m2;	
				m2 = NULL;
			}
			if(m3 != NULL && m3->getTtl() <= 0) {
				delete m3;	
				m3 = NULL;
			}
			while (m1!=NULL || m2!=NULL || m3!=NULL) return;
			fsm++;
		}
		//Bob fala de novo
		if(fsm == 10) {
			bodyguard->kill();
			((TalkableNpc*)(entities[personId]))->setMute(false);
			((TalkableNpc*)(entities[personId]))->setBeginYourself(true);
			key_delay = 0;
			fsm++;
			return;
		}
		//Bob falando
		if(fsm == 11) {
			((TalkableNpc*)(entities[personId]))->setBeginYourself(false);			
			while(((TalkableNpc*)(entities[personId]))->getTalking()) return;
			entities[heroId]->block();
			fsm++;
			((TalkableNpc*)(entities[personId]))->setMute(true);
			
			//Insere pedras
			world->insertEntity(new Entity((char*)"sprites/pedra.dat", 32, 32, 7*32, 14*32));
			world->insertEntity(new Entity((char*)"sprites/pedra.dat", 32, 32, 8*32, 14*32));
			world->insertEntity(new Entity((char*)"sprites/pedra.dat", 32, 32, 9*32, 14*32));
			world->insertEntity(new Entity((char*)"sprites/pedra.dat", 32, 32, 10*32, 14*32));
			world->insertEntity(new Entity((char*)"sprites/pedra.dat", 32, 32, 11*32, 14*32));
			world->insertEntity(new Entity((char*)"sprites/pedra.dat", 32, 32, 12*32, 14*32));
			world->insertEntity(new Entity((char*)"sprites/pedra.dat", 32, 32, 13*32, 14*32));
			
		}
		if(fsm == 12) {
			while(!goToPosition(entities[heroId], coord(10*32+12, 7*32+40)) | ( entities[hero2Id]->isAlive() && !goToPosition(entities[hero2Id], coord(9*32+12, 7*32+40))) ) return;
			entities[heroId]->move(_stop);
			entities[heroId]->updateState(t_parado);
			entities[hero2Id]->move(_stop);			
			entities[hero2Id]->updateState(t_parado);
			fsm++;
		}
		//Vira dumau
		if(fsm == 13) {			
			bob = new Boss((char*)"sprites/bob.dat", 32, 32, 10*32, 4*32, 10, 1, halt);
			world->insertEntity(bob);
			((TalkableNpc*)entities[personId])->kill();
			music = load_midi("music/bossbattle.mid");
			set_volume(-1, 100);
			play_midi(music, 1);
			entities[heroId]->unblock();		
			entities[hero2Id]->unblock();
			fsm++;
		}
		if(fsm == 14) {
			while(bob->isAlive()) return;
			entities[heroId]->block();	
			entities[heroId]->move(_stop);
			entities[hero2Id]->block();
			entities[hero2Id]->move(_stop);
			((TalkableNpc*)(entities[personId]))->setPosition(bob->getPosition());
			((TalkableNpc*)(entities[personId]))->revive();
			((TalkableNpc*)(entities[personId]))->updateState(t_parado);
			while(entities[heroId]->distance((*entities[personId])) > 50) {
				static Coord targetCenter, myCenter;
				targetCenter.x = (entities[heroId]->getPosition().x) + entities[heroId]->getWidth()/2;
				targetCenter.y = (entities[heroId]->getPosition().y) + entities[heroId]->getHeight()/2;
				myCenter.x = (entities[personId]->getPosition().x) + entities[personId]->getWidth()/2;
				myCenter.y = (entities[personId]->getPosition().y) + entities[personId]->getHeight()/2;
				if (targetCenter.x > myCenter.x){
					entities[heroId]->updatePosition(-1, 0);
					entities[heroId]->move(_left);
				}
				else if (targetCenter.x < myCenter.x){
					entities[heroId]->updatePosition(1, 0);
					entities[heroId]->move(_right);		
				}
				if (targetCenter.y > myCenter.y){
					entities[heroId]->updatePosition(0, -1);
					entities[heroId]->move(_up);
				}
				else if (targetCenter.y < myCenter.y){
					entities[heroId]->updatePosition(0, 1);
					entities[heroId]->move(_down);		
				}
				return;
			}
			entities[heroId]->move(_stop);
			((TalkableNpc*)(entities[personId]))->setMute(false);
			((TalkableNpc*)(entities[personId]))->setBeginYourself(true);
			fsm++;
			return;
		}
		if(fsm == 15) {
			((TalkableNpc*)(entities[personId]))->setBeginYourself(false);
			while(((TalkableNpc*)(entities[personId]))->getTalking()) return;
			((TalkableNpc*)(entities[personId]))->setMute(true);
			((TalkableNpc*)entities[personId])->kill();
			music = load_midi("music/magusbattle.mid");
			set_volume(-1, 100);
			play_midi(music, 1);
			estatua = new InvestigateNpc(10*32, 3*32, 32, 32, 1, "The crystal is here, we have just to pull it off from the statue. Now, we must collect them all...");
			world->insertEntity(estatua);
			entities[heroId]->unblock();	
			entities[hero2Id]->unblock();
			fsm++;
			return;
		}
		if(fsm == 16) {
			while(estatua->getCurrent() == -1) return;
			while(estatua->getTalking()) return;
			world->getEventHandler() = false;
		}
	}
}


void cityEvent(World* world, map<int,Entity*> &entities, int personId) {
	int heroId = 0;
	world->getEventHandler() = false;
	static int fsm = 0;
	if(timer%2 == 0) {
		entities[heroId]->block();
		entities[personId]->block();
		entities[heroId]->move(_stop);
		
		//Vai ateh o hero
		if(fsm == 0 && entities[heroId]->distance((*entities[personId])) > 50) {
			Coord targetCenter, myCenter;
			targetCenter.x = (entities[heroId]->getPosition().x) + entities[heroId]->getWidth()/2;
			targetCenter.y = (entities[heroId]->getPosition().y) + entities[heroId]->getHeight()/2;
			myCenter.x = (entities[personId]->getPosition().x) + entities[personId]->getWidth()/2;
			myCenter.y = (entities[personId]->getPosition().y) + entities[personId]->getHeight()/2;
			if (targetCenter.x > myCenter.x){
				entities[personId]->updatePosition(1, 0);
				entities[personId]->move(_right);
			}
			else if (targetCenter.x < myCenter.x){
				entities[personId]->updatePosition(-1, 0);
				entities[personId]->move(_left);		
			}
		} else {
			if(fsm == 0) fsm++;
			entities[personId]->move(_stop);
			//Conversa com o hero
			if(fsm == 1 && ((TalkableNpc*)(entities[personId]))->getBeginYourself() == false) {
				fsm++;
				((TalkableNpc*)(entities[personId]))->setBeginYourself(true);
				return;
			}
			((TalkableNpc*)(entities[personId]))->setBeginYourself(false);
			while(((TalkableNpc*)(entities[personId]))->getTalking()) return;
			
			//Faz o heroi voltar 2 celulas
			static int go = 64;
			while(go-- > 0) {
				entities[heroId]->updatePosition(0, 1);
				entities[heroId]->move(_down);
				return;
			}
			entities[heroId]->move(_stop);
			
			//Faz o npc voltar para a posicao inicial
			((TalkableNpc*)(entities[personId]))->addWaypoint(entities[personId]->getInitialPos());
			while(entities[personId]->getPosition() != entities[personId]->getInitialPos()) {
				if(((TalkableNpc*)(entities[personId]))->FollowWaypoint()){ (((TalkableNpc*)(entities[personId]))->getWayPoints().clear());}
				return;
			}
			entities[personId]->updateState(d_parado);
			entities[heroId]->unblock();			
			entities[personId]->unblock();
			world->getEventHandler() = true;
			fsm = 0;
			go = 64;
		}
	}
}


void innEvent(World* world, map<int,Entity*> &entities, int personId, bool multiplayer) {
	int heroId = 0;
	int hero2Id = 1;
	world->getEventHandler() = false;
	if(timer%2 == 0) {
		entities[heroId]->block();
		entities[personId]->block();
		
		//Conversa com o hero
		while(((TalkableNpc*)(entities[personId]))->getTalking()) return;
		
		//Pisca/toca musica
		play_sample(healSound, 255, 128, 1000, 0);
		
		
		//Cura o heroi
		((Hero*)(entities[heroId]))->totalHeal();		
		if(multiplayer) ((Hero2*)(entities[hero2Id]))->totalHeal();		
		entities[heroId]->unblock();			
		entities[personId]->unblock();
		world->getEventHandler() = true;
	}
}

#endif
