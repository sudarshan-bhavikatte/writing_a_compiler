#pragma once
using namespace std;
enum class  TokenType {
    EXIT,
    INT_LIT,
    SEMICOLON,
    OPEN_PAREN,
    CLOSE_PAREN,
    IDENT,
    LET,
    EQUALS,
    PLUS,
};

struct Token {
    TokenType type;
    optional<string> value;
};

class Tokenizer
{
public:
    inline explicit Tokenizer(const string& src )
        : m_src(move(src))
    {

    };
    inline vector<Token> tokenize()
    {
        vector<Token> tokens;
        string buf;

        while (peek().has_value())
        {
            if (isalpha(peek().value()))
            {
                buf.push_back(consume());
                while (peek().has_value() && isalnum(peek().value()))
                {
                    buf.push_back(consume());
                }
                if (buf == "exit")
                {
                    tokens.push_back({.type = TokenType::EXIT});
                    buf.clear();
                    continue;
                }
                else if (buf == "let") {
                    tokens.push_back({.type = TokenType::LET});
                    buf.clear();
                    continue;
                }

                else
                {
                    tokens.push_back({.type = TokenType::IDENT, .value = buf});
                    buf.clear();
                    continue;
                }
            }
            else if (isdigit(peek().value()))
            {
                buf.push_back(consume());
                while (peek().has_value() && isdigit(peek().value()))
                {
                    buf.push_back(consume());
                }
                tokens.push_back({.type = TokenType::INT_LIT, .value = buf});
                buf.clear();
                continue;
            }
            else if (peek().value() == '(') {
                tokens.push_back({.type = TokenType::OPEN_PAREN});
                consume();
            }
            else if (peek().value() == ')') {
                tokens.push_back({.type = TokenType::CLOSE_PAREN});
                consume();
            }
            else if (peek().value() == ';')
            {
                tokens.push_back({.type = TokenType::SEMICOLON});
                consume();
                continue;
            }
            else if (peek().value() == '=')
            {
                tokens.push_back({.type = TokenType::EQUALS});
                consume();
                continue;
            }
            else if (peek().value() == '+') {
                consume();
                tokens.push_back({.type = TokenType::PLUS});
                continue;
            }
            else if (isspace(peek().value()))
            {
                consume();
                continue;
            }

            else
            {
                cerr << "Error : " << buf << endl;
                exit(EXIT_FAILURE);
            }
        }

        m_index = 0;
        return tokens;
    }

private:
    [[nodiscard]] inline optional<char> peek(int offset = 0) const
    {
        if (m_index + offset >= m_src.length())
        {
            return {};
        }
        else
        {
            return m_src.at(m_index + offset);
        }
    }
    inline char consume()
    {
        return m_src.at(m_index++);
    }
    const string m_src;
    size_t m_index = 0;
};