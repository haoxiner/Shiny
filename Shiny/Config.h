#pragma once
#include <string>
#include <map>
#include <vector>
#include <utility>
namespace Shiny
{

class Lexer
{
public:
    enum TokenType
    {
        TK_INVALID, TK_STRING, TK_NUMBER
    };
    Lexer(const std::string& text);
    std::pair<std::string, TokenType> NextAttribute();
    std::string GetString() const { return nextString_; }
    float GetNumber() const { return nextNumber_; }
    bool HasNextAttribute();
    bool HasError() const { return hasError_; }
private:
    std::pair<std::string, TokenType> nextAttribute_;
    std::string nextString_;
    float nextNumber_;

    std::string ScanString();
    float ScanNumber();
    void SkipSpacesAndComments();
    bool SkipSpaces();
    bool SkipComments();
    bool SkipComma();
    void Forward();
    const std::string text_;
    int row_ = 1;
    int column_ = 1;
    int offset_ = 0;
    bool hasError_ = false;
    bool nextIsReady_ = false;
};
class Config
{
public:
    bool Parse(const std::string& fileName);
    std::string GetString(const std::string& name) const;
    float GetFloat(const std::string& name) const;
    bool HasString(const std::string& name) const;
    bool HasNumber(const std::string& name) const;
private:
    
    std::map<std::string, std::string> stringValueMap_;
    std::map<std::string, float> numberValueMap_;
};
}