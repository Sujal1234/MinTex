#ifndef editor_h
#define editor_h

#include <cstddef>
#include <string>

class Line {
public:
    std::size_t max_size{ 1024 };
    std::size_t size{ 0 };
    std::string text{ "" };

    void insertTextBefore(std::size_t pos, const char* text);
    void backspaceAt(std::size_t pos, std::size_t amount);
    void deleteAt(std::size_t pos, std::size_t amount);
};

#endif //editor_h