#pragma once
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

#include "arena.hpp"
#include "tokenization.hpp"  // Assumed to define Token and TokenType
using namespace std;

struct NodeTermINT_LIT {
    Token INT_LIT;
};

struct NodeTermIDENT {
    Token IDENT;
};
struct NodeExpr;

// struct NodeBinExprMul {
//     NodeExpr* lhs;
//     NodeExpr* rhs;
// };

struct NodeBinExprAdd {
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExpr {
    NodeBinExprAdd* add;
};

struct NodeTerm {
    variant<NodeTermINT_LIT*, NodeTermIDENT*> vari;
};
struct NodeExpr {
    variant<NodeTerm*, NodeBinExpr*> vari;
};

struct NodeStmtExit {
    NodeExpr* expr;
};

struct NodeStmtLet {
    Token ident;
    NodeExpr* expr;
};

struct NodeStmt {
    variant<NodeStmtExit*, NodeStmtLet*> vari;
};

struct NodeProg {
    vector<NodeStmt> stmts;
};

class Parser {
public:
    explicit Parser(vector<Token> tokens)
        : m_tokens(move(tokens)),
          m_allocator(1024 * 1024 * 4)
    {}

    optional<NodeTerm*> parse_term() {
        if (auto int_lit = try_consume(TokenType::INT_LIT)) {
            auto Term_int_lit = m_allocator.alloc<NodeTermINT_LIT>();
            Term_int_lit->INT_LIT = int_lit.value();
            auto Term = m_allocator.alloc<NodeTerm>();
            Term->vari = Term_int_lit;
            return Term;
        }
        else if (auto ident = try_consume(TokenType::IDENT))  {
            auto Term_ident = m_allocator.alloc<NodeTermIDENT>();
            Term_ident->IDENT = ident.value();
            auto Term = m_allocator.alloc<NodeTerm>();
            Term->vari = Term_ident;
            return Term;
        }
        else {
            return {};
        }
    };

    optional<NodeExpr*> parse_expr() {

        if (auto term = parse_term()) {
            if(try_consume(TokenType::PLUS).has_value()) {
                auto bin_expr = m_allocator.alloc<NodeBinExpr>();
                    auto bin_expr_add = m_allocator.alloc<NodeBinExprAdd>();
                    auto lhs_expr = m_allocator.alloc<NodeExpr>();
                    lhs_expr->vari = term.value();
                    bin_expr_add->lhs = lhs_expr;
                    if (auto rhs = parse_expr()) {
                        bin_expr_add->rhs = rhs.value();
                        bin_expr->add = bin_expr_add;
                        auto expr = m_allocator.alloc<NodeExpr>();
                        expr->vari = bin_expr;
                        return expr;
                    }
                    else {
                        cerr << "Error parsing expression (expeccted expression)" << endl;
                        exit(EXIT_FAILURE);
                    }

            } else {
                auto expr = m_allocator.alloc<NodeExpr>();
                expr->vari = term.value();
                return expr;
            }
        } else {
            return {};
        }

    }

    optional<NodeStmt> parse_stmt() {
        if (peek().has_value() && peek().value().type == TokenType::EXIT &&
            peek(1).has_value() && peek(1).value().type == TokenType::OPEN_PAREN) {

            consume(); // 'exit'
            consume(); // '('

            auto node_expr = parse_expr();
            if (!node_expr.has_value()) {
                cerr << "Parser: Failed to parse expression" << endl;
                exit(EXIT_FAILURE);
            }

            try_consume(TokenType::CLOSE_PAREN, "expected )");
            try_consume(TokenType::SEMICOLON, "expected ;");


            auto exit_stmt = m_allocator.alloc<NodeStmtExit>();
            exit_stmt->expr = node_expr.value();
            return NodeStmt{ .vari = exit_stmt };
        }
        else if (peek().has_value() && peek().value().type == TokenType::LET &&
                 peek(1).has_value() && peek(1).value().type == TokenType::IDENT &&
                 peek(2).has_value() && peek(2).value().type == TokenType::EQUALS) {

            consume(); // 'let'
            Token ident = consume(); // Identifier
            consume(); // '='

            auto node_expr = parse_expr();
            if (!node_expr.has_value()) {
                cerr << "Parser: Failed to parse expression" << endl;
                exit(EXIT_FAILURE);
            }

            try_consume(TokenType::SEMICOLON, "expected ;");

            auto let_stmt = m_allocator.alloc<NodeStmtLet>();
            let_stmt->ident = ident;
            let_stmt->expr = node_expr.value();
            return NodeStmt{ .vari = let_stmt };
        }
        return {};
    }

    optional<NodeProg> parse_prog() {
        NodeProg prog;
        while (peek().has_value()) {
            auto stmt = parse_stmt();
            if (!stmt.has_value()) {
                cerr << "Parser: Failed to parse statement" << endl;
                exit(EXIT_FAILURE);
            }
            prog.stmts.push_back(stmt.value());
        }
        return prog;
    }

private:
    [[nodiscard]] optional<Token> peek(int offset = 0) const {
        if (m_index + offset >= m_tokens.size()) {
            return {};
        }
        return m_tokens.at(m_index + offset);
    }

    Token consume() {
        if (m_index < m_tokens.size()) {
            return m_tokens.at(m_index++);
        }
        return {};
    }

    inline optional<Token> try_consume(TokenType type) {
        if (peek().has_value() || peek().value().type == type) {
            return consume();
        }
        else {
            return  {};
        }
    }

    inline optional<Token> try_consume(TokenType type , const string& err_msg) {
        if (peek().has_value() || peek().value().type == type) {
            return consume();
        }
        else {
            cerr << err_msg << endl;
            return  {};
        }
    }

    vector<Token> m_tokens;
    size_t m_index = 0;
    ArenaAllocator m_allocator;
};
