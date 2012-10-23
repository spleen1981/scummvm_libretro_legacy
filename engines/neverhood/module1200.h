/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef NEVERHOOD_MODULE1200_H
#define NEVERHOOD_MODULE1200_H

#include "neverhood/neverhood.h"
#include "neverhood/module.h"
#include "neverhood/scene.h"

namespace Neverhood {

// Module1200

class Module1200 : public Module {
public:
	Module1200(NeverhoodEngine *vm, Module *parentModule, int which);
	virtual ~Module1200();
protected:
	int _sceneNum;
	void createScene(int sceneNum, int which);
	void updateScene();
};

// Scene1201

class AsScene1201Tape : public AnimatedSprite {
public:
	AsScene1201Tape(NeverhoodEngine *vm, Scene *parentScene, uint32 nameHash, int surfacePriority, int16 x, int16 y, uint32 fileHash);
protected:
	Scene *_parentScene;
	uint32 _nameHash;
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
};

class AsScene1201TntManRope : public AnimatedSprite {
public:
	AsScene1201TntManRope(NeverhoodEngine *vm, bool isDummyHanging);
protected:
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
};

class AsScene1201RightDoor : public AnimatedSprite {
public:
	AsScene1201RightDoor(NeverhoodEngine *vm, Sprite *klayman, bool isOpen);
protected:
	Sprite *_klayman;
	int _countdown;
	void update();
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
	void stOpenDoor();
	void stCloseDoor();
	void stCloseDoorDone();
};

class AsScene1201KlaymanHead : public AnimatedSprite {
public:
	AsScene1201KlaymanHead(NeverhoodEngine *vm);
protected:
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
};

class AsScene1201TntMan : public AnimatedSprite {
public:
	AsScene1201TntMan(NeverhoodEngine *vm, Scene *parentScene, Sprite *asTntManRope, bool isDown);
	virtual ~AsScene1201TntMan();
protected:
	Scene *_parentScene;
	Sprite *_asTntManRope;
	Sprite *_sprite;
	bool _isMoving;
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
	uint32 hmComingDown(int messageNum, const MessageParam &param, Entity *sender);
	void suMoving();
	void stStanding();
	void stComingDown();
	void stMoving();
};

class AsScene1201TntManFlame : public AnimatedSprite {
public:
	AsScene1201TntManFlame(NeverhoodEngine *vm, Sprite *asTntMan);
	~AsScene1201TntManFlame();
protected:
	Sprite *_asTntMan;
	void update();
	void suUpdate();
};

class AsScene1201Match : public AnimatedSprite {
public:
	AsScene1201Match(NeverhoodEngine *vm, Scene *parentScene);
protected:
	Scene *_parentScene;
	int _countdown;
	int _status;
	void update();
	uint32 hmOnDoorFrameAboutToMove(int messageNum, const MessageParam &param, Entity *sender);
	uint32 hmOnDoorFrameMoving(int messageNum, const MessageParam &param, Entity *sender);
	uint32 hmIdle(int messageNum, const MessageParam &param, Entity *sender);
	void stOnDoorFrameMoving();
	void stFallingFromDoorFrame();
	void stOnDoorFrameAboutToMove();
	void stIdleOnDoorFrame();
	void stIdleOnFloor();
};

class AsScene1201Creature : public AnimatedSprite {
public:
	AsScene1201Creature(NeverhoodEngine *vm, Scene *parentScene, Sprite *klayman);
protected:
	Scene *_parentScene;
	Sprite *_klayman;
	int _countdown;
	bool _klaymanTooClose;
	void update();
	uint32 hmWaiting(int messageNum, const MessageParam &param, Entity *sender);
	uint32 hmPincerSnap(int messageNum, const MessageParam &param, Entity *sender);
	uint32 hmPincerSnapKlayman(int messageNum, const MessageParam &param, Entity *sender);
	void stWaiting();
	void stPincerSnap();
	void stStartReachForTntDummy();
	void stReachForTntDummy();
	void stPincerSnapKlayman();
};

class AsScene1201LeftDoor : public AnimatedSprite {
public:
	AsScene1201LeftDoor(NeverhoodEngine *vm, Sprite *klayman);
protected:
	Sprite *_klayman;
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
	void stCloseDoor();
};

class SsScene1201Tnt : public StaticSprite {
public:
	SsScene1201Tnt(NeverhoodEngine *vm, uint32 elemIndex, uint32 pointIndex, int16 clipY2);
protected:
	uint32 _elemIndex;
};

class Scene1201 : public Scene {
public:
	Scene1201(NeverhoodEngine *vm, Module *parentModule, int which);
	virtual ~Scene1201();
protected:
	Sprite *_asMatch;
	AsScene1201TntMan *_asTntMan;
	Sprite *_asCreature;
	Sprite *_asTntManRope;
	Sprite *_asLeftDoor;
	Sprite *_asRightDoor;
	Sprite *_asTape;
	Sprite *_asKlaymanHead;
	bool _creatureExploded;
	void update();
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
};

// Scene1202

class AsScene1202TntItem : public AnimatedSprite {
public:
	AsScene1202TntItem(NeverhoodEngine *vm, Scene *parentScene, int index);
protected:
	Scene *_parentScene;
	int _itemIndex, _newPosition;
	uint32 hmShowIdle(int messageNum, const MessageParam &param, Entity *sender);
	uint32 hmChangePosition(int messageNum, const MessageParam &param, Entity *sender);
	void stShowIdle();
	void stChangePositionFadeOut();
	void stChangePositionFadeIn();
	void stChangePositionDone();
};

class Scene1202 : public Scene {
public:
	Scene1202(NeverhoodEngine *vm, Module *parentModule);
	virtual ~Scene1202();
protected:
	PaletteResource _paletteResource;
	Sprite *_asTntItems[18];
	int _counter;
	int _clickedIndex;
	byte _paletteData[1024];
	bool _isPuzzleSolved;
	bool _soundToggle;
	void update();
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
	uint32 hmSolved(int messageNum, const MessageParam &param, Entity *sender);
	bool isSolved();
};

} // End of namespace Neverhood

#endif /* NEVERHOOD_MODULE1200_H */
