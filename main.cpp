#include <iostream>
#include <cstring>

#include "parser.h"

const char* usage = R"(
usage: ni-gen [options] file

Options:
    -o <file>        Place the output into <file>.
    -asm [<file>]    Output generated assembly. If file not provided : into a console.
    -ir [<file>]     Output generated IR code. If file not provided : into a console.
)";

void incorrect_args(){
    std::cout << usage << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {

    bool asmPrint = false, irPrint = false;

    std::string inputF,outputF,asmF,irF;

    // command line arguments parse
    if (argc < 2)
        incorrect_args();


    for (auto i = 1; i < argc - 1; ++i) {
        if (strcmp(argv[i],"-o") == 0) {
            i++;
            if (i >= argc - 1)
                incorrect_args();
            outputF = argv[i];
        } else if (strcmp(argv[i],"-asm") == 0) {
            asmPrint = true;
            if (i + 2 < argc && argv[i+1][0] != '-')
                asmF = argv[i++];
        } else if (strcmp(argv[i],"-ir") == 0) {
            irPrint = true;
            if (i + 2 < argc && argv[i+1][0] != '-')
                irF = argv[i++];
        } else {
            std::cout << usage << std::endl;
            return EXIT_FAILURE;
        }
    }

    inputF = argv[argc - 1];




    try {
        Parser p(inputF);
        auto root = p.parse();
        if (!root)
            return 0;

        AST::Context ctx;
        root->checker(ctx);

        auto IRctx = ctx.createIRContext();
        auto IR = root->generateIR(IRctx);

        if (irPrint) {
            if (irF.empty())
                IR->print(std::cout);
            else {
                std::ofstream file(irF,std::ofstream::out);
                IR->print(file);
            }
        }

        auto T86ctx = IRctx.createT86Context();

        IR->generateT86(T86ctx);

        if (asmPrint) {
            if (asmF.empty())
                T86ctx.print(std::cout);
            else {
                std::ofstream file(asmF,std::ofstream::out);
                T86ctx.print(file);
            }
        }

        if (!outputF.empty()) {
            std::ofstream file(outputF, std::ofstream::out);
            T86ctx.print(file);
        }


    } catch (std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
