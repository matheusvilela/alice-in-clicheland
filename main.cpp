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
#include "events.h"
#include "boss.h"


void changeMultiplayer(bool& multiplayer, Entity *hero, Entity *hero2) {
	multiplayer = !multiplayer;
	if(!multiplayer) ((Hero2*)hero2)->kill();
	else {
		((Hero2*)hero2)->totalHeal();	
		hero2->setPosition(coord(hero->getPosition().x+32, hero->getPosition().y));
	}
}

/* Timer stuff */
volatile long speed_counter = 0;
void increment_speed_counter() { speed_counter++; }
END_OF_FUNCTION(increment_speed_counter);

int main(int argc, char *argv[]) {
	allegro_init(); // Initialize Allegro
	
	install_keyboard(); // Initialize keyboard routines
	
	install_timer(); // Initialize the timer routines
	
	LOCK_VARIABLE(speed_counter); //Used to set the timer - which regulates the game's
	LOCK_FUNCTION(increment_speed_counter); //speed.
	install_int_ex(increment_speed_counter, BPS_TO_TIMER(120)); //Set our BPS
	
	
	set_color_depth(32); // Set the color depth
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640,480,0,0); // Change our graphics mode to 640x480
	
	BITMAP *buffer = create_bitmap(640,480); //Declaracao do buffer
	
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
	MIDI *music;

	/*Sound effects*/
	swordHitSound = load_wav("music/sword.wav");
	swordMiss = load_wav("music/swordmiss.wav");
	healSound = load_wav("music/heal.wav");
	fireSound = load_wav("music/Fire.wav");
	iceSound = load_wav("music/Ice.wav");
	thunderSound = load_wav("music/Thunder.wav");
	absorbSound = load_wav("music/Absorb.wav");
	hitSound = load_wav("music/hit.wav");

	BITMAP* titleScreen = load_bitmap("sprites/titlescreen.bmp", NULL);
	BITMAP* gameover = load_bitmap("sprites/gameover.bmp", NULL);
	BITMAP* fim = load_bitmap("sprites/ending.bmp", NULL);

	
	//DEFINE SE O JOGO EH MULTIPLAYER
	bool multiplayer = false;
	
	World *city = new World((char*)"maps/cidade.map", (char*)"medievaltown", 50, 30, 814);
	World *qg = new World((char*)"maps/qg.map", (char*)"medievaltown", 20, 15, 814);
	World *familyHouse = new World((char*)"maps/familyHouse.map", (char*)"medievaltown", 20, 15, 814);
	World *inn = new World((char*)"maps/inn.map", (char*)"medievaltown", 20, 15, 814);
	World *forest = new World((char*)"maps/forest.map", (char*)"forest", 80, 60, 550, coord(0*32, 45*32));
	World *mausoleum = new World((char*)"maps/mausoleum.map", (char*)"mausoleum", 20, 30, 260, coord(0*32, 15*32));
	
	Screen myscreen(*city);
	
	
	Hero* heroi = new Hero((char*)"sprites/alice.dat", 11, 32, 640/2, 480/2/*10*32, 28*32*/, 22, 16);
	Hero2* heroi2 = new Hero2((char*)"sprites/guy.dat", 11, 32, 640/2+50, 480/2+50, 22, 16);
	heroi2->kill();
	
	//Insercoes na cidade
	city->insertEntity(heroi);
	city->insertEntity(heroi2);
	city->insertEntity(new InvestigateNpc(27*32+16, 14*32, 40, 35, 1, "A beautiful statue. It represents the Goddess of War."));
	city->insertEntity(new TalkableNpc((char*)"sprites/city_guard.dat", 32, 32, 36*32, 12*32, 2, halt, d_parado, 3, "Welcome to Cronos, headquarters of the Knights of Order.", "...", "My mission is to protect the city."));
	city->insertEntity(new TalkableNpc((char*)"sprites/city_guard.dat", 32, 32, 36*32, 18*32, 2, halt, d_parado, 2, "Going ahead, you will find the Slimes' Forest. Watch out, it's a dangerous place.", "I will give my live to protect master Banger!"));	
	//NAO MUDAR A ORDEM DO NPC DE BAIXO, ELE TEM QUE SER O SEXTO INSERIDO!
	city->insertEntity(new TalkableNpc((char*)"sprites/city_guard.dat", 32, 32, 36*32, 1*32, 2, halt, d_parado, 1, "Hey! Going this way is forbidden! Go back to the city."));
	city->insertEntity(new TalkableNpc((char*)"sprites/random3.dat", 32, 32, 32*32, 27*32, 2, wander, d_parado, 2, "During these hard days, Cronos is one of the safest places of the whole world.", "Aaaaaaah... I love butterflies! Aren't they graceful?"));
	city->insertEntity(new TalkableNpc((char*)"sprites/random1.dat", 32, 32,  24*32, 14*32, 2, wander, d_parado, 3, "Beyond the Slimes' Forest, there is a really old church. However, all the adventurers that tried to reach it were never seen again. They say there's a valuable treasure there.", "Don't be scared, maybe it is just some rumors...", "I prefer not to risk, and stay under the Knights of Order's protection."));
	city->insertEntity(new TalkableNpc((char*)"sprites/random4.dat", 32, 32, 4*32, 17*32, 2, wander, d_parado, 3, "Have you already met master Banger Yoda? He is right there, at the Knights of Order's head office.", "Master Banger is an incredible skillful warrior! They say that, during his youth, he defeated over 1000 enemies alone... That's how he got that eyepatch...", "With the Master here, we don't have to fear!"));
	TalkableNpc *caixeiro = new TalkableNpc((char*)"sprites/caixeiro.dat", 32, 32, 31*32, 4*32, 2, patrol, f_parado, 3, "Don't disturb me! Don't ya see I'm practicing?", "Who am I? Don't ya know me? I'm the legendary travelling salesman, young lady!", "I can't stop practicing! I gotta solve a NP-hard problem, using just my legs. =(");
	caixeiro->addWaypoint(coord(30*32, 4*32));
	caixeiro->addWaypoint(coord(26*32, 4*32));
	caixeiro->addWaypoint(coord(28*32, 6*32));
	city->insertEntity(caixeiro);
	//city->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 8*32, 17*32, 2, 100, 3, wander));
	//Borboletas
	city->insertEntity(new Npc((char*)"sprites/borboleta.dat", 0, 0, 1000, 900, 5, fly, 0));
	city->insertEntity(new Npc((char*)"sprites/borboleta.dat", 0, 0, 980, 900, 5, fly, 0));
	city->insertEntity(new Npc((char*)"sprites/borboleta.dat", 0, 0, 1000, 880, 5, fly, 0));
	city->insertEntity(new Npc((char*)"sprites/borboleta.dat", 0, 0, 980, 880, 5, fly, 0));
	city->insertEntity(new Npc((char*)"sprites/borboleta.dat", 0, 0, 1000, 850, 5, fly, 0));


	//Insercoes na casa da superior esquerda
	familyHouse->insertEntity(heroi);
	familyHouse->insertEntity(heroi2);
	familyHouse->insertEntity(new InvestigateNpc(6*32, 7*32, 25, 35, 2, "A stack of books. The ones who live here may study hard.", "Aff! I prefer to fight."));
	familyHouse->insertEntity(new InvestigateNpc(6*32, 3*32, 25, 35, 6, "To use your sword, press A.", "To use magic, just press S.", "You can choose which magic to use, pressing the numbers from 1 to 4. The chosen magic is showed beside your bars.", "Each experience item collected increases the current magic's experience. When the experience bar is entirely full, the current magic's power is increased!", "Watch out the enemies' attacks. Each attack taken decreases the current magic's experience.", "Holding X, you can run until your stamina bar (the orange one) goes empty."));
	familyHouse->insertEntity(new InvestigateNpc(12*32, 10*32, 25, 35, 1, "URGENT NEWS: After Bérun's destruction, now it was Fióren! Forces of Sinisterland broke down the walls of the city and completely destroyed the place. Many people died, but the royalty escaped to Bell Hill."));
	familyHouse->insertEntity(new TalkableNpc((char*)"sprites/random2.dat", 32, 32, 10*32, 7*32, 2, wander, d_parado, 2, "Ah! When will it end? I have just seen terrible news on the newspaper! If you wanna take a look, it's on the table...", "It's good to know how to protect yourself nowadays. If you wanna some tips, take a look at the book on the desk."));
	
	//Insercoes no qg
	qg->insertEntity(heroi);
	qg->insertEntity(heroi2);
	qg->insertEntity(new TalkableNpc((char*)"sprites/banger.dat", 32, 32, 5*32-7, 7*32-20, 3, halt, f_parado, 3,"Alice, you must collect the crystals and protect them! If they are destroyed, Sinisterland will destroy the world as we know it! I believe you are the only one able to manage this task...", "I heard rumors that one of the crystals is beyond the Slimes' Forest...", "I'm Banger. I kill thousands. =)"));
	qg->insertEntity(new TalkableNpc((char*)"sprites/banger_bodyguard.dat", 32, 32, 8*32, 6*32, 2, wander, f_parado, 2, "The citizens say to have seen a shadow walking on the Slimes' Forest. As the first general of the Knights of Order, it is my duty to check it!", "I have a bad feeling..."));
	qg->insertEntity(new TalkableNpc((char*)"sprites/mage_banger.dat", 32, 32, 10*32, 4*32, 2, wander, f_parado, 2, "We lost many of ours allies at Fióren...", "We can not give up! If Master Banger is with us, there is still hope!"));
	
	//Insercoes no inn
	inn->insertEntity(heroi);
	inn->insertEntity(heroi2);
	//NAO MUDAR A NURSE DE LUGAR TB!
	inn->insertEntity(new TalkableNpc((char*)"sprites/nurse.dat", 32, 32, 11*32, 9*32, 2, halt, f_parado, 1, "You look tired. Let me take care of you."));
	
	//Insercoes no mausoleum
	mausoleum->insertEntity(heroi);
	mausoleum->insertEntity(heroi2);	
	//BOB: NAO MUDAR
	mausoleum->insertEntity(new TalkableNpc((char*)"sprites/bob.dat", 32, 32, 10*32, 4*32, 2, halt, t_parado, 4, "Alice?! Yes, I haven't died! You are here for the crystal? Hahaha,  you don't understand the value of it for my master's plans, you fool!", "Die, pathetic Knight of Order!", "You also want to stop me? Even if you are my sister, we'll have to fight! No one dares get in my way, prepare yourself!", "Puf, puf... You have grown, my little sister, you became a great warrior. The crystal is yours, but you will never stop my master's progress. This will not be our last encounter!"));

	//Insercoes na floresta
	forest->insertEntity(heroi);
	forest->insertEntity(heroi2);
	//tumulos
	forest->insertEntity(new InvestigateNpc(62*32+16, 5*32, 40, 35, 1, "Michael Jackson, King of Pop, 1958-2009"));
	forest->insertEntity(new InvestigateNpc(59*32, 7*32, 32, 32, 1, "Dercy Gonçalves, famous for acting in the movie The Mummy, 1500 A.C. - 2008 D.C."));
	forest->insertEntity(new InvestigateNpc(57*32, 7*32, 32, 32, 1, "Galvão Bueno, endangered bird, 1500-2010"));
	forest->insertEntity(new InvestigateNpc(55*32, 7*32, 32, 32, 1, "Alan Turing, bronze powder nerd, 1912-1954"));
	forest->insertEntity(new InvestigateNpc(55*32, 16*32, 32, 32, 1, "Sir 'Stop looking for easter eggs e keep playing!!!', 1654 - 1702"));
/*	forest->insertEntity(new InvestigateNpc(57*32, 16*32, 32, 32, 1, ""));
	forest->insertEntity(new InvestigateNpc(59*32, 16*32, 32, 32, 1, ""));
	forest->insertEntity(new InvestigateNpc(55*32, 5*32, 32, 32, 1, ""));
	forest->insertEntity(new InvestigateNpc(57*32, 5*32, 32, 32, 1, ""));
	forest->insertEntity(new InvestigateNpc(59*32, 5*32, 32, 32, 1, ""));
	forest->insertEntity(new InvestigateNpc(62*32, 12*32, 32, 32, 1, ""));
	forest->insertEntity(new InvestigateNpc(63*32, 12*32, 32, 32, 1, ""));
	forest->insertEntity(new InvestigateNpc(62*32, 9*32, 32, 32, 1, ""));
	forest->insertEntity(new InvestigateNpc(63*32, 9*32, 32, 32, 1, ""));
	*/
	
	//primeira reta
	forest->insertEntity(new TalkableNpc((char*)"sprites/city_guard.dat", 32, 32, 5*32, 54*32, 2, halt, f_parado, 1, "If you are gonna follow  this trail, take care. Slimes like to hide in the shadows."));	
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 25*32, 54*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 23*32, 53*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 25*32, 52*32, 1, 3, 3, wander));
	//em cima da escada
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 22*32, 45*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 24*32, 45*32, 2, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 26*32, 45*32, 1, 3, 3, wander));
	//esquerda da escada
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 16*32, 40*32, 2, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 12*32, 40*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 12*32, 46*32, 2, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 15*32, 46*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 14*32, 43*32, 1, 5, 3, wander));
	//direita da escada
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 38*32, 46*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 38*32, 51*32, 2, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 43*32, 47*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 36*32, 44*32, 1, 5, 3, wander));
	//reta após descer escada
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 49*32, 51*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 48*32, 54*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 53*32, 52*32, 2, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 54*32, 54*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 58*32, 51*32, 2, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 63*32, 51*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 71*32, 51*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 71*32, 54*32, 1, 3, 3, wander));
	//acima da escada
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 49*32, 41*32, 2, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 53*32, 42*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 56*32, 40*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 62*32, 41*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 64*32, 43*32, 2, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 69*32, 43*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 68*32, 40*32, 1, 3, 3, wander));
	//acima proxima escada
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 68*32, 31*32, 1, 3, 3, wander));
//	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 65*32, 33*32, 2, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 63*32, 31*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 60*32, 33*32, 2, 3, 3, wander));
//	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 57*32, 30*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 50*32, 30*32, 1, 3, 3, wander));
	//grande area de arvores grandes
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 43*32, 36*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 43*32, 33*32, 2, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 40*32, 35*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 34*32, 27*32, 1, 5, 3, wander));
//	forest->insertEntity(new Enemy((char*)"sprites/bichor.dat", 32, 32, 39*32, 25*32, 2, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 39*32, 28*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bichor.dat", 32, 32, 31*32, 34*32, 2, 5, 3, wander));
//	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 28*32, 31*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 25*32, 35*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bichor.dat", 32, 32, 24*32, 26*32, 2, 5, 3, wander));
//	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 27*32, 24*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 30*32, 27*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bichor.dat", 32, 32, 31*32, 30*32, 2, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 43*32, 26*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 22*32, 34*32, 1, 5, 3, wander));
//	forest->insertEntity(new Enemy((char*)"sprites/bichor.dat", 32, 32, 20*32, 27*32, 2, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 16*32, 24*32, 1, 5, 3, wander));
	//area de subida pos grande arvores
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20.dat", 19, 20, 11*32, 24*32, 1, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 8*32, 22*32, 2, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 10*32, 18*32, 2, 3, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/slimey19x20r.dat", 19, 20, 8*32, 13*32, 2, 3, 3, wander));
	//sobe escada
	forest->insertEntity(new Enemy((char*)"sprites/bichor.dat", 32, 32, 9*32, 7*32, 2, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 17*32, 5*32, 1, 5, 3, wander));
	//area grande
//	forest->insertEntity(new Enemy((char*)"sprites/bichor.dat", 32, 32, 25*32, 7*32, 2, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 25*32, 11*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bichor.dat", 32, 32, 30*32, 13*32, 2, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bicho.dat", 32, 32, 31*32, 7*32, 1, 5, 3, wander));
	forest->insertEntity(new Enemy((char*)"sprites/bichor.dat", 32, 32, 26*32, 9*32, 2, 5, 3, wander));
	//vigiando cemiterio
	Enemy *patrollingEnemy1 = new Enemy((char*)"sprites/bicho.dat", 32, 32, 38*32, 10*32, 1, 5, 3, patrol);
	patrollingEnemy1->addWaypoint(coord(38*32, 5*32));
	patrollingEnemy1->addWaypoint(coord(38*32, 10*32));
	forest->insertEntity(patrollingEnemy1);
	Enemy *patrollingEnemy2 = new Enemy((char*)"sprites/bichor.dat", 32, 32, 42*32, 6*32, 2, 5, 3, patrol);
	patrollingEnemy2->addWaypoint(coord(42*32, 6*32));
	patrollingEnemy2->addWaypoint(coord(42*32, 9*32));
	forest->insertEntity(patrollingEnemy2);
	
	/* Algumas variaveis :D */
	frame_counter = 0;// A counter for which frame to draw
	timer = 0;
	key_delay = 0;
	
	bool controlMultiplayer = true;
	
	srand(time(NULL));
	music = load_midi("music/titlescreen.mid");
	set_volume(-1, 100);
	play_midi(music, 1);
	while(!key[KEY_ENTER] && !key[KEY_ESC]){
		blit(titleScreen, screen, 0, 0, 0, 0,640,480); //Desenha o buffer na tela
	}
	music = load_midi("music/millenialfair.mid");
	set_volume(-1, 100);
	play_midi(music, 1);
	while(!key[KEY_ESC]) {
		clear_keybuf();
		if(!heroi->isAlive()) break;
		/*Logica do speed_counter*/
		while(speed_counter > 0) {
			if(key[KEY_TAB] && controlMultiplayer) {
				changeMultiplayer(multiplayer, (*(myscreen.getWorld()->getEntities()))[0], (*(myscreen.getWorld()->getEntities()))[1]); 
				controlMultiplayer = false;
			}
			if(!key[KEY_TAB] && !controlMultiplayer) controlMultiplayer = true;
			Entity::doControl();
			myscreen.updatePosition(*heroi);
			//Warp da cidade para casa familyHouse
			if(myscreen.getWorld() == city && city->inCell(*heroi, 4, 13, _up)) {
				myscreen.setWorld(*familyHouse);
				heroi->setPosition(coord(10*32, 13*32));
				heroi2->setPosition(coord(11*32, 13*32));
				heroi2->updateState(t_parado);
			}
			//Warp da cidade para Inn
			if(myscreen.getWorld() == city && city->inCell(*heroi, 16, 16, _up)) {
				myscreen.setWorld(*inn);
				heroi->setPosition(coord(10*32+12, 13*32));
				heroi2->setPosition(coord(9*32+12, 13*32));
				heroi2->updateState(t_parado);
			}
			//Warp da cidade para QG
			if(myscreen.getWorld() == city && city->inCell(*heroi, 8, 27, _up)) {
				myscreen.setWorld(*qg);
				heroi->setPosition(coord(15*32+12, 11*32+15));
				heroi2->setPosition(coord(16*32+12, 11*32+15));
				heroi2->updateState(t_parado);
			}
			//Warp da cidade para floresta
			if(myscreen.getWorld() == city && city->inBorder(*heroi, _right)) {
				myscreen.setWorld(*forest);
				heroi->setPosition(coord(2*32, 55*32));
				heroi2->setPosition(coord(2*32, 56*32));
				heroi2->updateState(d_parado);
				music = load_midi("music/greenhill.mid");
				set_volume(-1, 100);
				play_midi(music, 1);
			}
			//Evento na cidade para Alice indo para mapa de cima (proibido)
			if(myscreen.getWorld() == city && (city->inBorder(*heroi, _up) || !city->getEventHandler())) {
				cityEvent(city, *(city->getEntities()), 5);			
			}
			//Warp do QG para a cidade
			if(myscreen.getWorld() == qg && (qg->inCell(*heroi, 15, 12, _down) || qg->inCell(*heroi, 16, 12, _down))) {
				myscreen.setWorld(*city);
				heroi->setPosition(coord(8*32+12, 28*32-15));
				heroi2->setPosition(coord(7*32+12, 28*32-15));
				heroi2->updateState(f_parado);
			}
			//Evento do INN
			if(myscreen.getWorld() == inn && ((heroi->distance(*(*inn->getEntities())[2]) <= 50 && key[KEY_Z]) || (heroi2->distance(*(*inn->getEntities())[2]) <= 50 && key[KEY_C]) || !qg->getEventHandler())) {
				innEvent(inn, *(inn->getEntities()), 2, multiplayer);				
			}
			//Warp da familyHouse para a cidade
			if(myscreen.getWorld() == familyHouse && familyHouse->inBorder(*heroi, _down)) {
				myscreen.setWorld(*city);
				heroi->setPosition(coord(4*32+12, 14*32));
				heroi2->setPosition(coord(3*32+12, 14*32));
				heroi2->updateState(f_parado);
			}
			//Warp do Inn para a cidade
			if(myscreen.getWorld() == inn && inn->inBorder(*heroi, _down)) {
				myscreen.setWorld(*city);
				heroi->setPosition(coord(16*32+12, 17*32));	
				heroi2->setPosition(coord(15*32+12, 17*32));
				heroi2->updateState(f_parado);	
			}
			//Warp da floresta para a cidade
			if(myscreen.getWorld() == forest && forest->inBorder(*heroi, _left)) {
				myscreen.setWorld(*city);
				heroi->setPosition(coord(48*32, 25*32));
				heroi2->setPosition(coord(48*32, 26*32));
				heroi2->updateState(e_parado);
				music = load_midi("music/millenialfair.mid");
				set_volume(-1, 100);
				play_midi(music, 1);
			}
			//Warp da floresta para o mausoleum
			if(myscreen.getWorld() == forest && (forest->inCell(*heroi, 67, 13, _up) || forest->inCell(*heroi, 68, 13, _up))) {
				myscreen.setWorld(*mausoleum);
				heroi->setPosition(coord(10*32, 28*32));
				heroi2->setPosition(coord(11*32, 28*32));
				heroi2->updateState(t_parado);
	      music = load_midi("music/magusbattle.mid");
	      set_volume(-1, 100);
	      play_midi(music, 1);
			}
			//Warp do mausoleum para a floresta
			if(myscreen.getWorld() == mausoleum && mausoleum->inBorder(*heroi, _down)) {
				myscreen.setWorld(*forest);
				heroi->setPosition(coord(68*32, 14*32));
				heroi2->setPosition(coord(69*32, 14*32));
				heroi2->updateState(f_parado);
				music = load_midi("music/greenhill.mid");
				set_volume(-1, 100);
				play_midi(music, 1);
			}
			//Evento do mausoleum
			if(myscreen.getWorld() == mausoleum && ( heroi->distance(*(*mausoleum->getEntities())[2]) <= 300 || (heroi2->isAlive() && heroi2->distance(*(*mausoleum->getEntities())[2]) <= 300)) && mausoleum->getEventHandler()) {
				bobEvent(mausoleum, *(mausoleum->getEntities()), 2);
				if(mausoleum->getEventHandler() == false) goto hehehe;
			}
			speed_counter--;
			if(++timer == 300){
				timer = 0;
			} 
			if(timer%20 == 0){if(++frame_counter == 300) frame_counter = 0;}
			key_delay--;
			
			myscreen.selectVisible();
			myscreen.getWorld()->handleCollision();
		}
		
		/*Desenho*/
		myscreen.draw(heroi, heroi2, multiplayer);
		myscreen.buffer2screen();
		myscreen.clearBuffer(); //Limpa o buffer
	}
hehehe:
	if(heroi->isAlive()) {
		while(!key[KEY_ESC]){
			blit(fim, screen, 0, 0, 0, 0,640,480); //Desenha o buffer na tela
		}				
	}
	else {
		music = load_midi("music/gameover.mid");
		set_volume(-1, 100);
		play_midi(music, 1);		
		while(!key[KEY_ESC]){
			blit(gameover, screen, 0, 0, 0, 0,640,480); //Desenha o buffer na tela
		}		
	}
	
	return(0);
	
}
END_OF_MAIN()
