#ifndef NPC_H
#define NPC_H
#include <cstdarg>
#include "entity.h"

enum NpcState {halt, wander, patrol, following, escaping, fly};

class Npc : public Entity {
 protected:
	int current_waypoint;
	NpcState state;
	Entity *target;
	vector <coord> waypoints;
	bool alive;
	
	bool preso;
	
	int preventDeadLock;

 public:
	Npc(char* path, int width, int height, int x, int y, int vel = 2, NpcState npc_state = halt, int h = 0, int v = 0, EntityState es = f_parado): Entity (path, width, height, x, y, h, v, vel, es) {
		state = npc_state;
		current_waypoint = 0;
		target = NULL;
		name = _Npc;
		preso = false;
		preventDeadLock = 0;
		alive = true;
	}
	~Npc() {waypoints.clear();}
	
	bool isAlive() {
		return alive;
	}
	bool kill() {
		alive = false;
	}
	void revive() { alive = true; }
	
	void wandering() {
		state = wander;
	}
	
	void stopWalk() {
		state = halt;
	}
	
	void addWaypoint(coord c){
		waypoints.push_back(c);
	}
	vector<coord>& getWayPoints() {return waypoints;}
	//return true se chega no point
	bool FollowWaypoint(){
		if (waypoints.empty()){
			return false;
		}
		
		if (waypoints[current_waypoint].y > position.y){
			updatePosition(0, 1);
			move(_down);
		}
		else if (waypoints[current_waypoint].y < position.y){
			updatePosition(0, -1);
			move(_up);
		}
		
		if (waypoints[current_waypoint].x > position.x){
			updatePosition(1, 0);
			move(_right);
		}
		else if (waypoints[current_waypoint].x < position.x){
			updatePosition(-1, 0);
			move(_left);
		}
			
		if (waypoints[current_waypoint] == position){
			move(_stop);
			current_waypoint = (current_waypoint + 1) % waypoints.size();
			return true;
		}
		return false;
	}
	
	void Wander(){
		int think = rand() % 10000;
		if (think < 100){
			waypoints.clear();
			think = rand() % 4;
			switch(think){
				case 0: addWaypoint(coord(position.x + width, position.y)); break;
				case 1: addWaypoint(coord(position.x - width, position.y)); break;
				case 2: addWaypoint(coord(position.x, position.y + height)); break;
				case 3: addWaypoint(coord(position.x, position.y - height)); break;
			}
		}
		else
			FollowWaypoint();
	}

	void Fly(){
		int think;
		if(waypoints.size() == 0){
			think = rand() % 4;
			switch(think){
				case 0: addWaypoint(coord(position.x + 15, position.y)); break;
				case 1: addWaypoint(coord(position.x - 15, position.y)); break;
				case 2: addWaypoint(coord(position.x, position.y + 15)); break;
				case 3: addWaypoint(coord(position.x, position.y - 15)); break;
			}
		}
		else
			if(FollowWaypoint()){ waypoints.clear();}
			preventDeadLock++;
			if(preventDeadLock >= 50){waypoints.clear(); preventDeadLock = 0;}
	}

	void follow(Entity& target){
		this->target = &target;
		state = following;
	}
	
	void escape(Entity& target){
		this->target = &target;
		state = escaping;
	}

	void followTarget(){
		Coord targetCenter, myCenter;
		targetCenter.x = (target->getPosition().x) + target->getWidth()/2;
		targetCenter.y = (target->getPosition().y) + target->getHeight()/2;
		myCenter.x = (getPosition().x) + getWidth()/2;
		myCenter.y = (getPosition().y) + getHeight()/2;
		if (target == NULL)
			return;
		if (targetCenter.y > myCenter.y){
			updatePosition(0, 1);
			move(_down);
		}
		else if (targetCenter.y < myCenter.y){
			updatePosition(0, -1);
			move(_up);
		}
		
		if (targetCenter.x > myCenter.x){
			updatePosition(1, 0);
			move(_right);
		}
		else if (targetCenter.x < myCenter.x){
			updatePosition(-1, 0);
			move(_left);
		}
	}

	void escapeTarget(){
		if (target == NULL)
			return;
		else if (max(abs(target->getPosition().y - position.y),abs(target->getPosition().x - position.x)) > 100){
			move(_stop);
			return;
		}
		
		if (target->getPosition().y > position.y){
			updatePosition(0,-1);
			move(_up);
		}
		else if (target->getPosition().y < position.y){
			updatePosition(0, 1);
			move(_down);
		}
		
		if (target->getPosition().x < position.x){
			updatePosition(1, 0);
			move(_right);
		}
		else if (target->getPosition().x > position.x){
			updatePosition(-1, 0);
			move(_left);
		}
	};
	
	void controlMe(){
		if(!blocked){
			switch (state){
				case patrol: FollowWaypoint(); break;
				case wander: Wander(); break;
				case following: followTarget(); break;
				case escaping: escapeTarget(); break;
				case fly: Fly(); break;
			}
		}
	}
};

#define MESSAGE_TTL 40
#define MAX_MESSAGES_DISPLAYED 3
#define LINE_WIDTH 25
class TalkableNpc: public Npc {
protected:
	bool beginYourself;
	string speaking;
	bool talking;
	int talkingWith;
	bool mute;
	int current_message;
	vector <string> messages;
	vector <string> dialog;
	
public:
	TalkableNpc(char* path, int width, int height, int x, int y, int vel = 2, NpcState npc_state = halt, EntityState es = f_parado): Npc(path, width, height, x, y, vel, npc_state, 0, 0, es){
		current_message = -1; 
		talking = false;
		name = _TalkableNpc;
		beginYourself = false;
		mute = false;
	}
	TalkableNpc(char* path, int width, int height, int x, int y, int vel, NpcState npc_state, EntityState es, int num_dialogs, ...): Npc(path, width, height, x, y, vel, npc_state, 0, 0, es){
		current_message = -1; 
		talking = false;
		mute = false;
		name = _TalkableNpc;
		beginYourself = false;
		va_list dialogs;
		va_start(dialogs, num_dialogs);
		for(int i = 0; i < num_dialogs; i++)
			messages.push_back(va_arg(dialogs, char*));
		va_end(dialogs);
	}
	
	~TalkableNpc(){
		messages.clear();
	};
		
	void addMessage(char* message){
		messages.push_back(message);
	}

	void setMute(bool b) {
		mute = b;
	}
	
	string nextTok(string &word, char delimiter) {
		string ret;
		int i;
		for(i = 0; i < word.size() && word[i] != delimiter; i++) {
			ret+=word[i];
		}
		if(i == word.size()) word.clear();
		else {
			word.erase(0, i+1);
			ret+=delimiter;
		}
		return ret;
	}
	bool getBeginYourself(){return beginYourself;}
	void setBeginYourself(bool b){beginYourself = b;}
	void controlMe(){
		Npc::controlMe();
		Entity *heroi = (*entities)[0];
		Entity *heroi2 = (*entities)[1];
		if (!mute && alive && !talking && (key[KEY_Z] || beginYourself) && !messages.empty() && distance(*heroi) <= 50 && key_delay <= 0){
			talkingWith = 0;
			talking = true;
			if(distanceX(*heroi) > distanceY(*heroi)){
				if(heroi->getPosition().x < position.x) { updateState(e_parado); heroi->updateState(d_parado); }
				else { updateState(d_parado); heroi->updateState(e_parado); }
			}
			else{
				if(heroi->getPosition().y < position.y) { updateState(t_parado); heroi->updateState(f_parado); }
				else { updateState(f_parado); heroi->updateState(t_parado); }
			}
			heroi->block();
			heroi->move(_stop);
			move(_stop);
			block();
			key_delay = talk_delay;
			if (++current_message == messages.size())
				current_message = 0;
			speaking = messages[current_message];
			string line;
			for(int i = 0; i < MAX_MESSAGES_DISPLAYED; i++) {
				while(line.size() < LINE_WIDTH && speaking.size() != 0) line+=nextTok(speaking, ' ');
				dialog.push_back(line);
				if(speaking.size() == 0) break;
				line.clear();
			}
		}
		else if (!mute && alive && talking && talkingWith == 0 && key[KEY_Z] && key_delay <= 0){
			key_delay = talk_delay;
			if(speaking.size() == 0) {
				dialog.clear();
				talking = false;
				heroi->unblock();
				unblock();
			}
			else {
				dialog.clear();
				string line;
				for(int i = 0; i < MAX_MESSAGES_DISPLAYED; i++) {
					while(line.size() < LINE_WIDTH && speaking.size() != 0) line+=nextTok(speaking, ' ');
					dialog.push_back(line);
					if(speaking.size() == 0) break;
					line.clear();
				}				
			}
		}			
		
		//Fala com heroi2
		if (!mute && alive && !talking && key[KEY_C] && !messages.empty() && distance(*heroi2) <= 50 && key_delay <= 0){
			talkingWith = 1;
			talking = true;
			if(distanceX(*heroi2) > distanceY(*heroi2)){
				if(heroi2->getPosition().x < position.x) { updateState(e_parado); heroi2->updateState(d_parado); }
				else { updateState(d_parado); heroi2->updateState(e_parado); }
			}
			else{
				if(heroi2->getPosition().y < position.y) { updateState(t_parado); heroi2->updateState(f_parado); }
				else { updateState(f_parado); heroi2->updateState(t_parado); }
			}
			heroi2->block();
			heroi2->move(_stop);
			move(_stop);
			block();
			key_delay = talk_delay;
			if (++current_message == messages.size())
				current_message = 0;
			speaking = messages[current_message];
			string line;
			for(int i = 0; i < MAX_MESSAGES_DISPLAYED; i++) {
				while(line.size() < LINE_WIDTH && speaking.size() != 0) line+=nextTok(speaking, ' ');
				dialog.push_back(line);
				if(speaking.size() == 0) break;
				line.clear();
			}
		}
		else if (!mute && alive && talking && talkingWith == 1 && key[KEY_C] && key_delay <= 0){
			key_delay = talk_delay;
			if(speaking.size() == 0) {
				dialog.clear();
				talking = false;
				heroi2->unblock();
				unblock();
			}
			else {
				dialog.clear();
				string line;
				for(int i = 0; i < MAX_MESSAGES_DISPLAYED; i++) {
					while(line.size() < LINE_WIDTH && speaking.size() != 0) line+=nextTok(speaking, ' ');
					dialog.push_back(line);
					if(speaking.size() == 0) break;
					line.clear();
				}				
			}
		}	
	}
	int getTalking() {return talking;}
	int getCurrent(){return current_message;}
	int getMessagesNumber() {return messages.size();}	
	void drawDialog(BITMAP* buffer, coord origin) { //Desenha as falas
		if (talking){
			for(int i = dialog.size()-1, j = 0; i >=0; i--, j++) {
				textout_centre_ex(buffer, font, dialog[i].c_str(), position.x + width/2 -origin.x, position.y - height/2 - text_height(font)* j + 5 - origin.y, makecol(0, 0, 0), makecol(255, 255, 255));
			}
		}
	}
};

class InvestigateNpc: public TalkableNpc {
protected:
  int distx;
  int disty;

public:
	InvestigateNpc(int x, int y, int distx, int disty, int num_dialogs = 0, ...): TalkableNpc((char*)"sprites/invisible.dat", 32, 32, x, y, 2, halt){
    this->distx = distx;
    this->disty = disty;
		current_message = -1;
		talking = false;
		name = _InvestigateNpc;
		va_list dialogs;
		va_start(dialogs, num_dialogs);
		for(int i = 0; i < num_dialogs; i++)
			messages.push_back(va_arg(dialogs, char*));
		va_end(dialogs);
	}
	void controlMe() {
    	Npc::controlMe();
		Entity *heroi = (*entities)[0];
		Entity *heroi2 = (*entities)[1];
		if (!talking && key[KEY_Z] && !messages.empty() && distanceX(*heroi) <= distx & distanceY(*heroi) <= disty && key_delay <= 0){
			talkingWith = 0;
			talking = true;
			heroi->block();
			heroi->move(_stop);
			move(_stop);
			block();
			key_delay = talk_delay;
			if (++current_message == messages.size())
				current_message = 0;
			speaking = messages[current_message];
			string line;
			for(int i = 0; i < MAX_MESSAGES_DISPLAYED; i++) {
				while(line.size() < LINE_WIDTH && speaking.size() != 0) line+=nextTok(speaking, ' ');
				dialog.push_back(line);
				if(speaking.size() == 0) break;
				line.clear();
			}
		}
		else if (talking && talkingWith == 0 && key[KEY_Z] && key_delay <= 0){
			key_delay = talk_delay;
			if(speaking.size() == 0) {
				dialog.clear();
				talking = false;
				heroi->unblock();
				unblock();
			}
			else {
				dialog.clear();
				string line;
				for(int i = 0; i < MAX_MESSAGES_DISPLAYED; i++) {
					while(line.size() < LINE_WIDTH && speaking.size() != 0) line+=nextTok(speaking, ' ');
					dialog.push_back(line);
					if(speaking.size() == 0) break;
					line.clear();
				}
			}
		}
		
		//conversa com hero2
		if (!talking && key[KEY_C] && !messages.empty() && distanceX(*heroi2) <= distx & distanceY(*heroi2) <= disty && key_delay <= 0){
			talkingWith = 1;
			talking = true;
			heroi2->block();
			heroi2->move(_stop);
			move(_stop);
			block();
			key_delay = talk_delay;
			if (++current_message == messages.size())
				current_message = 0;
			speaking = messages[current_message];
			string line;
			for(int i = 0; i < MAX_MESSAGES_DISPLAYED; i++) {
				while(line.size() < LINE_WIDTH && speaking.size() != 0) line+=nextTok(speaking, ' ');
				dialog.push_back(line);
				if(speaking.size() == 0) break;
				line.clear();
			}
		}
		else if (talking && talkingWith == 1 && key[KEY_C] && key_delay <= 0){
			key_delay = talk_delay;
			if(speaking.size() == 0) {
				dialog.clear();
				talking = false;
				heroi2->unblock();
				unblock();
			}
			else {
				dialog.clear();
				string line;
				for(int i = 0; i < MAX_MESSAGES_DISPLAYED; i++) {
					while(line.size() < LINE_WIDTH && speaking.size() != 0) line+=nextTok(speaking, ' ');
					dialog.push_back(line);
					if(speaking.size() == 0) break;
					line.clear();
				}
			}	
		}
	}
	void drawDialog(BITMAP* buffer, coord origin) { //Desenha as falas
		if (talking){
			if(talkingWith == 0) {
				for(int i = dialog.size()-1, j = 0; i >=0; i--, j++) {
					textout_centre_ex(buffer, font, dialog[i].c_str(), (*entities)[0]->getX() + (*entities)[0]->getWidth()/2 -origin.x, (*entities)[0]->getY() - (*entities)[0]->getHeight()/2 - text_height(font)* j + 5 - origin.y, makecol(0, 0, 0), makecol(255, 255, 255));
				}
			} else {
				for(int i = dialog.size()-1, j = 0; i >=0; i--, j++) {
					textout_centre_ex(buffer, font, dialog[i].c_str(), (*entities)[1]->getX() + (*entities)[1]->getWidth()/2 -origin.x, (*entities)[1]->getY() - (*entities)[1]->getHeight()/2 - text_height(font)* j + 5 - origin.y, makecol(0, 0, 0), makecol(255, 255, 255));
				}
			}
		}
	}
};

#endif
