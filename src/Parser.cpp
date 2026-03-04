#include "Parser.h"

#include <iostream>

#include "Colors.hpp"

Parser* GetParser(const std::string& fileName) {
    Parser *parser = nullptr;

    // Проверяем, не является ли файл файлом формата SystemVerilog
    parser = new Parser_SystemVerilog;
    if (parser->IsMyFormat(fileName)) {
        std::cout << FORMAT_INFO "SystemVerilog format detected for file '" << fileName << "'\n";
        return parser;
    }
    delete parser;
    parser = nullptr;

    // Проверяем, не является ли файл файлом формата Verilog
    parser = new Parser_Verilog;
    if (parser->IsMyFormat(fileName)) {
        std::cout << FORMAT_INFO "Verilog format detected for file '" << fileName << "'\n";
        return parser;
    }
    delete parser;
    parser = nullptr;

    // Проверяем, не является ли файл файлом формата VHDL
    parser = new Parser_VHDL;
    if (parser->IsMyFormat(fileName)) {
        std::cout << FORMAT_INFO "VHDL format detected for file '" << fileName << "'\n";
        return parser;
    }
    delete parser;
    parser = nullptr;

    return nullptr;
}

bool Parser::LoadFile(const std::string& filename)
{
    std::string line;
    lines.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    while (std::getline(file, line))
    {
        lines.push_back(line);
    }
    return true;
}

std::string Parser::Trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";

    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

std::string Parser::CleanToken(std::string& str)
{
    str = Trim(str);

    // Удаляем скобки, запятые, точки с запятой
    const std::string charsToRemove = "(){}[],;";

    for (char c : charsToRemove) {
        str.erase(std::remove(str.begin(), str.end(), c), str.end());
    }

    return Trim(str);
}

std::string Parser::ExtractTag(std::string& text)
{
    for (const auto& tag : tags)
    {
        size_t pos = text.find(tag);
        if (pos != std::string::npos)
        {
            text.erase(pos, tag.length());
            return tag;
        }
    }
    return "";
}

std::vector<Comment_block> Parser::Parse_CommentLine(const std::string& comment_text)
{
    std::vector<Comment_block> blocks;
    std::string remaining = Trim(comment_text);
    std::string current_tag;
    std::string current_text;

    while (!remaining.empty())
    {
        // Ищем позицию следующего тега
        size_t next_tag_pos = std::string::npos;
        std::string next_tag;

        for (const auto& tag : tags)
        {
            size_t found = remaining.find(tag);
            if (found != std::string::npos)
            {
                if (next_tag_pos == std::string::npos || found < next_tag_pos)
                {
                    next_tag_pos = found;
                    next_tag = tag;
                }
            }
        }

        if (next_tag_pos != std::string::npos)
        {
            // Если это не первый тег и есть накопленный текст
            if (!current_tag.empty() && !current_text.empty())
            {
                Comment_block block;
                block.tag = current_tag;
                block.lines.push_back(Trim(current_text));
                blocks.push_back(block);
                current_text.clear();
            }
            else if (current_tag.empty() && next_tag_pos > 0)
            {
                // Текст перед первым тегом
                std::string before_first_tag = Trim(remaining.substr(0, next_tag_pos));
                if (!before_first_tag.empty())
                {
                    Comment_block block;
                    block.tag = ""; // Текст без тега
                    block.lines.push_back(before_first_tag);
                    blocks.push_back(block);
                }
            }

            // Начинаем новый блок с текущим тегом
            current_tag = next_tag;
            current_text = remaining.substr(next_tag_pos + next_tag.length());
            remaining = ""; // Очищаем, так как мы взяли весь остаток

            // Проверяем, есть ли еще теги в оставшемся тексте
            size_t another_tag_pos = std::string::npos;
            for (const auto& tag : tags)
            {
                size_t found = current_text.find(tag);
                if (found != std::string::npos)
                {
                    another_tag_pos = found;
                    break;
                }
            }

            if (another_tag_pos != std::string::npos)
            {
                // Есть еще теги - разделяем
                remaining = current_text.substr(another_tag_pos);
                current_text = current_text.substr(0, another_tag_pos);
            }
        }
        else
        {
            // Нет больше тегов
            if (!current_tag.empty())
            {
                if (!current_text.empty())
                    current_text += " ";
                current_text += remaining;
            }
            else if (!remaining.empty())
            {
                // Текст без тега
                Comment_block block;
                block.tag = "";
                block.lines.push_back(Trim(remaining));
                blocks.push_back(block);
            }
            break;
        }
    }

    // Сохраняем последний блок, если есть
    if (!current_tag.empty() && !current_text.empty())
    {
        Comment_block block;
        block.tag = current_tag;
        block.lines.push_back(Trim(current_text));
        blocks.push_back(block);
    }

    return blocks;
}

std::vector<Comment_block> Parser::Parse_CommentLine(const std::vector<std::string>& comment_lines)
{
    std::vector<Comment_block> result;

    if (comment_lines.empty())
        return result;

    // Текущий накапливаемый блок
    Comment_block current_block;
    std::string current_tag;

    for (const auto& raw_line : comment_lines)
    {
        if (raw_line.empty()) continue;

        // Копируем строку для обработки
        std::string line = raw_line;

        // Ищем тег в строке (так же, как в однострочном варианте)
        std::string tag = ExtractTag(line);

        line = Trim(line);
        

        if (!tag.empty())
        {
            // Нашли новый тег

            // Если есть накопленный блок с предыдущим тегом - сохраняем его
            if (!current_block.lines.empty())
            {
                result.push_back(current_block);
                current_block = Comment_block();
            }

            // Начинаем новый блок с этим тегом
            current_block.tag = tag;
            if (!line.empty())
            {
                current_block.lines.push_back(line);
            }
        }
        else if (!current_block.tag.empty())
        {
            // Нет тега, но есть текущий блок - добавляем строку в него
            if (!line.empty())
            {
                current_block.lines.push_back(line);
            }
        }
        else
        {
            // Нет тега и нет текущего блока - создаём блок без тега
            Comment_block block;
            block.tag = "";
            if (!line.empty())
            {
                block.lines.push_back(line);
                result.push_back(block);
            }
        }
    }

    // Сохраняем последний накопленный блок
    if (!current_block.lines.empty())
    {
        result.push_back(current_block);
    }

    return result;
}

void FreeParser(Parser** parser) {
    delete (*parser);
    (*parser) = nullptr;
}
