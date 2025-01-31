#pragma once
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

#include "tokenization.hpp"
using namespace std;

struct NodeExprINT_LIT {
    Token INT_LIT;
};

struct NodeExprIDENT {
    Token IDENT;
};

struct NodeExpr {
    variant<NodeExprINT_LIT, NodeExprIDENT> vari;
};

struct NodeStmtExit {
    NodeExpr expr;
};

struct NodeStmtLet {
    Token ident;
    NodeExpr expr;
};

struct NodeStmt {
    variant<NodeStmtExit, NodeStmtLet> vari;
};



struct NodeProg {
    vector<NodeStmt> stmts;
};


class Parser {
public:
    inline explicit Parser(vector<Token> tokens) : m_tokens(move(tokens)) {}

    inline optional<NodeExpr> parse_expr() {
        if (peek().has_value()) {
            Token token = consume().value();
            if (token.type == TokenType::INT_LIT) {
                return NodeExpr{.vari = NodeExprINT_LIT{.INT_LIT = token}};
            }
            else if (token.type == TokenType::IDENT) {
                return NodeExpr{.vari = NodeExprIDENT{.IDENT = token}};
            }
        }
        return {};
    }

    inline optional<NodeStmt> parse_stmt() {
        if (peek().has_value() && peek().value().type == TokenType::EXIT &&
            peek(1).has_value() && peek(1).value().type == TokenType::OPEN_PAREN) {

            consume();
            consume();

            optional<NodeExpr> node_expr = parse_expr();
            if (!node_expr.has_value()) {
                cerr << "Parser: Failed to parse expression 1" << endl;
                exit(EXIT_FAILURE);
            }

            if (!peek().has_value() || peek().value().type != TokenType::CLOSE_PAREN) {
                cerr << "Parser: Expected ')' " << endl;
                exit(EXIT_FAILURE);
            }
            consume(); // Consume ')'

            if (!peek().has_value() || peek().value().type != TokenType::SEMICOLON) {
                cerr << "Parser: Expected ';'" << endl;
                exit(EXIT_FAILURE);
            }
            consume(); // Consume ';'

            return NodeStmt{.vari = NodeStmtExit{.expr = node_expr.value()}};
        }
        else if (peek().has_value() && peek().value().type == TokenType::LET &&
                 peek(1).has_value() && peek(1).value().type == TokenType::IDENT &&
                 peek(2).has_value() && peek(2).value().type == TokenType::EQUALS) {

            consume(); // 'let'
            Token ident = consume().value(); // Identifier
            consume(); // '='

            optional<NodeExpr> node_expr = parse_expr();
            if (!node_expr.has_value()) {
                cerr << "Parser: Failed to parse expression 2" << endl;
                exit(EXIT_FAILURE);
            }

            if (!peek().has_value() || peek().value().type != TokenType::SEMICOLON) {
                cerr << "Parser: Expected ';'" << endl;
                exit(EXIT_FAILURE);
            }
            consume(); // Consume ';'

            return NodeStmt{.vari = NodeStmtLet{.ident = ident, .expr = node_expr.value()}};
        }
        return {};
    }

    inline optional<NodeProg> parse_prog() {
        NodeProg prog;
        while (peek().has_value()) {
            optional<NodeStmt> stmt = parse_stmt();
            if (!stmt.has_value()) {
                cerr << "Parser: Failed to parse statement" << endl;
                exit(EXIT_FAILURE);
            }
            prog.stmts.push_back(stmt.value());
        }
        return prog;
    }

private:
    [[nodiscard]] inline optional<Token> peek(int offset = 0) const {
        if (m_index + offset >= m_tokens.size()) {
            return {};
        }
        return m_tokens.at(m_index + offset);
    }

    inline optional<Token> consume() {
        if (m_index < m_tokens.size()) {
            return m_tokens.at(m_index++);
        }
        return {};
    }

    vector<Token> m_tokens;
    size_t m_index = 0;
};
