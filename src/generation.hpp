#pragma once
#include <unordered_map>

#include "parser.hpp"

using namespace std;
class Generator {
public:
    inline Generator(NodeProg prog)
        :m_prog(move(prog))
    {
    }

    inline void gen_expr(const NodeExpr expr)  {
        struct ExprVisitor {
            Generator* gen;

            void operator()(const NodeExprINT_LIT& expr_int_lit) {
                gen->m_output << "    mov rax, " << expr_int_lit.INT_LIT.value.value() << "\n";
                gen->push("rax");
            }
            void operator()(const NodeExprIDENT& expr_ident) {
                if (!gen->m_vars.contains(expr_ident.IDENT.value.value())) {
                    cerr << "undefined variable " << expr_ident.IDENT.value.value() << "\n";
                    exit(EXIT_FAILURE);
                }
                const auto& var = gen->m_vars.at(expr_ident.IDENT.value.value());
                stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stack_size - var.stack_loc -1 ) * 8 << "]\n";
                gen->push(offset.str());
            }
        };
        ExprVisitor visitor{.gen = this};
        visit(visitor,expr.vari);
    }

    inline void  gen_stmt(const NodeStmt& stmt)  {
        struct StmtVisitor {
            Generator* gen;
            void operator()(const NodeStmtExit& stmt_exit)const  {
                gen->gen_expr(stmt_exit.expr);
                gen->m_output << "    mov rax, 60\n";
                gen->pop("rdi");
                gen->m_output << "    syscall";
            }
            void operator()(const NodeStmtLet& stmt_let) {
                if (gen->m_vars.contains(stmt_let.ident.value.value())) {
                    cerr << "cant use same variable name more than once " << endl;
                    cerr << stmt_let.ident.value.value() << endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({stmt_let.ident.value.value(), vari {.stack_loc = gen->m_stack_size}});
                gen->gen_expr(stmt_let.expr);

            }
        };
        StmtVisitor visitor{.gen = this};
        visit(visitor,stmt.vari);
    }

    [[nodiscard]] inline string gen_prog()  {
        stringstream output;
        m_output << "global _start\n_start:\n";

        for (const auto& stmt : m_prog.stmts) {
            gen_stmt(stmt);
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

    const NodeProg m_prog;
    stringstream m_output;
    size_t m_stack_size = 0;
    unordered_map<string, vari> m_vars {};
};
