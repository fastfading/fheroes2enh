/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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
#ifndef H2RAND_H
#define H2RAND_H

#include <vector>
#include <list>
#include <utility>
#include <iterator>
#include "types.h"

using namespace std;

namespace Rand
{
    void Init();

    u32 Get(u32 min, u32 max = 0);

    template<typename T>
    const T *Get(const vector<T> &vec)
    {
        typename vector<T>::const_iterator it = vec.begin();
        std::advance(it, Rand::Get(vec.size() - 1));
        return it == vec.end() ? nullptr : &(*it);
    }

    template<typename T>
    const T *Get(const list <T> &lst)
    {
        auto it = lst.begin();
        std::advance(it, Rand::Get(lst.size() - 1));
        return it == lst.end() ? nullptr : &(*it);
    }

    typedef pair<s32, u32> ValuePercent;

    class Queue : private vector<ValuePercent>
    {
    public:
        Queue(u32 size = 0);

        void Reset();

        void Push(s32, u32);

        size_t Size() const;

        s32 Get();
    };
}

#endif
