#include "Config.h"
#include "ResourceManager.h"
#include <cctype>
#include <utility>
#include <iostream>

bool Shiny::Config::Parse(const std::string& fileName)
{
    auto fileContent = ResourceManager::ReadFileToString(fileName);
    Lexer lexer(fileContent);
    bool hasError = true;
    while (lexer.HasNextAttribute()) {
        auto attribute = lexer.NextAttribute();
        auto name = attribute.first;
        auto tokenType = attribute.second;
        switch (tokenType) {
        case Shiny::Lexer::TK_INVALID:
            return false;
            break;
        case Shiny::Lexer::TK_STRING:
            stringValueMap_[name] = lexer.GetString();
            break;
        case Shiny::Lexer::TK_NUMBER:
            stringValueMap_[name] = lexer.GetNumber();
            break;
        }
    }
    return !hasError;
}

std::string Shiny::Config::GetString(const std::string& name) const
{
    auto iter = stringValueMap_.find(name);
    if (iter != stringValueMap_.end()) {
        return iter->second;
    }
    return "";
}

float Shiny::Config::GetFloat(const std::string& name) const
{
    auto iter = numberValueMap_.find(name);
    if (iter != numberValueMap_.end()) {
        return iter->second;
    }
    return 0.0f;
}

bool Shiny::Config::HasString(const std::string& name) const
{
    return (stringValueMap_.find(name) != stringValueMap_.end());
}

bool Shiny::Config::HasNumber(const std::string& name) const
{
    return (numberValueMap_.find(name) != numberValueMap_.end());
}

Shiny::Lexer::Lexer(const std::string& text) : text_(text), nextAttribute_("", TK_INVALID)
{
    SkipSpacesAndComments();
    if (offset_ < text_.length() && text_[offset_] == '{') {
        Forward();
    } else {
        hasError_ = true;
    }
}

std::pair<std::string, Shiny::Lexer::TokenType> Shiny::Lexer::NextAttribute()
{
    nextIsReady_ = false;
    return nextAttribute_;
}

bool Shiny::Lexer::HasNextAttribute()
{
    static int count = 0;
    count++;
    std::cerr << count << " count" << std::endl;
    if (nextIsReady_) {
        return true;
    }
    if (hasError_) {
        return false;
    }
    SkipSpacesAndComments();
    
    if (offset_ >= text_.length()) {
        return false;
    }
    
    if (text_[offset_] == '}') {
        return false;
    }
    
    // check name
    nextAttribute_.first = ScanString();
    std::cerr << nextAttribute_.first << std::endl;
    SkipSpacesAndComments();
    if (offset_ < text_.length()) {
        if (text_[offset_] != ':') {
            hasError_ = true;
        } else {
            Forward();
            SkipSpacesAndComments();
        }
    }
    nextAttribute_.second = TK_INVALID;
    if (hasError_) {
        return false;
    }
    // check value
    if (offset_ < text_.length()) {
        char ch = text_[offset_];

        // check string
        if ('"' == ch) {
            nextString_ = ScanString();
            if (hasError_) {
                return false;
            } else {
                nextAttribute_.second = TK_STRING;

            }
        } else {
            nextNumber_ = ScanNumber();
            if (hasError_) {
                return false;
            } else {
                nextAttribute_.second = TK_NUMBER;
            }
        }
    }
    
    SkipSpacesAndComments();
    if (text_[offset_] == '}') {
        Forward();
        return false;
    }
    nextIsReady_ = true;
    return true;
}

std::string Shiny::Lexer::ScanString()
{
    SkipSpacesAndComments();
    if (text_[offset_] != '"') {
        hasError_ = true;
        return "";
    }
    Forward();
    int start = offset_;
    int startRow = row_;
    int startColumn = column_;
    while (offset_ < text_.length() && text_[offset_] != '"') {
        Forward();
    }
    if (offset_ < text_.length() && text_[offset_] == '"') {
        Forward();
        return text_.substr(start, offset_ - start - 1);
    }
    hasError_ = true;
    return false;
}

float Shiny::Lexer::ScanNumber()
{
    SkipSpacesAndComments();
    int start = offset_;
    int startRow = row_;
    int startColumn = column_;
    char ch = text_[offset_];
    bool validNumber = false;
    while (offset_ < text_.length() &&
        (std::isalnum(ch) || '+' == ch || '-' == ch || '.' == ch)) {
        Forward();
        if (std::isalnum(ch)) {
            validNumber = true;
        }
        ch = text_[offset_];
    }
    if (!validNumber) {
        hasError_ = true;
        return 0.0f;
    }
    std::string content = text_.substr(start, offset_ - start);
    size_t lastPos;
    float value = std::stof(content, &lastPos);
    
    if (lastPos != content.length()) {
        hasError_ = true;
        return 0.0f;
    }
    
    return value;
}

void Shiny::Lexer::SkipSpacesAndComments()
{
    while (SkipSpaces() || SkipComments()) {
        // break until there are no spaces and comments
    }
}

bool Shiny::Lexer::SkipSpaces()
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

bool Shiny::Lexer::SkipComments()
{
    bool found = false;
    if (offset_ < text_.length() && text_.find("//", offset_) == 0) {
        found = true;
        int nextRow = row_ + 1;
        while (offset_ < text_.length() && row_ < nextRow) {
            Forward();
        }
    }
    return found;
}

bool Shiny::Lexer::SkipComma()
{
    SkipSpacesAndComments();
    if (offset_ < text_.length() && text_[offset_] == ',') {
        Forward();
        return true;
    }
    return false;
}

void Shiny::Lexer::Forward()
{
    if (offset_ < text_.length()) {
        if (text_[offset_] == '\n') {
            column_ = 1;
            row_++;
        } else {
            column_++;
        }
    }
    offset_++;
}