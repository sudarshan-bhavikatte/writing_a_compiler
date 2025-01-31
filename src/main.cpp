#include <iostream>
#include <sstream>
#include <fstream>
#include <optional>
#include <vector>

#include "./generation.hpp"
#include "./parser.hpp"
#include "./tokenization.hpp"

using namespace std;


int main(int argc, char *argv[]){
    if(argc != 2){
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return EXIT_FAILURE;
    }
    string contents;
    {
        stringstream contents_stream;
        fstream input(argv[1], ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(contents);
    vector<Token> tokens = tokenizer.tokenize();
    Parser parser(move(tokens));
    optional<NodeProg> prog = parser.parse_prog();

    if(!prog.has_value()) {
        cerr << "Error parsing file" << endl;
        exit(EXIT_FAILURE);
    }

    {
        Generator generator(prog.value());
        fstream file("out.asm", ios::out);
        file << generator.gen_prog();
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");

    return EXIT_SUCCESS;
}