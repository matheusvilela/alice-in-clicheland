#ifndef ITEM_H
#define ITEM_H
#include "entity.h"

enum itemType{blood_rec, energy_rec, xp};

class Item : public Entity{
 private:
 
 itemType type;
 int ttl;
 
 public:
	Item(EntityState as, char* path, int width, int height, itemType t, int ttl = 70, int x = 0, int y = 0, int h = 0, int v = 0) : Entity(path, width, height, x, y, h, v) {
		type = t;
		this->ttl = ttl;
		animationState = as;
		name = _Item;
	}
	
	itemType getType(){return type;}
	
	int getTtl(){return ttl;}
	
	void updateTtl(){ttl--;}
	
	void itemGot(){
		ttl = 0;
	}

};
#endif
