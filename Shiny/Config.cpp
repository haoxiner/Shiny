#include "Config.h"
#include "ResourceManager.h"
#include <cctype>

class Lexer
{
public:
    Lexer(const std::string& text) : text_(text)
    {

    }
private:
    void ScanNumber()
    {
        int start = offset_;
        int startRow = row_;
        int startColumn = column_;
        char ch = text_[offset_];
        while (offset_ < text_.length() &&
            (std::isalnum(ch) || '+' == ch || '-' == ch || '.' == ch)) {
            Forward();
            ch = text_[offset_];
        }
        std::string content = text_.substr(start, offset_);
        float value = std::stof(content);
    }
    void SkipSpacesAndComments()
    {
        while (SkipSpaces() || SkipComments()) {
            // break until there are no spaces and comments
        }
    }
    bool SkipSpaces()
    {
        bool found = false;
        if (offset_ < text_.length() && std::isspace(text_[offset_])) {
            found = true;
            while (offset_ < text_.length() && std::isspace(text_[offset_])) {
                Forward();
            }
        }
        return found;
    }
    bool SkipComments()
    {
        bool found = false;
        if (text_.find("//", offset_) == 0) {
            found = true;
            int nextRow = row_ + 1;
            while (offset_ < text_.length() && row_ < nextRow) {
                Forward();
            }
        }
        return found;
    }
    void Forward()
    {
        if (text_[offset_] == '\n') {
            column_ = 1;
            row_++;
        } else {
            column_++;
        }
        offset_++;
    }
    std::string text_;
    int row_ = 1;
    int column_ = 1;
    int offset_ = 0;
};

Shiny::Config::Config(const std::string& fileName)
{
    auto fileContent = ResourceManager::ReadFileToString(fileName);

}
