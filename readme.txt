Compiler for a TinyGo language

The full thesis is not included

To compile it start from the root dir of this project

'''
mkdir build
cd build
cmake ..
make
'''

run compiler by command './compiler'

it usage : usage: ni-gen [options] file

           Options:
               -o <file>        Place the output into <file>.
               -asm [<file>]    Output generated assembly. If file not provided : into a console.
               -ir [<file>]     Output generated IR code. If file not provided : into a console.

Grammar of the TinyGo located in /doc/language.md

Video of usage located in doc/ folder