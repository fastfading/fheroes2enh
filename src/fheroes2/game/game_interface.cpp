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

#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <ctime>
#include <sstream>
#include "agg.h"
#include "settings.h"
#include "maps.h"
#include "mp2.h"
#include "world.h"
#include "game.h"
#include "dialog.h"
#include "game_interface.h"

Interface::Basic::Basic() : gameArea(*this), radar(*this),
                            iconsPanel(*this), buttonsArea(*this),
                            heroesBar(*this),
                            statusWindow(*this), controlPanel(*this),
                            redraw(0)
{
    Settings &conf = Settings::Get().Get();
    const Display &display = Display::Get();
    const int scroll_width = BORDERWIDTH;

    SetHideInterface(conf.ExtGameHideInterface());

    scrollLeft = Rect(0, 0, scroll_width, display.h());
    scrollRight = Rect(display.w() - scroll_width, 0, scroll_width, display.h());
    scrollTop = Rect(0, 0, display.w() - radar.GetArea().w, scroll_width);
    scrollBottom = Rect(0, display.h() - scroll_width, display.w(), scroll_width);

    system_info.Set(Font::YELLOW_SMALL);
}

Interface::GameArea &Interface::Basic::GetGameArea()
{
    return gameArea;
}

Interface::Radar &Interface::Basic::GetRadar()
{
    return radar;
}

Interface::IconsPanel &Interface::Basic::GetIconsPanel()
{
    return iconsPanel;
}

Interface::ButtonsArea &Interface::Basic::GetButtonsArea()
{
    return buttonsArea;
}

Interface::StatusWindow &Interface::Basic::GetStatusWindow()
{
    return statusWindow;
}

Interface::ControlPanel &Interface::Basic::GetControlPanel()
{
    return controlPanel;
}

void Interface::Basic::SetHideInterface(bool f)
{
    Settings &conf = Settings::Get().Get();
    const Display &display = Display::Get();
    const u32 px = display.w() - BORDERWIDTH - RADARWIDTH;
    const u32 scroll_width = BORDERWIDTH;

    conf.SetHideInterface(f);

    if (f)
    {
        conf.SetShowPanel(true);

        Point pos_radr = conf.PosRadar();
        Point pos_bttn = conf.PosButtons();
        Point pos_icon = conf.PosIcons();
        Point pos_stat = conf.PosStatus();

        if (0 == pos_radr.x && 0 == pos_radr.y)
            pos_radr = Point(BORDERWIDTH, BORDERWIDTH);
        if (0 == pos_icon.x && 0 == pos_icon.y)
            pos_icon = Point(px - BORDERWIDTH,
                             radar.GetArea().y + radar.GetArea().h);
        if (0 == pos_bttn.x && 0 == pos_bttn.y)
            pos_bttn = Point(px - BORDERWIDTH,
                             iconsPanel.GetArea().y + iconsPanel.GetArea().h);
        if (0 == pos_stat.x && 0 == pos_stat.y)
            pos_stat = Point(px - BORDERWIDTH,
                             buttonsArea.GetArea().y + buttonsArea.GetArea().h);

        controlPanel.SetPos(display.w() - controlPanel.GetArea().w - scroll_width, 0);
        radar.SetPos(pos_radr.x, pos_radr.y);
        iconsPanel.SetPos(pos_icon.x, pos_icon.y);
        buttonsArea.SetPos(pos_bttn.x, pos_bttn.y);
        statusWindow.SetPos(pos_stat.x, pos_stat.y);
    } else
    {
        radar.SetPos(px, BORDERWIDTH);
        iconsPanel.SetPos(px, radar.GetArea().y + radar.GetArea().h + BORDERWIDTH);

        buttonsArea.SetPos(px, iconsPanel.GetArea().y + iconsPanel.GetArea().h + BORDERWIDTH);
        statusWindow.SetPos(px, buttonsArea.GetArea().y + buttonsArea.GetArea().h);
    }

    gameArea.Build();
}

Interface::Basic &Interface::Basic::Get()
{
    static Basic basic;
    return basic;
}

const Rect &Interface::Basic::GetScrollLeft() const
{
    return scrollLeft;
}

const Rect &Interface::Basic::GetScrollRight() const
{
    return scrollRight;
}

const Rect &Interface::Basic::GetScrollTop() const
{
    return scrollTop;
}

const Rect &Interface::Basic::GetScrollBottom() const
{
    return scrollBottom;
}


bool Interface::Basic::NeedRedraw() const
{
    return redraw != 0;
}

void Interface::Basic::SetRedraw(int f)
{
    redraw |= f;
}

void Interface::Basic::Redraw(int force)
{
    Settings &conf = Settings::Get();

    if ((redraw | force) & REDRAW_GAMEAREA) gameArea.Redraw(Display::Get(), LEVEL_ALL);

    if ((conf.ExtGameHideInterface() && conf.ShowRadar()) || ((redraw | force) & REDRAW_RADAR)) radar.Redraw();

    heroesBar.Redraw();

    if ((conf.ExtGameHideInterface() && conf.ShowIcons()) || ((redraw | force) & REDRAW_ICONS)) iconsPanel.Redraw();
    else if ((redraw | force) & REDRAW_HEROES) iconsPanel.RedrawIcons(ICON_HEROES);
    else if ((redraw | force) & REDRAW_CASTLES) iconsPanel.RedrawIcons(ICON_CASTLES);

    if ((conf.ExtGameHideInterface() && conf.ShowButtons()) || ((redraw | force) & REDRAW_BUTTONS))
        buttonsArea.Redraw();

    if ((conf.ExtGameHideInterface() && conf.ShowStatus()) || ((redraw | force) & REDRAW_STATUS)) statusWindow.Redraw();

    if (conf.ExtGameHideInterface() && conf.ShowControlPanel() && (redraw & REDRAW_GAMEAREA)) controlPanel.Redraw();

    // show system info
    if (conf.ExtGameShowSystemInfo())
        RedrawSystemInfo((conf.ExtGameHideInterface() ? 10 : 26),
                         Display::Get().h() - (conf.ExtGameHideInterface() ? 14 : 30), System::GetMemoryUsage());

    if ((redraw | force) & REDRAW_BORDER)
        GameBorderRedraw();

    redraw = 0;
}

void Interface::Basic::RedrawSystemInfo(s32 cx, s32 cy, u32 usage)
{
    ostringstream os;

    os << "mem. usage: " << usage / 1024 << "Kb" << ", cur. time: ";

    time_t rawtime;
    time(&rawtime);
    // strtime format: Www Mmm dd hh:mm:ss yyyy
    const char *strtime = ctime(&rawtime);

    // draw info
    os << string(&strtime[11], 8);

    system_info.Set(os.str());
    system_info.Blit(cx, cy);
}

s32 Interface::Basic::GetDimensionDoorDestination(s32 from, u32 distance, bool water) const
{
    Cursor &cursor = Cursor::Get();
    Display &display = Display::Get();
    Settings &conf = Settings::Get();
    LocalEvent &le = LocalEvent::Get();
    s32 dst = -1;

    while (le.HandleEvents())
    {
        const Point &mp = le.GetMouseCursor();
        dst = gameArea.GetIndexFromMousePoint(mp);
        if (0 > dst) break;

        const Maps::Tiles &tile = world.GetTiles(dst);

        const bool valid = ((gameArea.GetArea() & mp) &&
                            dst >= 0 &&
                            (!tile.isFog(conf.CurrentColor())) &&
                            MP2::isClearGroundObject(tile.GetObject()) &&
                            water == world.GetTiles(dst).isWater() &&
                            distance >= Maps::GetApproximateDistance(from, dst));

        cursor.SetThemes(valid ? (water ? Cursor::BOAT : Cursor::MOVE) : Cursor::WAR_NONE);

        // exit
        if (le.MousePressRight()) break;
        if (le.MouseClickLeft() && valid) return dst;

        // redraw cursor
        if (!cursor.isVisible())
        {
            cursor.Show();
            display.Flip();
        }
    }

    return -1;
}
