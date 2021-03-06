/***************************************************************************
 *   Copyright (C) 2010 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef H2BATTLE_CELL_H
#define H2BATTLE_CELL_H

#include "gamedefs.h"
#include "battle.h"
#include "ByteVectorReader.h"

#define CELLW 45
#define CELLH 52
#define CELLW2 23
#define CELLH2 26

namespace Battle
{
    class Board;

    class Unit;

    enum direction_t
    {
        UNKNOWN = 0x00,
        TOP_LEFT = 0x01,
        TOP_RIGHT = 0x02,
        RIGHT = 0x04,
        BOTTOM_RIGHT = 0x08,
        BOTTOM_LEFT = 0x10,
        LEFT = 0x20,
        CENTER = 0x40,
        RIGHT_SIDE = TOP_RIGHT | RIGHT | BOTTOM_RIGHT,
        LEFT_SIDE = TOP_LEFT | LEFT | BOTTOM_LEFT,
        AROUND = RIGHT_SIDE | LEFT_SIDE
    };

    class Cell
    {
    public:
        Cell();

        Cell(s32);

        void ResetQuality();

        void ResetDirection();

        void SetObject(int);

        void SetDirection(int);

        void SetQuality(u32);

        void SetArea(const Rect &);

        bool isPassable4(const Unit &, const Cell &) const;

        bool isPassable3(const Unit &, bool check_reflect) const;

        bool isPassable1(bool check_troop) const;

        bool isPositionIncludePoint(const Point &) const;

        s32 GetIndex() const;

        const Rect &GetPos() const;

        int GetObject() const;

        int GetDirection() const;

        s32 GetQuality() const;

        direction_t GetTriangleDirection(const Point &) const;

        const Unit *GetUnit() const;

        Unit *GetUnit();

        void SetUnit(Unit *);

    private:
        friend StreamBase &operator<<(StreamBase &, const Cell &);

        friend StreamBase &operator>>(StreamBase &, Cell &);

        friend ByteVectorReader &operator>>(ByteVectorReader &, Cell &);

        s32 index;
        Rect pos;
        int object;
        int direction;
        s32 quality;
        Unit *troop;
        Point coord[7];
    };

    StreamBase &operator<<(StreamBase &, const Cell &);

    StreamBase &operator>>(StreamBase &, Cell &);

    class Position : protected pair<Cell *, Cell *>
    {
    public:
        Position() : pair<Cell *, Cell *>(nullptr, nullptr)
        {}

        void Set(s32 head, bool wide, bool reflect);

        void Swap();

        bool isReflect() const;

        bool isValid() const;

        static Position GetCorrect(const Unit &, s32);

        Rect GetRect() const;

        Cell *GetHead();

        const Cell *GetHead() const;

        Cell *GetTail();

        const Cell *GetTail() const;
    };
}

#endif
