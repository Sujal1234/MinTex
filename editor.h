#ifndef editor_h
#define editor_h

#include <cstddef>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>
#include "SDLhelper.h"

class Line {
public:
    Line() = default;
    
    //std::size_t max_size{ 1024 };
    std::size_t size{ 0 };
    std::string text{};

    void insertTextBefore(std::size_t pos, const char* text);
    void backspaceAt(std::size_t pos, std::size_t amount);
    void deleteAt(std::size_t pos, std::size_t amount);
};

class Editor {
public:
    Editor() = default;

    std::vector<Line> lines = std::vector<Line>(1);
    std::size_t numLines{ 1 };
    std::size_t cursorLine{ 0 };
    std::size_t cursorPos{ 0 };

    void insertTextBeforeCursor(const char* text);
    void backspaceAtCursor(std::size_t amount);
    void deleteAtCursor(std::size_t amount);

    void insertLine(size_t pos);
    void moveCursorHorizontal(int x);
    void moveCursorVertical(int y);

    char currentChar(); //use only when sure that cursor is on a character.
    Line& currentLine();

    void renderCursor(SDL_Renderer * renderer, const int charWidth, const int charHeight);
    void renderText(SDL_Renderer* renderer, TTF_Font* font);
};

#endif //editor_h