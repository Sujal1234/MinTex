#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <cstddef>
#include <cassert>

#include "editor.h"

constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 600;
constexpr int FONT_SIZE = 28;

Line line;

size_t cursorLine = 0;
size_t cursorPos = 0;


void checkSDLError(int code) {
    if (code < 0) {
        fprintf(stderr, "SDL Initialisation error: %s\n", SDL_GetError());
        exit(1);
    }
}

template <typename T>
T checkSDLPointer(T ptr) {
    if (ptr == nullptr) {
        fprintf(stderr, "SDL Creation error: %s\n", SDL_GetError());
        exit(1);
    }
    return ptr;
}

static void writeText(SDL_Renderer* renderer, TTF_Font* font, const char* text) {
    SDL_Color color = { 255, 255, 255, 255 };

    SDL_Window* renderWindow = SDL_RenderGetWindow(renderer);


    int windowWidth;
    SDL_GetWindowSize(renderWindow, &windowWidth, nullptr);

    SDL_Surface* fontSurface = checkSDLPointer(
        TTF_RenderUTF8_Blended_Wrapped(font, text, color, windowWidth)
    );

    SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);

    SDL_Rect rect{
        .x = 0,
        .y = 0,
        .w = fontSurface->w,
        .h = fontSurface->h
    };

    SDL_RenderCopy(renderer, fontTexture, nullptr, &rect);

    SDL_FreeSurface(fontSurface);
    SDL_DestroyTexture(fontTexture);
}

static void renderCursor(SDL_Renderer* renderer, int charWidth, int charHeight) {
    SDL_Rect rect = {
    .x = (int)cursorPos * charWidth,
    .y = 0,
    .w = charWidth,
    .h = charHeight
    };

    //TODO:
    //Tune the cursor colour and alpha later.
    Uint8 alpha = 255; //Uint8 is a type defined in SDL

    //Make cursor semi-transparent if it's placed over text
    //TODO: Fix overflow!!!!!!!!!!!!
    if (line.size > 0 &&
        cursorPos < line.size &&
        !isspace(line.text[cursorPos]))
    {

        alpha = 128;
    }
    checkSDLError(
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha)
    );
    checkSDLError(
        SDL_RenderFillRect(renderer, &rect)
    );
}

int main(int argc, char* argv[]) {
    //line.size = 1;

    //Prevents rescaling of the window and textures so now my fonts are rendered at the correct
    //size and resolution.
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "system");

    //SDL_SetHint(S)

    checkSDLError(SDL_Init(SDL_INIT_VIDEO));
    SDL_Window* window =
        checkSDLPointer(
            SDL_CreateWindow(
                "NoobText", SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT,
                SDL_WINDOW_RESIZABLE
            )
        );

    SDL_Renderer* renderer =
        checkSDLPointer(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

    //Enables blending which allows textures to have non-255 opacity
    checkSDLError(
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND)
    );


    checkSDLError(TTF_Init());

    TTF_Font* font = TTF_OpenFont("FiraCode-Regular.ttf", FONT_SIZE);

    int fontWidth, fontHeight;
    TTF_SizeUTF8(font, "a", &fontWidth, &fontHeight);
    fontHeight = TTF_FontHeight(font);

    if (font == nullptr) {
        printf("Font error: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }

    bool running = true;
    while (running) {

        //Set background color to black and clear the window.
        checkSDLError(
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255)
        );
        checkSDLError(
            SDL_RenderClear(renderer)
        );

        SDL_Event event { 0 };

        while (SDL_PollEvent(&event)) {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                //For non-text keys
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;

                case SDLK_BACKSPACE:
                    line.backspaceAt(cursorPos, 1);
                    if (cursorPos > 0) {
                        cursorPos--;
                    }
                    break;

                case SDLK_DELETE:
                    line.deleteAt(cursorPos, 1);
                    break;

                case SDLK_LEFT:
                    if (cursorPos > 0) {
                        cursorPos--;
                    }
                    break;

                case SDLK_RIGHT:
                    if (cursorPos < line.size) {
                        cursorPos++;
                    }
                    break;
                }
                break;
            case SDL_TEXTINPUT:
                line.insertTextBefore(cursorPos, event.text.text);
                cursorPos += strlen(event.text.text);
            default:
                break;
            }
        }


        if (line.size > 0) {
            //converting std::string to a c type string because SDL_ttf uses that.
            writeText(renderer, font, line.text.c_str());
        }

        //Draw cursor
        renderCursor(renderer, fontWidth, fontHeight);


        //Present to the screen
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();

    return 0;
}