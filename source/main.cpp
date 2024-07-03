#include "simpletron.h++"
#include <fstream>

int main(int argc, char *argv[])
{
    Simpletron simpletron;
    std::vector<short> *program;

    if (argc <= 1)
    {
        program = simpletron.readProgram();
    }
    else
    {
        program = simpletron.readFromFile(argv[1]);
    }
    simpletron.run(*program);
    delete program;
}