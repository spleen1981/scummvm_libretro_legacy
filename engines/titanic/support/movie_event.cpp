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

#include "titanic/support/movie_event.h"

namespace Titanic {

CMovieEvent::CMovieEvent() : ListItem(), _fieldC(0), _field10(0),
	_field14(0), _field1C(0) {
}

CMovieEvent::CMovieEvent(const CMovieEvent *src) {
	_fieldC = src->_fieldC;
	_field10 = src->_field10;
	_field14 = src->_field14;
	_field18 = src->_field18;
	_field1C = src->_field1C;
}

void CMovieEvent::save(SimpleFile *file, int indent) const {
	file->writeNumberLine(0, indent);
	file->writeNumberLine(_fieldC, indent + 1);
	file->writeNumberLine(_field10, indent + 1);
	file->writeNumberLine(_field14, indent + 1);
	file->writeNumberLine(_field1C, indent + 1);

	ListItem::save(file, indent);
}

void CMovieEvent::load(SimpleFile *file) {
	int val = file->readNumber();
	if (!val) {
		_fieldC = file->readNumber();
		_field10 = file->readNumber();
		_field14 = file->readNumber();
		_field1C = file->readNumber();
	}

	ListItem::load(file);
}

} // End of namespace Titanic
