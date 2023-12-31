/*
Copyright (C) 1998 Pyrosoft Inc. (www.pyrosoftgames.com), Matthew Bogue
 
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include <iostream>
#include <string>

#include <time.h>

#include "Util/Log.hpp"
#include "Util/Exception.hpp"
#include "Util/FileSystem.hpp"
#include "Util/NTimer.hpp"
#include "SDLVideo.hpp"
#include <stdlib.h>

#include "Interfaces/ConsoleInterface.hpp"
#include "2D/Color.hpp"

#if defined _WIN32 || defined __MINGW32__

#include "Interfaces/GameConfig.hpp"

#endif

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "testing"
#endif

SDLVideo *Screen; // get rid of this later...

SDLVideo::SDLVideo()
        : window(0) {
#if defined _WIN32
#if not defined __MINGW32__
    if (GameConfig::video_usedirectx) {
        putenv("SDL_VIDEODRIVER=directx");
    }
#endif
#endif
    this->window = nullptr;
    this->renderer = nullptr;
    this->surface = nullptr;
    this->texture = nullptr;
    this->is_fullscreen = false;
    if (SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        throw Exception("Couldn't initialize SDL_video subsystem: %s",
                        SDL_GetError());
    }
}

SDLVideo::~SDLVideo() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
    if (surface != nullptr) {
        SDL_FreeSurface(surface);
    }
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void SDLVideo::setVideoMode(int new_width, int new_height, int bpp, bool fullscreen) {
    const bool was_fullscreen = this->is_fullscreen;
    this->is_fullscreen = fullscreen;

    if (window == nullptr) {
        if (fullscreen) {
            // use the native desktop resolution, and scale linearly later using renderer
            window = SDL_CreateWindow("NetPanzer " PACKAGE_VERSION,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      0, 0,
                                      SDL_WINDOW_FULLSCREEN_DESKTOP);
        } else {
            window = SDL_CreateWindow("NetPanzer " PACKAGE_VERSION,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      new_width, new_height,
                                      SDL_WINDOW_RESIZABLE);
        }
        if (window == nullptr) {
            throw Exception("Couldn't create a window %s", SDL_GetError());
        }

        renderer = SDL_CreateRenderer(window, -1, 0);

        if (renderer == nullptr) {
            throw Exception("Couldn't create renderer %s", SDL_GetError());
        }
    } else {
        if (fullscreen) {
            if (was_fullscreen) {
                // no change
            } else {
                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
        } else {
            if (was_fullscreen) {
                SDL_SetWindowFullscreen(window, 0);
            }
            SDL_SetWindowSize(window, new_width, new_height);
        }
    }

    if (surface != nullptr) {
        SDL_FreeSurface(surface);
    }

    surface = SDL_CreateRGBSurfaceWithFormat(0, new_width, new_height, 8, SDL_PIXELFORMAT_INDEX8);

    if (surface == nullptr) {
        throw Exception("Couldn't create surface %s", SDL_GetError());
    }

    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture == nullptr) {
        throw Exception("Couldn't create texture %s", SDL_GetError());
    }

    // make the scaled rendering look smoother.
    // Note: "linear" made game look blurry when game resolution did not match monitor resolution.
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_RenderSetLogicalSize(renderer, new_width, new_height);

    SDL_ShowWindow(window); // has to happen before fullscreen switch to fix cursor stuck in region issue

    // let's scare the mouse :)
    // this fixes the mouse cursor stuck to a small region after resolution change
    int showCursorResult = SDL_ShowCursor(SDL_DISABLE);
    if (showCursorResult < 0) {
        printf("Could not show cursor! %s\n", SDL_GetError());
    }

    // Center the mouse after changing the resolution - also helps mouse cursor from getting stuck in old region
    int centerX = new_width / 2;
    int centerY = new_height / 2;
    SDL_WarpMouseInWindow(window, centerX, centerY);
}

void SDLVideo::setPalette(SDL_Color *color) {
    SDL_SetPaletteColors(surface->format->palette, color, 0, 256);
}

SDL_Surface *SDLVideo::getSurface() {
    return surface;
}

void SDLVideo::render() {
    // This mechanism is only about 5-10% slower than SDL_BlitSurface && SDL_UpdateWindowSurface.
    // But, it gets us a lot (simpler code, much nicer rendering and scaling).
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void SDLVideo::doScreenshot() {
    // this is called blind faith
    static NTimer timer(1000);

    if (!timer.isTimeOut()) {
        return;
    }

    filesystem::mkdir("screenshots");

    char buf[256];
    time_t curtime = time(0);
    struct tm *loctime = localtime(&curtime);
    strftime(buf, sizeof(buf), "screenshots/%Y%m%d_%H%M%S.bmp", loctime);

    std::string bmpfile = filesystem::getRealWriteName(buf);
    SDL_SaveBMP(surface, bmpfile.c_str());
    ConsoleInterface::postMessage(Color::cyan, false, 0, "Screenshot saved as: %s", buf);
    timer.reset();
}

