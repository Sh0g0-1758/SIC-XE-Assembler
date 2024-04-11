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
map<string, int> SYMBOL_TABLE;              // Name of the symbol -> Address
map<string, int> SYMBOL_BLOCK;              // Name of the symbol -> Block Number
map<string, bool> SYMBOL_FLAG;              // Name of the symbol -> Need to be modified or not
vector<pair<string, int>> LIT_INTERMEDIATE; // {Literal, Length}
map<string, int> LITTAB;                    // Literal -> Address
map<string, int> LIT_BLOCK;                 // Literal -> Block Number
map<string, pair<int, int>> BLOCK_TABLE;    // {Block Name, {Block Number, Length}}
map<int, string> BLOCK_NAMES;               // {Block Number, Block Name}
map<int, int> BLOCK_LOCCTR;                 // {Block Number, LOCCTR}

vector<VariantType> OBJCODE; // UTILITY TO STORE THE OBJECT CODE

vector<pair<string, int>> RECORDS; // UTILITY TO STORE THE RECORDS

vector<string> MRECORDS; // UTILITY TO STORE THE MODIFICATION RECORDS

// BLOCK RELATED VARS
int BLOCK_NUMBER = 0;
int TOTAL_BLOCKS = 0;
string CURR_BLOCK_NAME = "DEFAULT";

// PROGRAM RELATED GLOBAL VARS
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
    ifstream opcodeFile("../tools/data/opcode.info");
    if (!opcodeFile.is_open())
    {
        cout << red << "Failed to open the file." << def << endl;
        exit(0);
    }
    string line;
    getline(opcodeFile, line); // Skipping the header
    while (getline(opcodeFile, line))
    {
        pre_process(line, OPTAB);
    }
    opcodeFile.close();

    // First pass of the assembler
    string File_Name;
    cout << cyan << "Enter the file path> " << def;
    cin >> File_Name;
    ifstream programFile(File_Name);
    if (!programFile.is_open())
    {
        cout << red << "Failed to open the file." << def << endl;
        exit(0);
    }
    getline(programFile, line); // Starting address and program name
    program_initialisation(line, NAME, START_ADDRESS, LOCCTR);
    BLOCK_TABLE["DEFAULT"] = {0, 0};
    BLOCK_NAMES[0] = "DEFAULT";
    while (getline(programFile, line))
    {
        handle_comments(line);
        remove_whitespaces(line);
        if (line.empty())
        {
            continue;
        }
        pass1(line, NOBASE, LIT_INTERMEDIATE, LITTAB, LOCCTR, INSTRUCTIONS, SYMBOL_TABLE, SYMBOL_FLAG, START_ADDRESS, ORG, MRECORDS, OPTAB, BASE, prevLOCCTR, BLOCK_TABLE, BLOCK_NUMBER, CURR_BLOCK_NAME, TOTAL_BLOCKS, BLOCK_NAMES, SYMBOL_BLOCK, LIT_BLOCK);
    }
    BLOCK_TABLE[CURR_BLOCK_NAME].second = LOCCTR;
    if (LIT_INTERMEDIATE.size() != 0)
    {
        Instruction instruction;
        for (auto it : LIT_INTERMEDIATE)
        {
            LITTAB[it.first] = LOCCTR;
            LIT_BLOCK[it.first] = BLOCK_NUMBER;
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
    int prev_block_size = 0;
    int prev_block_locctr = 0;
    for (auto it : BLOCK_NAMES)
    {
        if (it.first == 0)
        {
            BLOCK_LOCCTR[0] = 0;
            prev_block_size = BLOCK_TABLE[it.second].second;
            prev_block_locctr = 0;
        }
        else
        {
            BLOCK_LOCCTR[it.first] = prev_block_locctr + prev_block_size;
            prev_block_locctr = BLOCK_LOCCTR[it.first];
            prev_block_size = BLOCK_TABLE[it.second].second;
        }
    }
    for (auto it : SYMBOL_BLOCK)
    {
        if (it.second == -1)
        {
            continue;
        }
        SYMBOL_TABLE[it.first] += BLOCK_LOCCTR[it.second];
    }
    for (auto it : LIT_BLOCK)
    {
        LITTAB[it.first] += BLOCK_LOCCTR[it.second];
    }
    programFile.close();
    for (auto it : BLOCK_TABLE)
    {
        PROGRAM_LENGTH += it.second.second;
    }
    if (BASE[0] == '*')
    {
        BASE = BASE.substr(1);
        SYMBOL_TABLE["BASE"] = hexToInt(BASE);
    }
    else
    {
        SYMBOL_TABLE["BASE"] = SYMBOL_TABLE[BASE];
    }
    LOCCTR = START_ADDRESS;
    BLOCK_NUMBER = 0;
    pass2(INSTRUCTIONS, OBJCODE, LOCCTR, LITTAB, SYMBOL_TABLE, NOBASE, MRECORDS, BLOCK_NUMBER, BLOCK_LOCCTR, SYMBOL_FLAG);
    for (auto it : OBJCODE)
    {
        generateRECORDS(it, RECORDS);
    }
    string ASSEMBLER_RECORD = GETRECORDS(START_ADDRESS, NAME, RECORDS, PROGRAM_LENGTH, MRECORDS);
    removeNewlines(ASSEMBLER_RECORD);
    File_Name = "./../Output/" + get_file_name(File_Name) + "_generated.txt";
    ofstream outputFile(File_Name);

    if (!outputFile.is_open())
    {
        cout << red << "Failed to open the file." << def << endl;
        return 1;
    }
    outputFile << ASSEMBLER_RECORD << endl;
    outputFile.close();

    cout << green << "The HEADER Record has been successfully generated." << def << endl;
    exit(0);
}
