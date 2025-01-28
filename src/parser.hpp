#pragma once
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include "tokenization.hpp"
using namespace std;

struct NodeExpr {
    Token INT_LIT;
};

struct NodeExit {
    NodeExpr expr;
};


class Parser {
public:
    inline explicit Parser(vector<Token> tokens)
    : m_tokens(move(tokens))
    {
    };

    inline optional<NodeExpr> parse_expr() {
        if (peek().has_value() && peek().value().type == TokenType::INT_LIT) {
            return NodeExpr{
            .INT_LIT = consume()
            };
        }
        else {
            return {};
        }
    };

    inline optional<NodeExit> parse() {
        optional<NodeExit> node_exit;
        while (peek().has_value()) {
            if (peek().value().type == TokenType::EXIT) {
                consume();
                if (auto node_expr = parse_expr()) {
                    node_exit = NodeExit{.expr = node_expr.value()};
                }
                else {
                    cerr << "Parser: Failed to parse expression 1" << endl;
                    exit(EXIT_FAILURE);
                }
                if (peek().has_value() && peek().value().type == TokenType::SEMICOLON) {
                    consume();
                }
                else {
                    cerr << "Parser: Failed to parse expression 2" << endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
        m_index = 0;
        return node_exit;
    };
private:
    [[nodiscard]] inline optional<Token> peek(int ahead = 1) const
    {
        if (m_index + ahead > m_tokens.size())
        {
            return {};
        }
        else
        {
            return m_tokens.at(m_index);
        }
    }
    inline Token consume()
    {
        return m_tokens.at(m_index++);
    }
    vector<Token> m_tokens;
    size_t m_index = 0;
};
