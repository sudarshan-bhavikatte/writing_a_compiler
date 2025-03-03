#pragma once

#include <cassert>
#include <unordered_map>
#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <variant>

#include "parser.hpp"
using namespace std;

class Generator {
public:
    explicit Generator(NodeProg prog)
        : m_prog(move(prog))
    {}

    void gen_term(const NodeTerm* term) {
        struct TermVisitor {
            Generator* gen;
            void operator()(NodeTermINT_LIT* term_int_lit) const{
                gen->m_output << "    mov rax, " << term_int_lit->INT_LIT.value.value() << "\n";
                gen->push("rax");
            }
          void operator()(NodeTermIDENT* term_ident) const {
                if (!gen->m_vars.contains(term_ident->IDENT.value.value())) {
                    cerr << "undefined variable "
                         << term_ident->IDENT.value.value() << "\n";
                    exit(EXIT_FAILURE);
                }
                const auto& var = gen->m_vars.at(term_ident->IDENT.value.value());
                stringstream offset;
                offset << "QWORD [rsp + "
                       << (gen->m_stack_size - var.stack_loc - 1) * 8 << "]";
                gen->push(offset.str());
            }
        };
        TermVisitor visitor({.gen = this});
        visit(visitor, term->vari);
    }

    void gen_expr(const NodeExpr* expr) {
        struct ExprVisitor {
            Generator* gen;

            void operator()(const NodeTerm* term) {
                gen->gen_term(term);
            }

            void operator()(const NodeBinExpr* bin_expr) {
                gen->gen_expr(bin_expr->add->lhs);
                gen->gen_expr(bin_expr->add->rhs);
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "    add rax, rbx\n";
                gen->push("rax");
            }
        };
        ExprVisitor visitor { this };
        std::visit(visitor, expr->vari);
    }

    void gen_stmt(const NodeStmt* stmt) {
        struct StmtVisitor {
            Generator* gen;
            void operator()(const NodeStmtExit* stmt_exit) const {
                gen->gen_expr(stmt_exit->expr);
                gen->m_output << "    mov rax, 60\n";
                gen->pop("rdi");
                gen->m_output << "    syscall\n";
            }
            void operator()(const NodeStmtLet* stmt_let) {
                if (gen->m_vars.contains(stmt_let->ident.value.value())) {
                    cerr << "Cannot use the same variable name more than once: "
                         << stmt_let->ident.value.value() << "\n";
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({ stmt_let->ident.value.value(),
                                     vari { .stack_loc = gen->m_stack_size } });
                gen->gen_expr(stmt_let->expr);
            }
        };
        StmtVisitor visitor { this };
        std::visit(visitor, stmt->vari);
    }

    string gen_prog() {
        m_output << "global _start\n_start:\n";
        for (const auto& stmt : m_prog.stmts) {
            gen_stmt(&stmt);
        }
        return m_output.str();
    }
private:
    void push(const string& reg) {
        m_output << "    push " << reg << "\n";
        m_stack_size++;
    }
    void pop(const string& reg) {
        m_output << "    pop " << reg << "\n";
        m_stack_size--;
    }
    struct vari {
        size_t stack_loc;
    };

    NodeProg m_prog;
    stringstream m_output;
    size_t m_stack_size = 0;
    unordered_map<string, vari> m_vars;
};
