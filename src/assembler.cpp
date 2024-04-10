#include <bits/stdc++.h>
#include <variant>
#include "debug.h"
#include "def.h"
#include "record.h"
#include "initialize.h"
#include "utils.h"
#include "pass1.h"
#include "pass2.h"

map<string, Opcode> OPTAB;
vector<Instruction> INSTRUCTIONS;
map<string, int> SYMBOL_TABLE;
map<string, bool> SYMBOL_FLAG;
vector<pair<string, int>> LIT_INTERMEDIATE;
map<string, int> LITTAB;

vector<VariantType> OBJCODE;
vector<string> RECORDS;
vector<string> MRECORDS;
string NAME;
int START_ADDRESS;
int LOCCTR;
int PROGRAM_LENGTH;
bool NOBASE = false;
bool ORG = false;
int prevLOCCTR;
string BASE;

int main()
{
    // Processing the opcodes
    ifstream opcodeFile("../opcode.info");
    if (!opcodeFile.is_open())
    {
        cerr << "Failed to open the file." << endl;
        exit(0);
    }
    string line;
    getline(opcodeFile, line); // Skipping the header
    while (getline(opcodeFile, line))
    {
        pre_process(line, OPTAB);
    }
    opcodeFile.close();
    // debug(OPTAB);

    // First pass of the assembler
    ifstream programFile("../input.sic");
    if (!programFile.is_open())
    {
        cerr << "Failed to open the file." << endl;
        exit(0);
    }
    getline(programFile, line); // Starting address and program name
    program_initialisation(line, NAME, START_ADDRESS, LOCCTR);
    while (getline(programFile, line))
    {
        pass1(line, NOBASE, LIT_INTERMEDIATE, LITTAB, LOCCTR, INSTRUCTIONS, SYMBOL_TABLE, SYMBOL_FLAG, START_ADDRESS, ORG, MRECORDS, OPTAB, BASE, prevLOCCTR);
    }
    if (LIT_INTERMEDIATE.size() != 0)
    {
        Instruction instruction;
        for (auto it : LIT_INTERMEDIATE)
        {
            LITTAB[it.first] = LOCCTR;
            instruction.address = LOCCTR;
            instruction.data = it.first;
            instruction.format = Format::DATA;
            instruction.opcode.code = 0;
            instruction.opcode.format = Format::DATA;
            instruction.type = DataType::BYTE;
            LOCCTR += it.second;
            INSTRUCTIONS.push_back(instruction);
        }
        LIT_INTERMEDIATE.clear();
    }
    sort(INSTRUCTIONS.begin(), INSTRUCTIONS.end(), custom_sort);
    programFile.close();
    PROGRAM_LENGTH = LOCCTR - START_ADDRESS;
    if (BASE[0] == '*')
    {
        BASE = BASE.substr(1);
        SYMBOL_TABLE["BASE"] = hexToInt(BASE);
    }
    else
    {
        SYMBOL_TABLE["BASE"] = SYMBOL_TABLE[BASE];
    }
    // cout << (SPACE);
    // debug(SYMBOL_TABLE);
    // debug(LITTAB);
    // debug(SPACE);
    // debug(INSTRUCTIONS);
    // debug(SPACE)
    // debug(NAME)
    // debug(SPACE)
    // debug(START_ADDRESS)
    // debug(SPACE)
    // debug(LOCCTR)
    // debug(SPACE)
    // debug(PROGRAM_LENGTH)
    // debug(SPACE)
    LOCCTR = START_ADDRESS;
    pass2(INSTRUCTIONS, OBJCODE, LOCCTR, LITTAB, SYMBOL_TABLE, NOBASE, MRECORDS);
    // for (auto it : OBJCODE)
    // {
    //     printVariant(it);
    // }
    for (auto it : OBJCODE)
    {
        generateRECORDS(it, RECORDS);
    }
    printRECORDS(LOCCTR, START_ADDRESS, NAME, RECORDS, PROGRAM_LENGTH, MRECORDS);
    // cout << (SPACE);
    exit(0);
}
