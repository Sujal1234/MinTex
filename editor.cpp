#include "editor.h"
#include <string>
#include <cassert>
#include <vector>
#include <limits>

#include <SDL.h>
#include <SDL_ttf.h>


void Editor::insertTextBeforeCursor(const char* text) {
    //if (numLines == 0) numLines++;

    if (cursorLine >= lines.size()) {
        assert(cursorLine == lines.size());
        lines.emplace_back();
    }
    lines[cursorLine].insertTextBefore(cursorPos, text);
    cursorPos += strlen(text);
}

void Editor::backspaceAtCursor(std::size_t amount) {
    if (cursorLine >= lines.size()) {
        assert(cursorLine == lines.size());
        lines.emplace_back();
    }
    lines[cursorLine].backspaceAt(cursorPos, amount);
    cursorPos -= std::min(amount, cursorPos);
}

void Editor::deleteAtCursor(std::size_t amount) {
    if (cursorLine >= lines.size()) {
        assert(cursorLine == lines.size());
        lines.emplace_back();
    }
    lines[cursorLine].deleteAt(cursorPos, amount);
}

void Editor::insertLine(size_t pos) {
    Line temp;
    lines.insert(lines.begin() + pos + 1, temp);
    cursorLine++;
    cursorPos = 0;
    numLines++;
}

void Editor::moveCursorHorizontal(int x) {
    //make sure cursorPos doesn't become -ve
    if (x < 0 && (-x) > cursorPos) {
        cursorPos = 0;
        return;
    }
    if (x < 0) {
        cursorPos += x;
        return;
    }

    //make sure cursorPos doesn't exceed overflow.
    constexpr std::size_t maxSize_t{ std::numeric_limits<size_t>::max() };
    if (x > maxSize_t  - cursorPos) {
        cursorPos = lines[cursorLine].size;
        return;
    }

    //Don't go past end of line
    if (x + cursorPos > lines[cursorLine].size) {
        cursorPos = lines[cursorLine].size;
        return;
    }
    cursorPos += x;
}

void Editor::moveCursorVertical(int y) {
    //make sure cursorLine doesn't become -ve
    if (y < 0 && (-y) > cursorLine) {
        cursorLine = 0;
        //snap to the end of the line in case cursorPos exceeds the size of the line
        if (cursorPos > lines[cursorLine].size) {
            cursorPos = lines[cursorLine].size;
        }
        return;
    }

    if (y < 0) {
        cursorLine += y;
        if (cursorPos > lines[cursorLine].size) {
            cursorPos = lines[cursorLine].size;
        }
        return;
    }

    //make sure cursorLine doesn't overflow
    constexpr std::size_t maxSize_t{ std::numeric_limits<size_t>::max() };
    if (y > maxSize_t - cursorLine) {
        cursorLine = numLines;
        //snap to the end of the line in case cursorPos exceeds the size of the line
        if (cursorPos > lines[cursorLine].size) {
            cursorPos = lines[cursorLine].size;
        }
        return;
    }

    //Don't go past last line
    if (y + cursorLine > lines.size() - 1) {
        cursorLine = lines.size() - 1;
        if (cursorPos > lines[cursorLine].size) {
            cursorPos = lines[cursorLine].size;
        }
        return;
    }
    cursorLine += y;
    if (cursorPos > lines[cursorLine].size) {
        cursorPos = lines[cursorLine].size;
    }
}

Line& Editor::currentLine() {
    return lines[cursorLine];
}

char Editor::currentChar() {
    assert(cursorPos < lines[cursorLine].size);
    return lines[cursorLine].text[cursorPos];
}

void Editor::renderCursor(SDL_Renderer* renderer, const int charWidth, const int charHeight)
{
    SDL_Rect rect = {
    .x = (int)cursorPos * charWidth,
    .y = (int)cursorLine * charHeight,
    .w = charWidth,
    .h = charHeight
    };

    Uint8 alpha = 255; //Uint8 is a type defined in SDL

    //Make cursor semi-transparent if it's placed over text
    if (lines[cursorLine].size > 0 &&
        cursorPos < lines[cursorLine].size &&
        !std::isspace(this->currentChar())
        )
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

void Editor::renderText(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color color = { 255, 255, 255, 255 };

    SDL_Window* renderWindow = SDL_RenderGetWindow(renderer);


    //int windowWidth;
    //SDL_GetWindowSize(renderWindow, &windowWidth, nullptr);

    int fontHeight;
    TTF_SizeUTF8(font, "a", NULL, &fontHeight);
    for (size_t i = 0; i < numLines; i++)
    {
        if (lines[i].text.size() == 0) {
            continue;
        }

        SDL_Surface* fontSurface = checkSDLPointer(
            //converting std::string to a c type string because SDL_ttf uses that.
            TTF_RenderUTF8_Blended(font, lines[i].text.c_str(), color)
        );

        SDL_Texture* fontTexture = checkSDLPointer(
            SDL_CreateTextureFromSurface(renderer, fontSurface)
        );

        SDL_Rect rect{
            .x = 0,
            .y = fontHeight * (int) i,
            .w = fontSurface->w,
            .h = fontSurface->h
        };

        SDL_RenderCopy(renderer, fontTexture, nullptr, &rect);

        SDL_FreeSurface(fontSurface);
        SDL_DestroyTexture(fontTexture);
    }
}