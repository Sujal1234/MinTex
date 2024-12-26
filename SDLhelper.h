#ifndef SDLhelper_h
#define SDLhelper_h

#include <SDL.h>
#include <iostream>

inline void checkSDLError(int code) {
    if (code < 0) {
        std::cerr << (stderr, "SDL Initialisation error: %s\n", SDL_GetError());
        exit(1);
    }
}

template <typename T>
T checkSDLPointer(T ptr) {
    if (ptr == nullptr) {
        std::cerr << (stderr, "SDL Creation error: %s\n", SDL_GetError());
        exit(1);
    }
    return ptr;
}

#endif //SDLhelper_h