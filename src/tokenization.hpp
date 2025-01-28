#pragma once
using namespace std;
enum class  TokenType {
    EXIT,
    INT_LIT,
    SEMICOLON,
};

struct Token {
    TokenType type;
    optional<string> value;
};

class Tokenizer
{
public:
    inline explicit Tokenizer(const string& src )
        : m_src(src)
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
                else
                {
                    cerr << "Unknown keyword: " << buf << endl;
                    exit(EXIT_FAILURE);
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
            else if (peek().value() == ';')
            {
                tokens.push_back({.type = TokenType::SEMICOLON});
                consume();
                continue;
            }
            else if (isspace(peek().value()))
            {
                consume();
                continue;
            }
            else
            {
                cerr << "Unexpected character: '" << peek().value() << "' at index " << m_index << endl;
                exit(EXIT_FAILURE);
            }
        }

        m_index = 0;
        return tokens;
    }

private:
    [[nodiscard]] inline optional<char> peek(int ahead = 1) const
    {
        if (m_index + ahead > m_src.length())
        {
            return {};
        }
        else
        {
            return m_src.at(m_index);
        }
    }
    inline char consume()
    {
        return m_src.at(m_index++);
    }
    const string m_src;
    size_t m_index = 0;
};