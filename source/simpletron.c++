#include "simpletron.h++"
#include <fstream>

bool Simpletron::parse(const std::vector<short> program)
{
    std::cout << "Parsing... " << std::endl;
    if (program.size() > MEM_SIZE)
    {
        std::cout << "Program too large to fit in memory." << std::endl;
        return false;
    }
    char opCode, operand;
    short instruction;

    for (char i = 0; i < (char)program.size(); i++)
    {
        instruction = program[i];
        opCode = instruction / 100;
        operand = instruction % 100;
        if (instruction == 0)
        {
            std::cout << "Invalid instruction." << std::endl;
            return false;
        }
        auto handler = handlers.find(opCode);
        if (handler == handlers.end())
        {
            std::cout << "Invalid instruction: " << instruction << std::endl;
            return false;
        }
        else if (handlers[opCode] == nullptr)
        {
            std::cout << "Instruction not implemented yet: " << (int)opCode << std::endl;
            return false;
        }
    }
    std::cout << "Program is valid." << std::endl;
    return true;
}

void Simpletron::dumpRegisters() const
{
    std::cout << "Registers:" << std::endl;
    std::cout << "  accumulator         : " << std::showpos << std::setfill('0') << std::setw(5) << std::internal << reg.accumulator << std::endl;
    std::cout << "  instructionCounter  : " << std::noshowpos << std::setfill(' ') << std::setw(5) << std::internal << reg.counter << std::endl;
    std::cout << "  instructionRegister : " << std::showpos << std::setfill('0') << std::setw(5) << std::internal << reg.instruction << std::endl;
    std::cout << "  operationCode       : " << std::noshowpos << std::setfill(' ') << std::setw(5) << std::internal << reg.opCode << std::endl;
    std::cout << "  operand             : " << std::noshowpos << std::setfill(' ') << std::setw(5) << std::internal << reg.operand << std::endl;
}

void Simpletron::dumpMemory() const
{
    std::cout << "Memory\n       0     1     2     3     4     5     6     7     8     9" << std::endl;
    for (int i = 0; i < 100; ++i)
    {
        if (i % 10 == 0)
        {
            if (i == 0)
                std::cout << " ";
            std::cout << std::noshowpos << i << " ";
            /*if (i < 100 && i != 0)
                std::cout << " "; */
            // std::cout << std::setfill(' ') << std::setw(3) << std::noshowpos << i << " ";
        }
        std::cout << std::showpos << std::setfill('0') << std::setw(5) << std::internal << memory[i] << " ";
        if ((i + 1) % 10 == 0)
            std::cout << std::endl;
    }
}

void Simpletron::dump() const
{
    dumpRegisters();
    dumpMemory();
}

void Simpletron::reset()
{
    reg.accumulator = 0;
    reg.counter = -1;
    reg.instruction = 0;
    reg.operand = 0;
    reg.opCode = 0;
}

bool Simpletron::opRead()
{
    std::cout << "> ";
    std::cin >> memory[reg.operand];
    return true;
}

bool Simpletron::opWrite()
{
    std::cout << "Output: " << std::setw(5) << std::setfill('0') << std::showpos << std::internal << memory[reg.operand] << std::endl;
    return true;
}

bool Simpletron::opLoad()
{
    reg.accumulator = memory[reg.operand];
    return true;
}

bool Simpletron::opStore()
{
    memory[reg.operand] = reg.accumulator;
    return true;
}

bool Simpletron::opAdd()
{
    reg.accumulator += memory[reg.operand];
    return true;
}

bool Simpletron::opSubtract()
{
    reg.accumulator -= memory[reg.operand];
    return true;
}

bool Simpletron::opDivide()
{
    if (memory[reg.operand] == 0)
    {
        std::cerr << "ERROR: Attempt to divide by zero at address " << reg.operand << std::endl;
        return false;
    }
    reg.accumulator /= memory[reg.operand];
    return true;
}

bool Simpletron::opMultiply()
{
    reg.accumulator *= memory[reg.operand];
    return true;
}

bool Simpletron::opBranch()
{
    reg.counter = reg.operand - 1;
    return true;
}

bool Simpletron::opBranchNeg()
{
    if (reg.accumulator < 0)
        reg.counter = reg.operand - 1;
    return true;
}

bool Simpletron::opBranchZero()
{
    if (reg.accumulator == 0)
        reg.counter = reg.operand - 1;
    return true;
}

bool Simpletron::opHalt()
{
    reg.counter = MEM_SIZE;
    return false;
}

void Simpletron::execute()
{
    reset();
    while (++reg.counter < MEM_SIZE)
    {
        // 1. fetch
        reg.instruction = memory[reg.counter];

        // 2. decode
        reg.opCode = reg.instruction / 100;
        reg.operand = reg.instruction % 100;
        bool (Simpletron::*handler)() = handlers[reg.opCode];

        // 3. execute
        if (!(this->*handler)())
            break;
    }
}

void Simpletron::load(const std::vector<short>& program)
{
    for (auto i = 0; i < (int)program.size(); i++)
    {
        memory[i] = program[i];
    }
}

void Simpletron::run(const std::vector<short>& program)
{
    std::cout << "Loading program into memory..." << std::endl;
    load(program);
    std::cout << "Running..." << std::endl;
    execute();
    std::cout << std::endl
              << "*** Program finished ***" << std::endl;
    dump();
}

void Simpletron::printInteractiveMenu() const
{
    std::cout << "*** ------------------------------------------------------------------- ***" << std::endl
              << "***                      Welcome to Simpletron!                         ***" << std::endl
              << "*** ------------------------------------------------------------------- ***" << std::endl
              << "*** Please enter your program one instruction (or data word) at a time. ***" << std::endl
              << "*** I will type the location number and a question mark(?).             ***" << std::endl
              << "*** You then type the word for that location.                           ***" << std::endl
              << "*** Type the sentinel -99999 to stop entering your program.             ***" << std::endl
              << "*** ------------------------------------------------------------------- ***" << std::endl;
}

std::vector<short> *Simpletron::readProgram()
{
    printInteractiveMenu();
    short i = 0;
    short instruction;
    std::vector<short> *program = new std::vector<short>();
    do
    {
        std::cout << "   " << std::setw(4) << std::setfill('0') << std::noshowpos << i << " ? ";
        std::cin >> instruction;
        if (instruction != -9999)
        {
            program->push_back(instruction);
            i++;
        }
    } while (instruction != -9999);
    return program;
}

std::vector<short> *Simpletron::readFromFile(std::string fileName)
{
    std::ifstream file(fileName, std::ifstream::in);
    if (!file)
    {
        std::cout << "Could not open file '" << fileName << "'" << std::endl;
        return nullptr;
    }
    std::vector<short> *program = new std::vector<short>();
    short instruction;
    while (file >> instruction)
    {
        program->push_back(instruction);
    }
    file.close();
    return program;
}