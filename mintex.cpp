#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <cstddef>
#include <cassert>
#include <cctype>

#include "editor.h"
#include "SDLhelper.h" //For SDL error handling functions

constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 600;
constexpr int FONT_SIZE = 28;

Editor editor{};

int main(int argc, char* argv[]) {
    //Prevents rescaling of the window and textures so now my fonts are rendered 
    //at the correct size and resolution.
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "system");

    checkSDLError(SDL_Init(SDL_INIT_VIDEO));
    SDL_Window* window =
        checkSDLPointer(
            SDL_CreateWindow(
                "MinTex", SDL_WINDOWPOS_UNDEFINED,
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
                    editor.backspaceAtCursor(1);
                    break;

                case SDLK_DELETE:
                    editor.deleteAtCursor(1);
                    break;

                case SDLK_LEFT:
                    /*if (editor.cursorPos > 0) {
                        editor.cursorPos--;
                    }*/
                    editor.moveCursorHorizontal(-1);
                    break;

                case SDLK_RIGHT:
                    /*if (editor.cursorPos < editor.currentLine().size) {
                        editor.cursorPos++;
                    }*/
                    editor.moveCursorHorizontal(1);
                    break;

                case SDLK_UP:
                    editor.moveCursorVertical(-1);
                    break;

                case SDLK_DOWN:
                    editor.moveCursorVertical(1);
                    break;

                case SDLK_RETURN:
                    editor.insertLine(editor.cursorLine);
                    break;
                }
                break;
            case SDL_TEXTINPUT:
                editor.insertTextBeforeCursor(event.text.text);
                break;

            default:
                break;
            }
        }


        editor.renderText(renderer, font);

        editor.renderCursor(renderer, fontWidth, fontHeight);

        //Present to the screen
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();

    return 0;
}