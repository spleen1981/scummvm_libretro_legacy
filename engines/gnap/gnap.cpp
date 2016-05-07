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

#include "gnap/gnap.h"
#include "gnap/datarchive.h"
#include "gnap/gamesys.h"
#include "gnap/resource.h"
#include "gnap/sound.h"

#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/error.h"
#include "common/fs.h"
#include "common/timer.h"

#include "engines/util.h"

#include "graphics/cursorman.h"

namespace Gnap {

static const int kCursors[] = {
	LOOK_CURSOR,
	GRAB_CURSOR,
	TALK_CURSOR,
	PLAT_CURSOR
};

static const int kDisabledCursors[] = {
	NOLOOK_CURSOR,
	NOGRAB_CURSOR,
	NOTALK_CURSOR,
	NOPLAT_CURSOR
};

static const char *kCursorNames[] = {
	"LOOK_CURSOR",
	"GRAB_CURSOR",
	"TALK_CURSOR",
	"PLAT_CURSOR",
	"NOLOOK_CURSOR",
	"NOGRAB_CURSOR",
	"NOTALK_CURSOR",
	"NOPLAT_CURSOR",
	"EXIT_L_CURSOR",
	"EXIT_R_CURSOR",
	"EXIT_U_CURSOR",
	"EXIT_D_CURSOR",
	"EXIT_NE_CURSOR",
	"EXIT_NW_CURSOR",
	"EXIT_SE_CURSOR",
	"EXIT_SW_CURSOR",
	"WAIT_CURSOR"
};


static const int kCursorSpriteIds[30] = {
	0x005, 0x008, 0x00A, 0x004, 0x009, 0x003,
	0x006, 0x007, 0x00D, 0x00F, 0x00B, 0x00C,
	0x019, 0x01C, 0x015, 0x014, 0x010, 0x01A,
	0x018, 0x013, 0x011, 0x012, 0x01B, 0x016,
	0x017, 0x01D, 0x01E, 0x01F, 0x76A, 0x76B
};

static const char *kSceneNames[] = {
	"open", "pigpn", "truck", "creek", "mafrm", "frbrn", "inbrn", "crash",
	"porch", "barbk", "kitch", "bar", "juke", "wash", "john", "jkbox",
	"brawl", "stret", "frtoy", "intoy", "frgro", "park", "cash", "ingro",
	"frcir", "booth", "circ", "outcl", "incln", "monk", "elcir", "beer",
	"pig2", "trk2", "creek", "frbrn", "inbrn", "mafrm", "infrm", "efair",
	"fair", "souv", "chick", "ship", "kiss", "disco", "boot", "can",
	"can2", "drive", "tung", "puss", "space", "phone", "can3"
};

GnapEngine::GnapEngine(OSystem *syst, const ADGameDescription *gd) :
	Engine(syst), _gameDescription(gd) {

	_random = new Common::RandomSource("gnap");
	DebugMan.addDebugChannel(kDebugBasic, "basic", "Basic debug level");

	Engine::syncSoundSettings();
	_scene = nullptr;
}

GnapEngine::~GnapEngine() {
	delete _random;
}

Common::Error GnapEngine::run() {
	// Initialize the graphics mode to ARGB8888
	Graphics::PixelFormat format = Graphics::PixelFormat(4, 8, 8, 8, 8, 16, 8, 0, 24);
	initGraphics(800, 600, true, &format);

	// We do not support color conversion yet
	if (_system->getScreenFormat() != format)
		return Common::kUnsupportedColorMode;

	_lastUpdateClock = 0;

	// >>>>> Variable initialization
	_cursorIndex = -1;
	_verbCursor = 1;

	_loadGameSlot = -1;
	if (ConfMan.hasKey("save_slot"))
		_loadGameSlot = ConfMan.getInt("save_slot");

	invClear();
	clearFlags();

	_grabCursorSprite = nullptr;
	_newGrabCursorSpriteIndex = -1;
	_backgroundSurface = nullptr;
	_isStockDatLoaded = false;
	_gameDone = false;
	_isPaused = false;
	_pauseSprite = nullptr;

	////////////////////////////////////////////////////////////////////////////

	_exe = new Common::PEResources();
	if (!_exe->loadFromEXE("ufos.exe"))
		error("Could not load ufos.exe");

	_dat = new DatManager();
	_spriteCache = new SpriteCache(_dat);
	_soundCache = new SoundCache(_dat);
	_sequenceCache = new SequenceCache(_dat);
	_gameSys = new GameSys(this);
	_soundMan = new SoundMan(this);
	_debugger = new Debugger(this);
	_gnap = new PlayerGnap(this);
	_plat = new PlayerPlat(this);

	_menuBackgroundSurface = nullptr;

	initGlobalSceneVars();
	mainLoop();

	delete _plat;
	delete _gnap;
	delete _soundMan;
	delete _gameSys;
	delete _sequenceCache;
	delete _soundCache;
	delete _spriteCache;
	delete _dat;
	delete _debugger;
	delete _exe;

	return Common::kNoError;
}

void GnapEngine::updateEvents() {
	Common::Event event;

	while (_eventMan->pollEvent(event)) {
		switch (event.type) {
		case Common::EVENT_KEYDOWN:
			// Check for debugger
			if (event.kbd.keycode == Common::KEYCODE_d && (event.kbd.flags & Common::KBD_CTRL)) {
				// Attach to the debugger
				_debugger->attach();
				_debugger->onFrame();
			}

			_keyPressState[event.kbd.keycode] = 1;
			_keyDownState[event.kbd.keycode] = 1;
			break;
		case Common::EVENT_KEYUP:
			_keyDownState[event.kbd.keycode] = 0;
  			break;
		case Common::EVENT_MOUSEMOVE:
  			_mouseX = event.mouse.x;
  			_mouseY = event.mouse.y;
  			break;
		case Common::EVENT_LBUTTONUP:
  			_mouseButtonState._left = false;
  			break;
		case Common::EVENT_LBUTTONDOWN:
			_leftClickMouseX = event.mouse.x;
			_leftClickMouseY = event.mouse.y;
			_mouseButtonState._left = true;
			_mouseClickState._left = true;
  			break;
		case Common::EVENT_RBUTTONUP:
			_mouseButtonState._right = false;
  			break;
		case Common::EVENT_RBUTTONDOWN:
  			_mouseButtonState._right = true;
  			_mouseClickState._right = true;
  			break;
		case Common::EVENT_QUIT:
			quitGame();
			break;
		default:
			break;
		}
	}
}

void GnapEngine::gameUpdateTick() {
	updateEvents();

	// TODO Check _gameDone in the various game loops
	if (shouldQuit()) {
		_gameDone = true;
		_sceneDone = true;
	}

	// TODO Improve this (variable frame delay to keep ~15fps)
	int currClock = _system->getMillis();
	if (currClock >= _lastUpdateClock + 66) {
		_gameSys->fatUpdate();
		_gameSys->drawSprites();
		_gameSys->updateScreen();
		_gameSys->updatePlaySounds();
		_gameSys->_gameSysClock++;
		updateTimers();
		_lastUpdateClock = currClock;
	}

	_soundMan->update();
	_system->updateScreen();
	_system->delayMillis(5);
}

void GnapEngine::saveTimers() {
	for (int i = 0; i < kMaxTimers; ++i )
		_savedTimers[i] = _timers[i];
}

void GnapEngine::restoreTimers() {
	for (int i = 0; i < kMaxTimers; ++i )
		_timers[i] = _savedTimers[i];
}

void GnapEngine::pauseGame() {
	if (!_isPaused) {
		saveTimers();
		hideCursor();
		setGrabCursorSprite(-1);
		_pauseSprite = _gameSys->createSurface(0x1076C);
		_gameSys->insertSpriteDrawItem(_pauseSprite, (800 - _pauseSprite->w) / 2, (600 - _pauseSprite->h) / 2, 356);
		_lastUpdateClock = 0;
		gameUpdateTick();
		// TODO playMidi("pause.mid");
		_isPaused = true;
	}
}

void GnapEngine::resumeGame() {
	if (_isPaused) {
		restoreTimers();
		_gameSys->removeSpriteDrawItem(_pauseSprite, 356);
		_lastUpdateClock = 0;
		gameUpdateTick();
		deleteSurface(&_pauseSprite);
		// TODO stopMidi();
		_isPaused = false;
		clearAllKeyStatus1();
		_mouseClickState._left = false;
		_mouseClickState._right = false;
		showCursor();
		_gameSys->_gameSysClock = 0;
		_gameSys->_lastUpdateClock = 0;
	}
}

void GnapEngine::updatePause() {
	while (_isPaused) {
		gameUpdateTick();
		if (isKeyStatus1(Common::KEYCODE_p)) {
			clearKeyStatus1(Common::KEYCODE_p);
			resumeGame();
		}
		//_system->delayMillis(100);
	}
}

int GnapEngine::getRandom(int max) {
	return _random->getRandomNumber(max - 1);
}

int GnapEngine::readSavegameDescription(int savegameNum, Common::String &description) {
	description = Common::String::format("Savegame %d", savegameNum);
	return 0;
}

int GnapEngine::loadSavegame(int savegameNum) {
	return 1;
}

void GnapEngine::delayTicks(int a1) {
	// TODO
	gameUpdateTick();
}

void GnapEngine::delayTicksCursor(int a1) {
	// TODO
	gameUpdateTick();
}

void GnapEngine::setHotspot(int index, int16 x1, int16 y1, int16 x2, int16 y2, uint16 flags,
	int16 walkX, int16 walkY) {
	_hotspots[index]._x1 = x1;
	_hotspots[index]._y1 = y1;
	_hotspots[index]._x2 = x2;
	_hotspots[index]._y2 = y2;
	_hotspots[index]._flags = flags;
	_hotspots[index]._id = index;
	_hotspotsWalkPos[index].x = walkX;
	_hotspotsWalkPos[index].y = walkY;
}

int GnapEngine::getHotspotIndexAtPos(int16 x, int16 y) {
	for (int i = 0; i < _hotspotsCount; ++i)
		if (!_hotspots[i].isFlag(SF_DISABLED) && _hotspots[i].isPointInside(x, y))
			return i;
	return -1;
}

void GnapEngine::updateCursorByHotspot() {
	if (!_isWaiting) {
		int hotspotIndex = getHotspotIndexAtPos(_mouseX, _mouseY);

		if (_debugger->_showHotspotNumber) {
			// NOTE This causes some display glitches so don't worry
			char t[256];
			sprintf(t, "hotspot = %d", hotspotIndex);
			_gameSys->fillSurface(0, 10, 10, 80, 16, 0, 0, 0);
			_gameSys->drawTextToSurface(0, 10, 10, 255, 255, 255, t);
		}

		if (hotspotIndex < 0)
			setCursor(kDisabledCursors[_verbCursor]);
		else if (_hotspots[hotspotIndex]._flags & SF_EXIT_L_CURSOR)
			setCursor(EXIT_L_CURSOR);
		else if (_hotspots[hotspotIndex]._flags & SF_EXIT_R_CURSOR)
			setCursor(EXIT_R_CURSOR);
		else if (_hotspots[hotspotIndex]._flags & SF_EXIT_U_CURSOR)
			setCursor(EXIT_U_CURSOR);
		else if (_hotspots[hotspotIndex]._flags & SF_EXIT_D_CURSOR)
			setCursor(EXIT_D_CURSOR);
		else if (_hotspots[hotspotIndex]._flags & SF_EXIT_NE_CURSOR)
			setCursor(EXIT_NE_CURSOR);
		else if (_hotspots[hotspotIndex]._flags & SF_EXIT_NW_CURSOR)
			setCursor(EXIT_NW_CURSOR);
		else if (_hotspots[hotspotIndex]._flags & SF_EXIT_SE_CURSOR)
			setCursor(EXIT_SE_CURSOR);
		else if (_hotspots[hotspotIndex]._flags & SF_EXIT_SW_CURSOR)
			setCursor(EXIT_SW_CURSOR);
		else if (_hotspots[hotspotIndex]._flags & (1 << _verbCursor))
			setCursor(kCursors[_verbCursor]);
		else
			setCursor(kDisabledCursors[_verbCursor]);
	}
	// Update platypus hotspot
	_hotspots[0]._x1 = _gridMinX + 75 * _plat->_pos.x - 30;
	_hotspots[0]._y1 = _gridMinY + 48 * _plat->_pos.y - 100;
	_hotspots[0]._x2 = _gridMinX + 75 * _plat->_pos.x + 30;
	_hotspots[0]._y2 = _gridMinY + 48 * _plat->_pos.y;
}

int GnapEngine::getClickedHotspotId() {
	int result = -1;
	if (_isWaiting)
		_mouseClickState._left = false;
	else if (_mouseClickState._left) {
		int hotspotIndex = getHotspotIndexAtPos(_leftClickMouseX, _leftClickMouseY);
		if (hotspotIndex >= 0) {
			_mouseClickState._left = false;
			_timers[3] = 300;
			result = _hotspots[hotspotIndex]._id;
		}
	}
	return result;
}

int GnapEngine::getInventoryItemSpriteNum(int index) {
	return kCursorSpriteIds[index];
}

void GnapEngine::updateMouseCursor() {
	if (_mouseClickState._right) {
		// Switch through the verb cursors
		_mouseClickState._right = false;
		_timers[3] = 300;
		_verbCursor = (_verbCursor + 1) % 4;
		if (!isFlag(kGFPlatypus) && _verbCursor == PLAT_CURSOR && _cursorValue == 1)
			_verbCursor = (_verbCursor + 1) % 4;
		if (!_isWaiting)
			setCursor(kDisabledCursors[_verbCursor]);
		setGrabCursorSprite(-1);
	}
	if (_isWaiting && ((_gnap->_actionStatus < 0 && _plat->_actionStatus < 0) || _sceneWaiting)) {
		setCursor(kDisabledCursors[_verbCursor]);
		_isWaiting = false;
	} else if (!_isWaiting && (_gnap->_actionStatus >= 0 || _plat->_actionStatus >= 0) && !_sceneWaiting) {
		setCursor(WAIT_CURSOR);
		_isWaiting = true;
	}
}

void GnapEngine::setVerbCursor(int verbCursor) {
	_verbCursor = verbCursor;
	if (!_isWaiting)
		setCursor(kDisabledCursors[_verbCursor]);
}

void GnapEngine::setCursor(int cursorIndex) {
	if (_cursorIndex != cursorIndex) {
		const char *cursorName = kCursorNames[cursorIndex];
		Graphics::WinCursorGroup *cursorGroup = Graphics::WinCursorGroup::createCursorGroup(*_exe, Common::WinResourceID(cursorName));
		if (cursorGroup) {
			Graphics::Cursor *cursor = cursorGroup->cursors[0].cursor;
			CursorMan.replaceCursor(cursor->getSurface(), cursor->getWidth(), cursor->getHeight(),
				cursor->getHotspotX(), cursor->getHotspotY(), cursor->getKeyColor());
			CursorMan.replaceCursorPalette(cursor->getPalette(), 0, 256);
			delete cursorGroup;
		}
		_cursorIndex = cursorIndex;
	}
}

void GnapEngine::showCursor() {
	CursorMan.showMouse(true);
}

void GnapEngine::hideCursor() {
	CursorMan.showMouse(false);
}

void GnapEngine::setGrabCursorSprite(int index) {
	freeGrabCursorSprite();
	if (index >= 0) {
		createGrabCursorSprite(makeRid(1, kCursorSpriteIds[index]));
		setVerbCursor(GRAB_CURSOR);
	}
	_grabCursorSpriteIndex = index;
}

void GnapEngine::createGrabCursorSprite(int spriteId) {
	_grabCursorSprite = _gameSys->createSurface(spriteId);
	_gameSys->insertSpriteDrawItem(_grabCursorSprite,
		_mouseX - (_grabCursorSprite->w / 2),
		_mouseY - (_grabCursorSprite->h / 2),
		300);
	delayTicks(5);
}

void GnapEngine::freeGrabCursorSprite() {
	if (_grabCursorSprite) {
		_gameSys->removeSpriteDrawItem(_grabCursorSprite, 300);
		_gameSys->removeSpriteDrawItem(_grabCursorSprite, 301);
		delayTicks(5);
		deleteSurface(&_grabCursorSprite);
	}
}

void GnapEngine::updateGrabCursorSprite(int x, int y) {
	if (_grabCursorSprite) {
		int newGrabCursorX = _mouseX - (_grabCursorSprite->w / 2) - x;
		int newGrabCursorY = _mouseY - (_grabCursorSprite->h / 2) - y;
		if (_currGrabCursorX != newGrabCursorX || _currGrabCursorY != newGrabCursorY) {
			_currGrabCursorX = newGrabCursorX;
			_currGrabCursorY = newGrabCursorY;
			Common::Rect rect(newGrabCursorX, newGrabCursorY,
				newGrabCursorX + _grabCursorSprite->w, newGrabCursorY + _grabCursorSprite->h);
			_gameSys->invalidateGrabCursorSprite(300, rect, _grabCursorSprite, _grabCursorSprite);
		}
	}
}

void GnapEngine::invClear() {
	_inventory = 0;
}

void GnapEngine::invAdd(int itemId) {
	_inventory |= (1 << itemId);
}

void GnapEngine::invRemove(int itemId) {
	_inventory &= ~(1 << itemId);
}

bool GnapEngine::invHas(int itemId) {
	return (_inventory & (1 << itemId)) != 0;
}

void GnapEngine::clearFlags() {
	_gameFlags = 0;
}

void GnapEngine::setFlag(int num) {
	_gameFlags |= (1 << num);
}

void GnapEngine::clearFlag(int num) {
	_gameFlags &= ~(1 << num);
}

bool GnapEngine::isFlag(int num) {
	return (_gameFlags & (1 << num)) != 0;
}

Graphics::Surface *GnapEngine::addFullScreenSprite(int resourceId, int id) {
	_fullScreenSpriteId = id;
	_fullScreenSprite = _gameSys->createSurface(resourceId);
	_gameSys->insertSpriteDrawItem(_fullScreenSprite, 0, 0, id);
	return _fullScreenSprite;
}

void GnapEngine::removeFullScreenSprite() {
	_gameSys->removeSpriteDrawItem(_fullScreenSprite, _fullScreenSpriteId);
	deleteSurface(&_fullScreenSprite);
}

void GnapEngine::showFullScreenSprite(int resourceId) {
	hideCursor();
	setGrabCursorSprite(-1);
	addFullScreenSprite(resourceId, 256);
	while (!_mouseClickState._left && !isKeyStatus1(Common::KEYCODE_ESCAPE) &&
		!isKeyStatus1(Common::KEYCODE_SPACE) && !isKeyStatus1(29)) {
		gameUpdateTick();
	}
	_mouseClickState._left = false;
	clearKeyStatus1(Common::KEYCODE_ESCAPE);
	clearKeyStatus1(29);
	clearKeyStatus1(Common::KEYCODE_SPACE);
	removeFullScreenSprite();
	showCursor();
}

void GnapEngine::queueInsertDeviceIcon() {
	_gameSys->insertSequence(0x10849, 20, 0, 0, kSeqNone, 0, _deviceX1, _deviceY1);
}

void GnapEngine::insertDeviceIconActive() {
	_gameSys->insertSequence(0x1084A, 21, 0, 0, kSeqNone, 0, _deviceX1, _deviceY1);
}

void GnapEngine::removeDeviceIconActive() {
	_gameSys->removeSequence(0x1084A, 21, true);
}

void GnapEngine::setDeviceHotspot(int hotspotIndex, int x1, int y1, int x2, int y2) {
	_deviceX1 = x1;
	_deviceX2 = x2;
	_deviceY1 = y1;
	_deviceY2 = y2;
	if (x1 == -1)
		_deviceX1 = 730;
	if (x2 == -1)
		_deviceX2 = 780;
	if (y1 == -1)
		_deviceY1 = 14;
	if (y2 == -1)
		_deviceY2 = 79;
	_hotspots[hotspotIndex]._x1 = _deviceX1;
	_hotspots[hotspotIndex]._y1 = _deviceY1;
	_hotspots[hotspotIndex]._x2 = _deviceX2;
	_hotspots[hotspotIndex]._y2 = _deviceY2;
	_hotspots[hotspotIndex]._flags = SF_TALK_CURSOR | SF_GRAB_CURSOR | SF_LOOK_CURSOR;
	_hotspots[hotspotIndex]._id = hotspotIndex;
}

int GnapEngine::getSequenceTotalDuration(int resourceId) {
	SequenceResource *sequenceResource = _sequenceCache->get(resourceId);
	int maxValue = 0;
	for (int i = 0; i < sequenceResource->_animationsCount; ++i) {
		SequenceAnimation *animation = &sequenceResource->_animations[i];
		if (animation->_additionalDelay + animation->_maxTotalDuration > maxValue)
			maxValue = animation->_additionalDelay + animation->_maxTotalDuration;
	}
	int totalDuration = maxValue + sequenceResource->_totalDuration;
	_sequenceCache->release(resourceId);
	return totalDuration;
}

bool GnapEngine::isSoundPlaying(int resourceId) {
	return _soundMan->isSoundPlaying(resourceId);
}

void GnapEngine::playSound(int resourceId, bool looping) {
	debugC(kDebugBasic, "playSound(%08X, %d)", resourceId, looping);
	_soundMan->playSound(resourceId, looping);
}

void GnapEngine::stopSound(int resourceId) {
	_soundMan->stopSound(resourceId);
}

void GnapEngine::setSoundVolume(int resourceId, int volume) {
	_soundMan->setSoundVolume(resourceId, volume);
}

void GnapEngine::updateTimers() {
	for (int i = 0; i < kMaxTimers; ++i)
		if (_timers[i] > 0)
			--_timers[i];
}

void GnapEngine::initGameFlags(int num) {
	invClear();
	invAdd(kItemMagazine);
	switch (num) {
	case 1:
		setFlag(kGFPlatypusTalkingToAssistant);
		break;
	case 2:
		clearFlags();
		break;
	case 3:
		invAdd(kItemDiceQuarterHole);
		clearFlags();
		break;
	case 4:
		invAdd(kItemDiceQuarterHole);
		invAdd(kItemHorn);
		invAdd(kItemLightbulb);
		clearFlags();
		setFlag(kGFPlatypus);
		setFlag(kGFMudTaken);
		setFlag(kGFNeedleTaken);
		setFlag(kGFTwigTaken);
		setFlag(kGFUnk04);
		setFlag(kGFKeysTaken);
		setFlag(kGFGrassTaken);
		setFlag(kGFBarnPadlockOpen);
		break;
	}
}

void GnapEngine::loadStockDat() {
	if (!_isStockDatLoaded) {
		_isStockDatLoaded = true;
		_dat->open(1, "stock_n.dat");
		//createMenuSprite();
		// NOTE Skipped preloading of data
	}
}

void GnapEngine::mainLoop() {
	_newCursorValue = 1;
	_cursorValue = -1;
	_newSceneNum = 0;
	_currentSceneNum = 55;
	_prevSceneNum = 55;
	invClear();
	clearFlags();
	_grabCursorSpriteIndex = -1;
	_grabCursorSprite = nullptr;

	loadStockDat();

	if (_loadGameSlot != -1) {
		// Load a savegame
		int slot = _loadGameSlot;
		_loadGameSlot = -1;
		loadGameState(slot);

		showCursor();
	}

	while (!_gameDone) {
		debugC(kDebugBasic, "New scene: %d", _newSceneNum);

		_prevSceneNum = _currentSceneNum;
		_currentSceneNum = _newSceneNum;

		debugC(kDebugBasic, "GnapEngine::mainLoop() _prevSceneNum: %d; _currentSceneNum: %d", _prevSceneNum, _currentSceneNum);

		if (_newCursorValue != _cursorValue) {
			debugC(kDebugBasic, "_newCursorValue: %d", _newCursorValue);
			_cursorValue = _newCursorValue;
			if (!_wasSavegameLoaded)
				initGameFlags(_cursorValue);
		}

		_sceneSavegameLoaded = _wasSavegameLoaded;
		_wasSavegameLoaded = false;

		initScene();

		runSceneLogic();
		afterScene();

		_soundMan->stopAll();

		// Force purge all resources
		_sequenceCache->purge(true);
		_soundCache->purge(true);
		_spriteCache->purge(true);

		if (isKeyStatus1(28)) {
			clearKeyStatus1(28);
			if (_debugLevel == 4)
				_gameDone = true;
		}
	}

	if (_backgroundSurface)
		deleteSurface(&_backgroundSurface);

	_dat->close(1);
	// TODO freeMenuSprite();
	// TODO freeFont();
}

void GnapEngine::initScene() {
	Common::String datFilename;

	_isLeavingScene = false;
	_sceneDone = false;
	_newSceneNum = 55;
	_gnap->_actionStatus = -1;
	_plat->_actionStatus = -1;
	_gnap->initBrainPulseRndValue();
	hideCursor();
	clearAllKeyStatus1();
	_mouseClickState._left = false;
	_mouseClickState._right = false;
	_sceneClickedHotspot = -1;

	datFilename = Common::String::format("%s_n.dat", kSceneNames[_currentSceneNum]);

	debugC(kDebugBasic, "GnapEngine::initScene() datFilename: %s", datFilename.c_str());

	_dat->open(0, datFilename.c_str());

	int backgroundId = initSceneLogic();

	if (!_backgroundSurface) {
		if (_currentSceneNum != 0)
			_backgroundSurface = _gameSys->loadBitmap(makeRid(1, 0x8AA));
		else
			_backgroundSurface = _gameSys->loadBitmap(makeRid(0, backgroundId));
		_gameSys->setBackgroundSurface(_backgroundSurface, 0, 500, 1, 1000);
	}

	if (_currentSceneNum != 0 && _currentSceneNum != 16 && _currentSceneNum != 47 &&
		_currentSceneNum != 48 && _currentSceneNum != 54) {
		_gameSys->drawBitmap(backgroundId);
	}

	if ((_cursorValue == 4 && isFlag(kGFGnapControlsToyUFO)) || _currentSceneNum == 41)
		playSound(makeRid(1, 0x8F6), true);

}

void GnapEngine::endSceneInit() {
	showCursor();
	if (_newGrabCursorSpriteIndex >= 0)
		setGrabCursorSprite(_newGrabCursorSpriteIndex);
}

void GnapEngine::afterScene() {
	if (_gameDone)
		return;

	if (_newCursorValue == _cursorValue && _newSceneNum != 0 && _newSceneNum != 16 &&
		_newSceneNum != 47 && _newSceneNum != 48 && _newSceneNum != 54 && _newSceneNum != 49 &&
		_newSceneNum != 50 && _newSceneNum != 51 && _newSceneNum != 52)
		_newGrabCursorSpriteIndex = _grabCursorSpriteIndex;
	else
		_newGrabCursorSpriteIndex = -1;

	setGrabCursorSprite(-1);

	_gameSys->requestClear2(false);
	_gameSys->requestClear1();
	_gameSys->waitForUpdate();

	_gameSys->requestClear2(false);
	_gameSys->requestClear1();
	_gameSys->waitForUpdate();

	screenEffect(0, 0, 0, 0);

	_dat->close(0);

	for (int animationIndex = 0; animationIndex < 12; ++animationIndex)
		_gameSys->setAnimation(0, 0, animationIndex);

	clearKeyStatus1(Common::KEYCODE_p);

	_mouseClickState._left = false;
	_mouseClickState._right = false;

}

void GnapEngine::checkGameKeys() {
	if (isKeyStatus1(Common::KEYCODE_p)) {
		clearKeyStatus1(Common::KEYCODE_p);
		pauseGame();
		updatePause();
	}
	// TODO? Debug input
}

void GnapEngine::startSoundTimerA(int timerIndex) {
	_soundTimerIndexA = timerIndex;
	_timers[timerIndex] = getRandom(50) + 100;
}

int GnapEngine::playSoundA() {
	static const int kSoundIdsA[] = {
		0x93E, 0x93F, 0x941, 0x942, 0x943, 0x944,
		0x945, 0x946, 0x947, 0x948, 0x949
	};

	int soundId = -1;

	if (!_timers[_soundTimerIndexA]) {
		_timers[_soundTimerIndexA] = getRandom(50) + 100;
		soundId = kSoundIdsA[getRandom(11)];
		playSound(soundId | 0x10000, false);
	}
	return soundId;
}

void GnapEngine::startSoundTimerB(int timerIndex) {
	_soundTimerIndexB = timerIndex;
	_timers[timerIndex] = getRandom(50) + 150;
}

int GnapEngine::playSoundB() {
	static const int kSoundIdsB[] = {
		0x93D, 0x929, 0x92A, 0x92B, 0x92C, 0x92D,
		0x92E, 0x92F, 0x930, 0x931, 0x932, 0x933,
		0x934, 0x935, 0x936, 0x937, 0x938, 0x939,
		0x93A
	};

	int soundId = -1;

	if (!_timers[_soundTimerIndexB]) {
		_timers[_soundTimerIndexB] = getRandom(50) + 150;
		soundId = kSoundIdsB[getRandom(19)];
		playSound(soundId | 0x10000, false);
	}
	return soundId;
}

void GnapEngine::startSoundTimerC(int timerIndex) {
	_soundTimerIndexC = timerIndex;
	_timers[timerIndex] = getRandom(50) + 150;
}

int GnapEngine::playSoundC() {
	static const int kSoundIdsC[] = {
		0x918, 0x91F, 0x920, 0x922, 0x923, 0x924,
		0x926
	};

	int soundId = -1;

	if (!_timers[_soundTimerIndexC]) {
		_timers[_soundTimerIndexC] = getRandom(50) + 150;
		soundId = kSoundIdsC[getRandom(7)] ;
		playSound(soundId | 0x10000, false);
	}
	return soundId;
}

void GnapEngine::startIdleTimer(int timerIndex) {
	_idleTimerIndex = timerIndex;
	_timers[timerIndex] = 3000;
}

void GnapEngine::updateIdleTimer() {
	if (!_timers[_idleTimerIndex]) {
		_timers[_idleTimerIndex] = 3000;
		_gameSys->insertSequence(0x1088B, 255, 0, 0, kSeqNone, 0, 0, 75);
	}
}

void GnapEngine::screenEffect(int dir, byte r, byte g, byte b) {
	if (dir == 1) {
		for (int y = 300; y < 600; y += 50) {
			_gameSys->fillSurface(0, 0, y, 800, 50, r, g, b);
			_gameSys->fillSurface(0, 0, 549 - y + 1, 800, 50, r, g, b);
			gameUpdateTick();
			_system->delayMillis(50);
		}
	} else {
		for (int y = 0; y < 300; y += 50) {
			_gameSys->fillSurface(0, 0, y, 800, 50, r, g, b);
			_gameSys->fillSurface(0, 0, 549 - y + 1, 800, 50, r, g, b);
			gameUpdateTick();
			_system->delayMillis(50);
		}
	}
}

bool GnapEngine::isKeyStatus1(int key) {
	return _keyPressState[key] != 0;;
}

bool GnapEngine::isKeyStatus2(int key) {
	return _keyDownState[key] != 0;;
}

void GnapEngine::clearKeyStatus1(int key) {
	_keyPressState[key] = 0;
	_keyDownState[key] = 0;
}

void GnapEngine::clearAllKeyStatus1() {
	_keyStatus1[0] = 0;
	_keyStatus1[1] = 0;
	memset(_keyPressState, 0, sizeof(_keyPressState));
	memset(_keyDownState, 0, sizeof(_keyDownState));
}

void GnapEngine::deleteSurface(Graphics::Surface **surface) {
	if (surface && *surface) {
		(*surface)->free();
		delete *surface;
		*surface = nullptr;
	}
}

bool GnapEngine::testWalk(int animationIndex, int someStatus, int gridX1, int gridY1, int gridX2, int gridY2) {
	if (_mouseClickState._left && someStatus == _gnap->_actionStatus) {
		_isLeavingScene = false;
		_gameSys->setAnimation(0, 0, animationIndex);
		_gnap->_actionStatus = -1;
		_plat->_actionStatus = -1;
		_gnap->walkTo(Common::Point(gridX1, gridY1), -1, -1, 1);
		_plat->walkTo(Common::Point(gridX2, gridY2), -1, -1, 1);
		_mouseClickState._left = false;
		return true;
	}
	return false;
}

void GnapEngine::doCallback(int callback) {
	switch (callback) {
	case 8:
	case 10:
	case 20:
		_scene->updateAnimationsCb();
		break;
	}
}

bool GnapEngine::gnapPlatypusAction(int gridX, int gridY, int platSequenceId, int callback) {
	bool result = false;

	if (_gnap->_actionStatus <= -1 && _plat->_actionStatus <= -1) {
		_gnap->_actionStatus = 100;
		Common::Point checkPt = _plat->_pos + Common::Point(gridX, gridY);
		if (isPointBlocked(checkPt) && (_gnap->_pos != checkPt)) {
			_plat->walkStep();
			checkPt = _plat->_pos + Common::Point(gridX, gridY);
		}

		if (!isPointBlocked(checkPt) && (_gnap->_pos != checkPt)) {
			_gnap->walkTo(checkPt, 0, 0x107B9, 1);
			while (_gameSys->getAnimationStatus(0) != 2) {
				updateMouseCursor();
				doCallback(callback);
				gameUpdateTick();
			}
			_gameSys->setAnimation(0, 0, 0);
			if (_gnap->_pos == _plat->_pos + Common::Point(gridX, gridY)) {
				_gameSys->setAnimation(platSequenceId, _plat->_id, 1);
				_plat->playSequence(platSequenceId);
				while (_gameSys->getAnimationStatus(1) != 2) {
					updateMouseCursor();
					doCallback(callback);
					gameUpdateTick();
				}
				result = true;
			}
		}
		_gnap->_actionStatus = -1;
	}
	return result;
}

void GnapEngine::gnapUseDisguiseOnPlatypus() {
	_gameSys->setAnimation(0x10846, _gnap->_id, 0);
	_gnap->playSequence(0x10846);
	while (_gameSys->getAnimationStatus(0) != 2)
		gameUpdateTick();
	_newSceneNum = 47;
	_isLeavingScene = true;
	_sceneDone = true;
	setFlag(kGFPlatypusDisguised);
}

////////////////////////////////////////////////////////////////////////////////

void GnapEngine::initGlobalSceneVars() {
	// Shared by scenes 17 && 18
	_s18GarbageCanPos = 8;

	// Toy UFO
	_toyUfoId = 0;
	_toyUfoActionStatus = -1;
	_toyUfoX = 0;
	_toyUfoY = 50;
}

void GnapEngine::playSequences(int fullScreenSpriteId, int sequenceId1, int sequenceId2, int sequenceId3) {
	setGrabCursorSprite(-1);
	_gameSys->setAnimation(sequenceId2, _gnap->_id, 0);
	_gameSys->insertSequence(sequenceId2, _gnap->_id,
		makeRid(_gnap->_sequenceDatNum, _gnap->_sequenceId), _gnap->_id,
		kSeqSyncWait, 0, 15 * (5 * _gnap->_pos.x - 25), 48 * (_gnap->_pos.y - 8));
	_gnap->_sequenceId = sequenceId2;
	_gnap->_sequenceDatNum = 0;
	while (_gameSys->getAnimationStatus(0) != 2)
		gameUpdateTick();
	hideCursor();
	addFullScreenSprite(fullScreenSpriteId, 255);
	_gameSys->setAnimation(sequenceId1, 256, 0);
	_gameSys->insertSequence(sequenceId1, 256, 0, 0, kSeqNone, 0, 0, 0);
	while (_gameSys->getAnimationStatus(0) != 2)
		gameUpdateTick();
	_gameSys->setAnimation(sequenceId3, _gnap->_id, 0);
	_gameSys->insertSequence(sequenceId3, _gnap->_id,
		makeRid(_gnap->_sequenceDatNum, _gnap->_sequenceId), _gnap->_id,
		kSeqSyncWait, 0, 15 * (5 * _gnap->_pos.x - 25), 48 * (_gnap->_pos.y - 8));
	removeFullScreenSprite();
	showCursor();
	_gnap->_sequenceId = sequenceId3;
}

void GnapEngine::toyUfoSetStatus(int flagNum) {
	clearFlag(kGFUnk16);
	clearFlag(kGFJointTaken);
	clearFlag(kGFUnk18);
	clearFlag(kGFGroceryStoreHatTaken);
	setFlag(flagNum);
}

int GnapEngine::toyUfoGetSequenceId() {
	if (isFlag(kGFUnk16))
		return 0x84E;
	if (isFlag(kGFJointTaken))
		return 0x84B;
	if (isFlag(kGFUnk18))
		return 0x84D;
	if (isFlag(kGFGroceryStoreHatTaken))
		return 0x84C;
	return 0x84E;
}

bool GnapEngine::toyUfoCheckTimer() {
	if (!isFlag(kGFGnapControlsToyUFO) || isFlag(kGFUnk18) || _timers[9] ||
		_toyUfoSequenceId == 0x870 || _toyUfoSequenceId == 0x871 || _toyUfoSequenceId == 0x872 || _toyUfoSequenceId == 0x873)
		return false;
	_sceneDone = true;
	_newSceneNum = 41;
	return true;
}

void GnapEngine::toyUfoFlyTo(int destX, int destY, int minX, int maxX, int minY, int maxY, int animationIndex) {
	GridStruct flyNodes[34];

	if (destX == -1)
		destX = _leftClickMouseX;

	if (destY == -1)
		destY = _leftClickMouseY;

	//CHECKME
	int clippedDestX = CLIP(destX, minX, maxX);
	int clippedDestY = CLIP(destY, minY, maxY);
	int dirX = 0, dirY = 0; // 0, -1 or 1

	if (clippedDestX != _toyUfoX)
		dirX = (clippedDestX - _toyUfoX) / ABS(clippedDestX - _toyUfoX);

	if (clippedDestY != _toyUfoY)
		dirY = (clippedDestY - _toyUfoY) / ABS(clippedDestY - _toyUfoY);

	int deltaX = ABS(clippedDestX - _toyUfoX);
	int deltaY = ABS(clippedDestY - _toyUfoY);

	int i = 0;
	if (deltaY > deltaX) {
		int flyDirYIncr = 32;
		int gridDistY = deltaY / flyDirYIncr;
		int curMove = 0;
		while (curMove < deltaY && i < 34) {
			if (gridDistY - 5 >= i) {
				flyDirYIncr = MIN(36, 8 * i + 8);
			} else {
				flyDirYIncr = MAX(6, flyDirYIncr - 3);
			}
			curMove += flyDirYIncr;
			flyNodes[i]._gridX1 = _toyUfoX + dirX * deltaX * curMove / deltaY;
			flyNodes[i]._gridY1 = _toyUfoY + dirY * curMove;
			++i;
		}
	} else {
		int flyDirXIncr = 36;
		int gridDistX = deltaX / flyDirXIncr;
		int curMove = 0;
		while (curMove < deltaX && i < 34) {
			if (gridDistX - 5 >= i) {
				flyDirXIncr = MIN(38, 8 * i + 8);
			} else {
				flyDirXIncr = MAX(6, flyDirXIncr - 3);
			}
			curMove += flyDirXIncr;
			flyNodes[i]._gridX1 = _toyUfoX + dirX * curMove;
			flyNodes[i]._gridY1 = _toyUfoY + dirY * deltaY * curMove / deltaX;
			++i;
		}
	}

	int nodesCount = i - 1;

	_toyUfoX = clippedDestX;
	_toyUfoY = clippedDestY;

	if (nodesCount > 0) {
		int seqId = 0;
		if (isFlag(kGFUnk16))
			seqId = 0x867;
		else if (isFlag(kGFJointTaken))
			seqId = 0x84F;
		else if (isFlag(kGFUnk18))
			seqId = 0x85F;
		else if (isFlag(kGFGroceryStoreHatTaken))
			seqId = 0x857;
		else
			error("Unhandled flag in GnapEngine::toyUfoFlyTo(): 0x%x", _gameFlags);
		flyNodes[0]._sequenceId = seqId;
		flyNodes[0]._id = 0;
		_gameSys->insertSequence(seqId | 0x10000, 0,
			_toyUfoSequenceId | 0x10000, _toyUfoId,
			kSeqSyncWait, 0, flyNodes[0]._gridX1 - 365, flyNodes[0]._gridY1 - 128);
		for (i = 1; i < nodesCount; ++i) {
			flyNodes[i]._sequenceId = seqId + (i % 8);
			flyNodes[i]._id = i;
			_gameSys->insertSequence(flyNodes[i]._sequenceId | 0x10000, flyNodes[i]._id,
				flyNodes[i - 1]._sequenceId | 0x10000, flyNodes[i - 1]._id,
				kSeqSyncWait, 0,
				flyNodes[i]._gridX1 - 365, flyNodes[i]._gridY1 - 128);
		}

		_toyUfoSequenceId = flyNodes[nodesCount - 1]._sequenceId;
		_toyUfoId = flyNodes[nodesCount - 1]._id;

		if (animationIndex >= 0)
			_gameSys->setAnimation(_toyUfoSequenceId | 0x10000, _toyUfoId, animationIndex);

	}
}

} // End of namespace Gnap
