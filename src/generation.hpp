#pragma once
#include "parser.hpp"

using namespace std;
class Generator {
public:
    inline Generator(NodeExit root)
        :m_root(move(root))
    {
    }

    [[nodiscard]] inline string generate() const {
        stringstream output;
        output << "global _start\n_start:\n";
        output << "    mov rax, 60\n";
        output << "    mov rdi, " << m_root.expr.INT_LIT.value.value() << "\n";
        output << "    syscall";
        return output.str();
    }
private:
    const NodeExit m_root;

};
