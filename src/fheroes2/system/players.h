/***************************************************************************
 *   Copyright (C) 2011 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#ifndef H2PLAYERS_H
#define H2PLAYERS_H

#include "ByteVectorReader.h"
#include <string>
#include <vector>
#include "bitmodes.h"
#include "color.h"
#include "gamedefs.h"
#include "rect.h"

namespace Maps
{
    class FileInfo;
}

class Castle;

class Heroes;

// control_t
enum
{
    CONTROL_NONE = 0,
    CONTROL_HUMAN = 1,
    CONTROL_AI = 4,
    CONTROL_REMOTE = 2 /*, CONTROL_LOCAL = CONTROL_AI | CONTROL_HUMAN */ };
enum
{
    FOCUS_UNSEL = 0, FOCUS_HEROES = 1, FOCUS_CASTLE = 2
};

struct Focus : pair<int, void *>
{
    Focus() : pair<int, void *>(FOCUS_UNSEL, nullptr)
    {}

    bool isValid() const
    { return first != FOCUS_UNSEL && second; }

    void Reset()
    {
        first = FOCUS_UNSEL;
        second = nullptr;
    }

    void Set(Castle *ptr)
    {
        first = FOCUS_CASTLE;
        second = ptr;
    }

    void Set(Heroes *ptr)
    {
        first = FOCUS_HEROES;
        second = ptr;
    }

    Castle *GetCastle()
    { return first == FOCUS_CASTLE && second ? reinterpret_cast<Castle *>(second) : nullptr; }

    Heroes *GetHeroes()
    { return first == FOCUS_HEROES && second ? reinterpret_cast<Heroes *>(second) : nullptr; }
};

struct Control
{
    virtual int GetControl() const = 0;

    virtual ~Control() = default;

    bool isControlAI() const;

    bool isControlHuman() const;

    bool isControlRemote() const;

    bool isControlLocal() const;
};

class Player : public Control
{
public:
    Player(int col = Color::NONE);

    bool isID(u32) const;

    bool isColor(int) const;

    bool isName(const string &) const;

    bool isPlay() const;

    void SetColor(int);

    void SetRace(int);

    void SetControl(int);

    void SetPlay(bool);

    void SetFriends(int);

    void SetName(const string &);

    int GetControl() const;

    int GetColor() const;

    int GetRace() const;

    int GetFriends() const;

    int GetID() const;

    const string &GetName() const;

    Focus &GetFocus();

    const Focus &GetFocus() const;

    BitModes _bitModes;

protected:
    friend StreamBase &operator<<(StreamBase &, const Player &);

    friend StreamBase &operator>>(StreamBase &, Player &);

    friend ByteVectorReader &operator>>(ByteVectorReader &, Player &);

    int control;
    int color;
    int race;
    int friends;
    string name;
    u32 id;
    Focus focus;
};

StreamBase &operator<<(StreamBase &, const Player &);

StreamBase &operator>>(StreamBase &, Player &);

ByteVectorReader &operator>>(ByteVectorReader &, Player &);

class Players
{
public:
    Players();

    ~Players();

    void Init(int colors);

    void Init(const Maps::FileInfo &);

    void clear();

    void SetStartGame();

    int GetColors(int control = 0xFF, bool strong = false) const;

    int GetActualColors() const;

    string String() const;

    Player *GetCurrent();

    const Player *GetCurrent() const;

    static Player *Get(int color);

    static int GetPlayerControl(int color);

    static int GetPlayerRace(int color);

    static int GetPlayerFriends(int color);

    static bool GetPlayerInGame(int color);

    static bool isFriends(int player, int colors);

    static void SetPlayerRace(int color, int race);

    static void SetPlayerControl(int color, int ctrl);

    static void SetPlayerInGame(int color, bool);

    static int HumanColors();

    static int FriendColors();

    vector<Player *> _items;
    int current_color;
};

StreamBase &operator<<(StreamBase &, const Players &);

StreamBase &operator>>(StreamBase &, Players &);

ByteVectorReader &operator>>(ByteVectorReader &, Players &);

namespace Interface
{
    struct PlayerInfo
    {
        PlayerInfo() : player(nullptr)
        {}

        bool operator==(const Player *) const;

        Player *player;
        Rect rect1; // opponent
        Rect rect2; // class
        Rect rect3; // change
    };

    struct PlayersInfo : vector<PlayerInfo>
    {
        PlayersInfo(bool /* show name */, bool /* show race */, bool /* show swap button */);

        void UpdateInfo(Players &, const Point &opponents, const Point &classes);

        Player *GetFromOpponentClick(const Point &pt);

        Player *GetFromOpponentNameClick(const Point &pt);

        Player *GetFromOpponentChangeClick(const Point &pt);

        Player *GetFromClassClick(const Point &pt);

        void RedrawInfo(bool show_play_info = false) const;

        bool QueueEventProcessing();

        bool show_name;
        bool show_race;
        bool show_swap;
    };
}

#endif
