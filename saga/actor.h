/* ScummVM - Scumm Interpreter
 * Copyright (C) 2004 The ScummVM project
 *
 * The ReInherit Engine is (C)2000-2003 by Daniel Balsom.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

// Actor management module header file

#ifndef SAGA_ACTOR_H__
#define SAGA_ACTOR_H__

#include "saga/sprite.h"
#include "saga/actordata.h"
#include "saga/list.h"

namespace Saga {

#define ACTOR_BASE_SPEED 0.25
#define ACTOR_BASE_ZMOD 0.5

#define ACTOR_DEFAULT_ORIENT 2

#define ACTOR_ACTIONTIME 80

#define ACTOR_DIALOGUE_LETTERTIME 50
#define ACTOR_DIALOGUE_HEIGHT 100

#define ACTOR_LMULT 4

#define ACTOR_ORIENTATION_COUNT 4

#define IS_VALID_ACTOR_INDEX(index) ((index >= 0) && (index < ACTORCOUNT))
#define IS_VALID_ACTOR_ID(id) ((id == 1) || (id >= 0x2000) && (id < (0x2000 | ACTORCOUNT)))
#define ACTOR_ID_TO_INDEX(id) ((((uint16)id) == 1 ) ? 0 : (int)(((uint16)id) & ~0x2000))
#define ACTOR_INDEX_TO_ID(index) ((((int)index) == 0 ) ? 1 : (uint16)(((int)index) | 0x2000))

enum ACTOR_INTENTS {
	INTENT_NONE = 0,
	INTENT_PATH = 1,
	INTENT_SPEAK = 2
};

enum ACTOR_WALKFLAGS {
	WALK_NONE = 0x00,
	WALK_NOREORIENT = 0x01
};

enum ACTOR_ORIENTATIONS {
	ORIENT_N = 0,
	ORIENT_NE = 1,
	ORIENT_E = 2,
	ORIENT_SE = 3,
	ORIENT_S = 4,
	ORIENT_SW = 5,
	ORIENT_W = 6,
	ORIENT_NW = 7
};

enum ACTOR_ACTIONS {
	ACTION_IDLE = 0,
	ACTION_WALK = 1,
	ACTION_SPEAK = 2,
	ACTION_COUNT
};

enum ACTOR_ACTIONFLAGS {
	ACTION_NONE = 0x00,
	ACTION_LOOP = 0x01
};

struct ActorOrientation {
	int frameIndex;
	int frameCount;
};

struct ActorFrame {
	ActorOrientation dir[ACTOR_ORIENTATION_COUNT];
};

struct WALKNODE {
	int calc_flag;
	Point node_pt;
	WALKNODE() {
		calc_flag = 0;
	}
};

typedef Common::List<WALKNODE> WalkNodeList;

struct WALKINTENT {
	int wi_active;
	uint16 wi_flags;
	int wi_init;

	int time;
	float slope;
	int x_dir;
	Point org;
	Point cur;

	Point dst_pt;
	WalkNodeList nodelist;

	int sem_held;
	SEMAPHORE *sem;

	WALKINTENT() { 
		wi_active = 0;
		wi_flags = 0;
		wi_init = 0;

		time = 0;
		slope = 0;
		x_dir = 0;

		sem_held = 0;
		sem = NULL;
	}
};


struct ACTORDIALOGUE {
	int d_playing;
	const char *d_string;
	uint16 d_voice_rn;
	long d_time;
	int d_sem_held;
	SEMAPHORE *d_sem;
	ACTORDIALOGUE() { memset(this, 0, sizeof(*this)); }
};

typedef Common::List<ACTORDIALOGUE> ActorDialogList;


struct ACTORINTENT {
	int a_itype;
	uint16 a_iflags;
	int a_idone;

	int si_init;
	uint16 si_flags;
	int si_last_action;
	ActorDialogList si_diaglist;	/* Actor dialogue list */

	WALKINTENT walkIntent;

	ACTORINTENT() {
		a_itype = 0;
		a_iflags = 0;
		a_idone = 0;

		si_init = 0;
		si_flags = 0;
		si_last_action = 0;
	}
};

typedef Common::List<ACTORINTENT> ActorIntentList;

struct ActorData {
	bool disabled;				// Actor disabled in init section
	int index;					// Actor index
	uint16 actorId;				// Actor id
	int nameIndex;				// Actor's index in actor name string list
	byte speechColor;			// Actor dialogue color
	uint16 flags;				// Actor flags
	
	int sceneNumber;			// scene of actor
	int actorX;					// Actor's logical coordinates
	int actorY;					// 
	int actorZ;					// 

	Point screenPosition;		// Actor's screen coordinates
	int screenDepth;			//
	int screenScale;			//

	int currentAction;		
	int facingDirection;
	int actionDirection;
	
	SPRITELIST *spriteList;		// Actor's sprite list data
	int spriteListResourceId;	// Actor's sprite list resource id

	ActorFrame *frames;			// Actor's frames
	int framesCount;			// Actor's frames count
	int frameListResourceId;	// Actor's frame list resource id




	int idle_time;
	int orient;
	int speaking;

	
	// The actor intent list describes what the actor intends to do;
	// multiple intents can be queued. The actor must complete an 
	// intent before moving on to the next; thus actor movements, esp
	// as described from scripts, can be serialized

	ActorIntentList a_intentlist;

	// WALKPATH path;

	int def_action;
	uint16 def_action_flags;

	int action;
	uint16 action_flags;
	int action_frame;
	int action_time;


	ActorData() {
		disabled = false;
		index = 0;
		actorId = 0;
		nameIndex = 0;
		currentAction = 0;
		facingDirection = 0;
		actionDirection = 0;
		speechColor = 0;
		frames = NULL;
		framesCount = 0;
		frameListResourceId = 0;
		spriteList = NULL;
		spriteListResourceId = 0;
		flags = 0;
		sceneNumber = 0;
		actorX = 0;
		actorY = 0;
		actorZ = 0;
		screenDepth = 0;

		idle_time = 0;
		orient = 0;
		speaking = 0;
		def_action = 0;
		def_action_flags = 0;
		action = 0;
		action_flags = 0;
		action_frame = 0;
		action_time = 0;
	}
};

typedef ActorData* ActorDataPointer;
typedef SortedList<ActorDataPointer> ActorOrderList;


struct ACTIONTIMES {
	int action;
	int time;
};

class Actor {
public:
	Actor(SagaEngine *vm);
	~Actor();

	void CF_actor_move(int argc, const char **argv);
	void CF_actor_moverel(int argc, const char **argv);
	void CF_actor_seto(int argc, const char **argv);
	void CF_actor_setact(int argc, const char **argv);

	int direct(int msec);
	int drawActors();
	void updateActorsScene();			// calls from scene loading to update Actors info

	void AtoS(Point &screenPoint, const Point &actorPoint);
	void StoA(Point &actorPoint, const Point &screenPoint);

	void move(uint16 actorId, const Point &movePoint);
	void moveRelative(uint16 actorId, const Point &movePoint);

	void walkTo(uint16 actorId, const Point *walk_pt, uint16 flags, SEMAPHORE *sem);
		
	void speak(uint16 actorId, const char *d_string, uint16 d_voice_rn, SEMAPHORE *sem);
	
	int skipDialogue();
	
	int getSpeechTime(const char *d_string, uint16 d_voice_rn);
	void setOrientation(uint16 actorId, int orient);
	void setAction(uint16 actorId, int action_n, uint16 action_flags);
	void setDefaultAction(uint16 actorId, int action_n, uint16 action_flags);

	
private:
	int handleWalkIntent(ActorData *actor, WALKINTENT *a_walk_int, int *complete_p, int msec);
	int handleSpeakIntent(ActorData *actor, ACTORINTENT *a_aintent, int *complete_p, int msec);
	int setPathNode(WALKINTENT *walk_int, const Point &src_pt, Point *dst_pt, SEMAPHORE *sem);

	ActorData *getActor(uint16 actorId);

	bool loadActorResources(ActorData * actor);
	
	void createDrawOrderList();

	SagaEngine *_vm;
	RSCFILE_CONTEXT *_actorContext;
	ActorOrderList _drawOrderList;
	ActorData _actors[ACTORCOUNT];
};

} // End of namespace Saga

#endif
