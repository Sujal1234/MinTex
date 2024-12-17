#include "editor.h"
#include <string>
#include <assert.h>

void Line::insertTextBefore(std::size_t pos, const char* insertText) {
    //when pos == line.size we are appending text.
    assert(pos <= size);
    text.insert(pos, insertText);
    size = text.size();
}

void Line::backspaceAt(std::size_t pos, std::size_t amount) {
    if (amount > pos) {
        amount = pos;
    }
        
    text.erase(pos - amount, amount);
    size = text.size();
}

void Line::deleteAt(std::size_t pos, std::size_t amount) {
    if (pos >= size - 1) {
        return;
    }
    std::size_t rightofcursor = size - pos - 1;//excluding cursor's current position

    if (amount > rightofcursor) {
        amount = rightofcursor;
    }

    text.erase(pos, amount);
    size = text.size();
}