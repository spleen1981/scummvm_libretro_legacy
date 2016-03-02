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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/scummsys.h"
#include "common/config-manager.h"
#include "common/debug.h"
#include "common/error.h"
#include "common/file.h"
#include "common/system.h"
#include "common/events.h"
#include "common/stream.h"
#include "common/savefile.h"

#include "engines/util.h"

#include "graphics/palette.h"

#include "adl/adl.h"
#include "adl/display.h"

namespace Adl {

AdlEngine::AdlEngine(OSystem *syst, const AdlGameDescription *gd) :
		Engine(syst),
		_gameDescription(gd),
		_display(nullptr),
		_isRestarting(false),
		_isRestoring(false),
		_saveVerb(0),
		_saveNoun(0),
		_restoreVerb(0),
		_restoreNoun(0),
		_canSaveNow(false),
		_canRestoreNow(false) {
}

AdlEngine::~AdlEngine() {
	delete _display;
}

bool AdlEngine::hasFeature(EngineFeature f) const {
	switch (f) {
	case kSupportsLoadingDuringRuntime:
	case kSupportsSavingDuringRuntime:
	case kSupportsRTL:
		return true;
	default:
		return false;
	}
}

Common::Error AdlEngine::run() {
	initGraphics(560, 384, true);

	byte palette[6 * 3] = {
		0x00, 0x00, 0x00,
		0xff, 0xff, 0xff,
		0xc7, 0x34, 0xff,
		0x38, 0xcb, 0x00,
		0x00, 0x00, 0xff, // FIXME
		0xff, 0xa5, 0x00  // FIXME
	};

	g_system->getPaletteManager()->setPalette(palette, 0, 6);

	_display = new Display();

	int saveSlot = ConfMan.getInt("save_slot");
	if (saveSlot >= 0) {
		if (!loadState(saveSlot))
			error("Failed to load save game from slot %i", saveSlot);
		_display->setCursorPos(Common::Point(0, 23));
		_isRestoring = true;
	} else {
		runIntro();
		initState();
	}

	runGame();

	return Common::kNoError;
}

Common::String AdlEngine::readString(Common::ReadStream &stream, byte until) {
	Common::String str;

	while (1) {
		byte b = stream.readByte();

		if (stream.eos() || stream.err() || b == until)
			break;

		str += b;
	};

	return str;
}

void AdlEngine::printStrings(Common::SeekableReadStream &stream, int count) {
	while (1) {
		Common::String str = readString(stream);
		_display->printString(str);

		if (--count == 0)
			break;

		stream.seek(3, SEEK_CUR);
	};
}

Common::String AdlEngine::getEngineString(int str) {
	return _strings[str];
}

void AdlEngine::wordWrap(Common::String &str) {
	uint end = 39;

	while (1) {
		if (str.size() <= end)
			return;

		while (str[end] != APPLECHAR(' '))
			--end;

		str.setChar(APPLECHAR('\r'), end);
		end += 40;
	}
}

void AdlEngine::printMessage(uint idx, bool wait) {
	Common::String msg = _messages[idx - 1];
	wordWrap(msg);
	_display->printString(msg);

	if (wait)
		delay(14 * 166018 / 1000);
}

void AdlEngine::printEngineMessage(EngineMessage msg) {
	printMessage(getEngineMessage(msg));
}

void AdlEngine::readCommands(Common::ReadStream &stream, Commands &commands) {
	while (1) {
		Command command;
		command.room = stream.readByte();

		if (command.room == 0xff)
			return;

		command.verb = stream.readByte();
		command.noun = stream.readByte();

		byte scriptSize = stream.readByte() - 6;

		command.numCond = stream.readByte();
		command.numAct = stream.readByte();

		for (uint i = 0; i < scriptSize; ++i)
			command.script.push_back(stream.readByte());

		if (stream.eos() || stream.err())
			error("Failed to read commands");

		if (command.numCond == 0 && command.script[0] == IDO_ACT_SAVE) {
			_saveVerb = command.verb;
			_saveNoun = command.noun;
		}

		if (command.numCond == 0 && command.script[0] == IDO_ACT_LOAD) {
			_restoreVerb = command.verb;
			_restoreNoun = command.noun;
		}

		commands.push_back(command);
	}
}

void AdlEngine::takeItem(byte noun) {
	Common::Array<Item>::iterator item;

	for (item = _state.items.begin(); item != _state.items.end(); ++item) {
		if (item->noun != noun || item->room != _state.room)
			continue;

		if (item->state == IDI_ITEM_DOESNT_MOVE) {
			printEngineMessage(IDI_MSG_ITEM_DOESNT_MOVE);
			return;
		}

		if (item->state == IDI_ITEM_MOVED) {
			item->room = IDI_NONE;
			return;
		}

		Common::Array<byte>::const_iterator pic;
		for (pic = item->roomPictures.begin(); pic != item->roomPictures.end(); ++pic) {
			if (*pic == curRoom().curPicture) {
				item->room = IDI_NONE;
				item->state = IDI_ITEM_MOVED;
				return;
			}
		}
	}

	printEngineMessage(IDI_MSG_ITEM_NOT_HERE);
}

void AdlEngine::dropItem(byte noun) {
	Common::Array<Item>::iterator item;

	for (item = _state.items.begin(); item != _state.items.end(); ++item) {
		if (item->noun != noun || item->room != IDI_NONE)
			continue;

		item->room = _state.room;
		item->state = IDI_ITEM_MOVED;
		return;
	}

	printEngineMessage(IDI_MSG_DONT_UNDERSTAND);
}

#define ARG(N) (command.script[offset + (N)])

void AdlEngine::doActions(const Command &command, byte noun, byte offset) {
	for (uint i = 0; i < command.numAct; ++i) {
		switch (ARG(0)) {
		case IDO_ACT_VAR_ADD:
			var(ARG(2)) += ARG(1);
			offset += 3;
			break;
		case IDO_ACT_VAR_SUB:
			var(ARG(2)) -= ARG(1);
			offset += 3;
			break;
		case IDO_ACT_VAR_SET:
			var(ARG(1)) = ARG(2);
			offset += 3;
			break;
		case IDO_ACT_LIST_ITEMS: {
			Common::Array<Item>::const_iterator item;

			for (item = _state.items.begin(); item != _state.items.end(); ++item)
				if (item->room == IDI_NONE)
					printMessage(item->description);

			++offset;
			break;
		}
		case IDO_ACT_MOVE_ITEM:
			item(ARG(1)).room = ARG(2);
			offset += 3;
			break;
		case IDO_ACT_SET_ROOM:
			curRoom().curPicture = curRoom().picture;
			_state.room = ARG(1);
			offset += 2;
			break;
		case IDO_ACT_SET_CUR_PIC:
			curRoom().curPicture = ARG(1);
			offset += 2;
			break;
		case IDO_ACT_SET_PIC:
			curRoom().picture = curRoom().curPicture = ARG(1);
			offset += 2;
			break;
		case IDO_ACT_PRINT_MSG:
			printMessage(ARG(1));
			offset += 2;
			break;
		case IDO_ACT_SET_LIGHT:
			_state.isDark = false;
			++offset;
			break;
		case IDO_ACT_SET_DARK:
			_state.isDark = true;
			++offset;
			break;
		case IDO_ACT_SAVE:
			saveState(0);
			++offset;
			break;
		case IDO_ACT_LOAD:
			loadState(0);
			++offset;
			// Original engine continues processing here (?)
			break;
		case IDO_ACT_RESTART: {
			_display->printString(_strings[IDI_STR_PLAY_AGAIN]);

			// We allow restoring via GMM here
			_canRestoreNow = true;
			Common::String input = inputString();
			_canRestoreNow = false;

			if (_isRestoring)
				return;

			if (input.size() == 0 || input[0] != APPLECHAR('N')) {
				_isRestarting = true;
				_display->clear(0x00);
				_display->decodeFrameBuffer();
				restartGame();
				return;
			}
			// Fall-through
		}
		case IDO_ACT_QUIT:
			printEngineMessage(IDI_MSG_THANKS_FOR_PLAYING);
			quitGame();
			return;
		case IDO_ACT_PLACE_ITEM:
			item(ARG(1)).room = ARG(2);
			item(ARG(1)).position.x = ARG(3);
			item(ARG(1)).position.y = ARG(4);
			offset += 5;
			break;
		case IDO_ACT_SET_ITEM_PIC:
			item(ARG(2)).picture = ARG(1);
			offset += 3;
			break;
		case IDO_ACT_RESET_PIC:
			curRoom().curPicture = curRoom().picture;
			++offset;
			break;
		case IDO_ACT_GO_NORTH:
		case IDO_ACT_GO_SOUTH:
		case IDO_ACT_GO_EAST:
		case IDO_ACT_GO_WEST:
		case IDO_ACT_GO_UP:
		case IDO_ACT_GO_DOWN: {
			byte room = curRoom().connections[ARG(0) - IDO_ACT_GO_NORTH];

			if (room == 0) {
				printEngineMessage(IDI_MSG_CANT_GO_THERE);
				return;
			}

			curRoom().curPicture = curRoom().picture;
			_state.room = room;
			return;
		}
		case IDO_ACT_TAKE_ITEM:
			takeItem(noun);
			++offset;
			break;
		case IDO_ACT_DROP_ITEM:
			dropItem(noun);
			++offset;
			break;
		case IDO_ACT_SET_ROOM_PIC:
			room(ARG(1)).picture = room(ARG(1)).curPicture = ARG(2);
			offset += 3;
			break;
		default:
			error("Invalid action opcode %02x", ARG(0));
		}
	}
}

bool AdlEngine::matchCommand(const Command &command, byte verb, byte noun, bool run) {
	if (command.room != IDI_NONE && command.room != _state.room)
		return false;

	if (command.verb != IDI_NONE && command.verb != verb)
		return false;

	if (command.noun != IDI_NONE && command.noun != noun)
		return false;

	uint offset = 0;
	for (uint i = 0; i < command.numCond; ++i) {
		switch (ARG(0)) {
		case IDO_CND_ITEM_IN_ROOM:
			if (item(ARG(1)).room != ARG(2))
				return false;
			offset += 3;
			break;
		case IDO_CND_MOVES_GE:
			if (ARG(1) > _state.moves)
				return false;
			offset += 2;
			break;
		case IDO_CND_VAR_EQ:
			if (var(ARG(1)) != ARG(2))
				return false;
			offset += 3;
			break;
		case IDO_CND_CUR_PIC_EQ:
			if (curRoom().curPicture != ARG(1))
				return false;
			offset += 2;
			break;
		case IDO_CND_ITEM_PIC_EQ:
			if (item(ARG(1)).picture != ARG(2))
				return false;
			offset += 3;
			break;
		default:
			error("Invalid condition opcode %02x", command.script[offset]);
		}
	}

	if (run)
		doActions(command, noun, offset);

	return true;
}

#undef ARG

bool AdlEngine::doOneCommand(const Commands &commands, byte verb, byte noun) {
	Commands::const_iterator cmd;

	for (cmd = commands.begin(); cmd != commands.end(); ++cmd)
		if (matchCommand(*cmd, verb, noun))
			return true;

	return false;
}

void AdlEngine::doAllCommands(const Commands &commands, byte verb, byte noun) {
	Commands::const_iterator cmd;
	bool oldIsRestoring = _isRestoring;

	for (cmd = commands.begin(); cmd != commands.end(); ++cmd) {
		matchCommand(*cmd, verb, noun);

		// We assume no restarts happen in this command group. This
		// simplifies enabling GMM savegame loading on the restart
		// prompt.
		if (_isRestarting || _isRestoring != oldIsRestoring)
			error("Unexpected restart action encountered");
	}
}

bool AdlEngine::canSaveGameStateCurrently() {
	if (!_canSaveNow)
		return false;

	Commands::const_iterator cmd;

	// Here we check whether or not the game currently accepts the command
	// "SAVE GAME". This prevents saving via the GMM in situations where
	// it wouldn't otherwise be possible to do so.
	for (cmd = _roomCommands.begin(); cmd != _roomCommands.end(); ++cmd) {
		if (matchCommand(*cmd, _saveVerb, _saveNoun, false)) {
			if (cmd->verb != _saveVerb || cmd->noun != _saveNoun)
				return false;
			return cmd->numCond == 0 && cmd->script[0] == IDO_ACT_SAVE;
		}
	}

	return false;
}

bool AdlEngine::canLoadGameStateCurrently() {
	return _canRestoreNow;
}

void AdlEngine::clearScreen() {
	_display->setMode(Display::kModeMixed);
	_display->clear(0x00);
}

void AdlEngine::drawItems() {
	Common::Array<Item>::const_iterator item;

	uint dropped = 0;

	for (item = _state.items.begin(); item != _state.items.end(); ++item) {
		if (item->room != _state.room)
			continue;

		if (item->state == IDI_ITEM_MOVED) {
			if (curRoom().picture == curRoom().curPicture) {
				const Common::Point &p =  _itemOffsets[dropped];
				if (item->isLineArt)
					_display->drawLineArt(_lineArt[item->picture - 1], p);
				else
					drawPic(item->picture, p);
				++dropped;
			}
			continue;
		}

		Common::Array<byte>::const_iterator pic;

		for (pic = item->roomPictures.begin(); pic != item->roomPictures.end(); ++pic) {
			if (*pic == curRoom().curPicture) {
				if (item->isLineArt)
					_display->drawLineArt(_lineArt[item->picture - 1], item->position);
				else
					drawPic(item->picture, item->position);
				continue;
			}
		}
	}
}

void AdlEngine::showRoom() {
	if (!_state.isDark) {
		drawPic(curRoom().curPicture);
		drawItems();
	}

	_display->decodeFrameBuffer();
	printMessage(curRoom().description, false);
}

bool AdlEngine::saveState(uint slot, const Common::String *description) {
	Common::String fileName = Common::String::format("%s.s%02d", _targetName.c_str(), slot);
	Common::OutSaveFile *outFile = getSaveFileManager()->openForSaving(fileName);

	if (!outFile) {
		warning("Failed to open file '%s'", fileName.c_str());
		return false;
	}

	outFile->writeUint32BE(MKTAG('A', 'D', 'L', ':'));
	outFile->writeByte(SAVEGAME_VERSION);

	char name[SAVEGAME_NAME_LEN] = { };

	if (description)
		strncpy(name, description->c_str(), sizeof(name) - 1);
	else {
		Common::String defaultName("Save ");
		defaultName += 'A' + slot;
		strncpy(name, defaultName.c_str(), sizeof(name) - 1);
	}

	outFile->write(name, sizeof(name));

	outFile->writeByte(_state.room);
	outFile->writeByte(_state.moves);
	outFile->writeByte(_state.isDark);

	outFile->writeUint32BE(_state.rooms.size());
	for (uint i = 0; i < _state.rooms.size(); ++i) {
		outFile->writeByte(_state.rooms[i].picture);
		outFile->writeByte(_state.rooms[i].curPicture);
	}

	outFile->writeUint32BE(_state.items.size());
	for (uint i = 0; i < _state.items.size(); ++i) {
		outFile->writeByte(_state.items[i].room);
		outFile->writeByte(_state.items[i].picture);
		outFile->writeByte(_state.items[i].position.x);
		outFile->writeByte(_state.items[i].position.y);
		outFile->writeByte(_state.items[i].state);
	}

	outFile->writeUint32BE(_state.vars.size());
	for (uint i = 0; i < _state.vars.size(); ++i)
		outFile->writeByte(_state.vars[i]);

	outFile->finalize();

	if (outFile->err()) {
		delete outFile;
		warning("Failed to save game '%s'", fileName.c_str());
		return false;
	}

	delete outFile;
	return true;
}

bool AdlEngine::loadState(uint slot) {
	Common::String fileName = Common::String::format("%s.s%02d", _targetName.c_str(), slot);
	Common::InSaveFile *inFile = getSaveFileManager()->openForLoading(fileName);

	if (!inFile) {
		warning("Failed to open file '%s'", fileName.c_str());
		return false;
	}

	if (inFile->readUint32BE() != MKTAG('A', 'D', 'L', ':')) {
		warning("No header found in '%s'", fileName.c_str());
		delete inFile;
		return false;
	}

	byte saveVersion = inFile->readByte();
	if (saveVersion != SAVEGAME_VERSION) {
		warning("Save game version %i not supported", saveVersion);
		delete inFile;
		return false;
	}

	initState();

	inFile->seek(SAVEGAME_NAME_LEN, SEEK_CUR);

	_state.room = inFile->readByte();
	_state.moves = inFile->readByte();
	_state.isDark = inFile->readByte();

	uint32 size = inFile->readUint32BE();
	if (size != _state.rooms.size())
		error("Room count mismatch (expected %i; found %i)", _state.rooms.size(), size);

	for (uint i = 0; i < size; ++i) {
		_state.rooms[i].picture = inFile->readByte();
		_state.rooms[i].curPicture = inFile->readByte();
	}

	size = inFile->readUint32BE();
	if (size != _state.items.size())
		error("Item count mismatch (expected %i; found %i)", _state.items.size(), size);

	for (uint i = 0; i < size; ++i) {
		_state.items[i].room = inFile->readByte();
		_state.items[i].picture = inFile->readByte();
		_state.items[i].position.x = inFile->readByte();
		_state.items[i].position.y = inFile->readByte();
		_state.items[i].state = inFile->readByte();
	}

	size = inFile->readUint32BE();
	if (size != _state.vars.size())
		error("Variable count mismatch (expected %i; found %i)", _state.vars.size(), size);

	for (uint i = 0; i < size; ++i)
		_state.vars[i] = inFile->readByte();

	if (inFile->err() || inFile->eos())
		error("Failed to load game '%s'", fileName.c_str());

	delete inFile;
	return true;
}

Room &AdlEngine::room(uint i) {
	if (i < 1 || i > _state.rooms.size())
		error("Room %i out of range [1, %i]", i, _state.rooms.size());

	return _state.rooms[i - 1];
}

Room &AdlEngine::curRoom() {
	return room(_state.room);
}

Item &AdlEngine::item(uint i) {
	if (i < 1 || i > _state.items.size())
		error("Item %i out of range [1, %i]", i, _state.items.size());

	return _state.items[i - 1];
}

byte &AdlEngine::var(uint i) {
	if (i >= _state.vars.size())
		error("Variable %i out of range [0, %i]", i, _state.vars.size() - 1);

	return _state.vars[i];
}

void AdlEngine::loadWords(Common::ReadStream &stream, WordMap &map) {
	uint index = 0;

	while (1) {
		++index;

		byte buf[kWordSize];

		if (stream.read(buf, kWordSize) < kWordSize)
			error("Error reading word list");

		Common::String word((char *)buf, kWordSize);

		if (!map.contains(word))
			map[word] = index;

		byte synonyms = stream.readByte();

		if (stream.err() || stream.eos())
			error("Error reading word list");

		if (synonyms == 0xff)
			break;

		for (uint i = 0; i < synonyms; ++i) {
			if (stream.read((char *)buf, kWordSize) < kWordSize)
				error("Error reading word list");

			word = Common::String((char *)buf, kWordSize);

			if (!map.contains(word))
				map[word] = index;
		}
	}
}

Common::String AdlEngine::getLine() {
	// Original engine uses a global here, which isn't reset between
	// calls and may not match actual mode
	bool textMode = false;

	while (1) {
		Common::String line = inputString(APPLECHAR('?'));

		if (shouldQuit() || _isRestoring)
			return "";

		if ((byte)line[0] == ('\r' | 0x80)) {
			textMode = !textMode;
			_display->setMode(textMode ? Display::kModeText : Display::kModeMixed);
			continue;
		}

		// Remove the return
		line.deleteLastChar();
		return line;
	}
}

Common::String AdlEngine::getWord(const Common::String &line, uint &index) {
	Common::String str;

	for (uint i = 0; i < 8; ++i)
		str += APPLECHAR(' ');

	int copied = 0;

	// Skip initial whitespace
	while (1) {
		if (index == line.size())
			return str;
		if (line[index] != APPLECHAR(' '))
			break;
		++index;
	}

	// Copy up to 8 characters
	while (1) {
		if (copied < 8)
			str.setChar(line[index], copied++);

		index++;

		if (index == line.size() || line[index] == APPLECHAR(' '))
			return str;
	}
}

void AdlEngine::getInput(uint &verb, uint &noun) {
	while (1) {
		_display->printString(getEngineString(IDI_STR_ENTER_COMMAND));
		Common::String line = getLine();

		if (shouldQuit() || _isRestoring)
			return;

		uint index = 0;
		Common::String verbStr = getWord(line, index);

		if (!_verbs.contains(verbStr)) {
			Common::String err = getEngineString(IDI_STR_VERB_ERROR);
			for (uint i = 0; i < verbStr.size(); ++i)
				err.setChar(verbStr[i], i + 19);
			_display->printString(err);
			continue;
		}

		verb = _verbs[verbStr];

		Common::String nounStr = getWord(line, index);

		if (!_nouns.contains(nounStr)) {
			Common::String err = getEngineString(IDI_STR_NOUN_ERROR);
			for (uint i = 0; i < verbStr.size(); ++i)
				err.setChar(verbStr[i], i + 19);
			for (uint i = 0; i < nounStr.size(); ++i)
				err.setChar(nounStr[i], i + 30);
			_display->printString(err);
			continue;
		}

		noun = _nouns[nounStr];
		return;
	}
}

void AdlEngine::printASCIIString(const Common::String &str) {
	Common::String aStr;

	Common::String::const_iterator it;
	for (it = str.begin(); it != str.end(); ++it)
			aStr += APPLECHAR(*it);

	_display->printString(aStr);
}

Common::String AdlEngine::inputString(byte prompt) {
	Common::String s;

	if (prompt > 0)
		_display->printString(Common::String(prompt));

	while (1) {
		byte b = inputKey();

		if (g_engine->shouldQuit() || _isRestoring)
			return 0;

		if (b == 0)
			continue;

		if (b == ('\r' | 0x80)) {
			s += b;
			_display->printString(Common::String(b));
			return s;
		}

		if (b < 0xa0) {
			switch (b) {
			case Common::KEYCODE_BACKSPACE | 0x80:
				if (!s.empty()) {
					_display->moveCursorBackward();
					_display->setCharAtCursor(APPLECHAR(' '));
					s.deleteLastChar();
				}
				break;
			};
		} else {
			s += b;
			_display->printString(Common::String(b));
		}
	}
}

byte AdlEngine::convertKey(uint16 ascii) {
	ascii = toupper(ascii);

	if (ascii >= 0x80)
		return 0;

	ascii |= 0x80;

	if (ascii >= 0x80 && ascii <= 0xe0)
		return ascii;

	return 0;
}

byte AdlEngine::inputKey() {
	Common::EventManager *ev = g_system->getEventManager();

	byte key = 0;

	_display->showCursor(true);

	while (!g_engine->shouldQuit() && !_isRestoring && key == 0) {
		Common::Event event;
		if (ev->pollEvent(event)) {
			if (event.type != Common::EVENT_KEYDOWN)
				continue;

			if (event.kbd.flags & Common::KBD_CTRL) {
				if (event.kbd.keycode == Common::KEYCODE_q)
					g_engine->quitGame();
				continue;
			}

			switch (event.kbd.keycode) {
			case Common::KEYCODE_BACKSPACE:
			case Common::KEYCODE_RETURN:
				key = convertKey(event.kbd.keycode);
				break;
			default:
				if (event.kbd.ascii >= 0x20 && event.kbd.ascii < 0x80)
					key = convertKey(event.kbd.ascii);
			};
		}

		_display->updateTextSurface();
		_display->updateScreen();
		g_system->updateScreen();
		g_system->delayMillis(16);
	}

	_display->showCursor(false);

	return key;
}

void AdlEngine::delay(uint32 ms) {
	Common::EventManager *ev = g_system->getEventManager();

	uint32 start = g_system->getMillis();

	while (!g_engine->shouldQuit() && g_system->getMillis() - start < ms) {
		Common::Event event;
		if (ev->pollEvent(event)) {
			if (event.type == Common::EVENT_KEYDOWN && (event.kbd.flags & Common::KBD_CTRL)) {
				switch(event.kbd.keycode) {
				case Common::KEYCODE_q:
					g_engine->quitGame();
					break;
				default:
					break;
				}
			}
		}
		_display->updateScreen();
		g_system->updateScreen();
		g_system->delayMillis(16);
	}
}

Common::Error AdlEngine::loadGameState(int slot) {
	if (loadState(slot)) {
		_isRestoring = true;
		return Common::kNoError;
	}

	return Common::kUnknownError;
}

Common::Error AdlEngine::saveGameState(int slot, const Common::String &desc) {
	if (saveState(slot, &desc))
		return Common::kNoError;

	return Common::kUnknownError;
}

AdlEngine *AdlEngine::create(GameType type, OSystem *syst, const AdlGameDescription *gd) {
	switch(type) {
	case kGameTypeHires1:
		return HiRes1Engine__create(syst, gd);
	default:
		error("Unknown GameType");
	}
}

} // End of namespace Adl
