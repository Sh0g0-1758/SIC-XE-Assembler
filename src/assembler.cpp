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
// SYMBOL TABLE
map<string, int> SYMBOL_TABLE;
// UTILITY TO MAP EACH SYMBOL TO ITS CORRECT ADDRESS ACCORDING TO THE BLOCK
map<string, int> SYMBOL_BLOCK;
// UTILITY TO CHECK FOR SYMBOLS THAT NEED TO BE PLACED IN THE MODIFICATION RECORD
map<string, bool> SYMBOL_FLAG;
// UTILITY TO STORE THE LITERAL TABLE
vector<pair<string, int>> LIT_INTERMEDIATE;
// LITERAL TABLE
map<string, int> LITTAB;
// UTILITY TO MAP EACH LITERAL TO ITS CORRECT ADDRESS ACCORDING TO THE BLOCK
map<string, int> LIT_BLOCK;
// BLOCK TABLE
map<string, pair<int, int>> BLOCK_TABLE;
// UTILITY TO MAP BLOCK NUMBER TO BLOCK NAME
map<int, string> BLOCK_NAMES;
// UTILITY TO STORE LOC_CTR OF EACH BLOCK
map<int, int> BLOCK_LOCCTR;
// BLOCK RELATED VARS
int BLOCK_NUMBER = 0;
int TOTAL_BLOCKS = 0;
string CURR_BLOCK_NAME = "DEFAULT";

// UTILITY TO STORE THE OBJECT CODE
vector<VariantType> OBJCODE;
// UTILITY TO STORE THE RECORDS
vector<string> RECORDS;
// UTILITY TO STORE THE MODIFICATION RECORDS
vector<string> MRECORDS;
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
    ifstream opcodeFile("../data/opcode.info");
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
    string File_Name;
    cout << "Enter the file path> ";
    cin >> File_Name;
    ifstream programFile(File_Name);
    if (!programFile.is_open())
    {
        cerr << "Failed to open the file." << endl;
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
        SYMBOL_TABLE[it.first] += BLOCK_LOCCTR[it.second];
    }
    for (auto it : LIT_BLOCK)
    {
        LITTAB[it.first] += BLOCK_LOCCTR[it.second];
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
    cout << (SPACE);
    debug(SYMBOL_TABLE);
    debug(LITTAB);
    debug(SPACE);
    debug(INSTRUCTIONS);
    debug(SPACE);
    debug(NAME);
    debug(SPACE);
    debug(START_ADDRESS);
    debug(SPACE);
    debug(LOCCTR);
    debug(SPACE);
    debug(PROGRAM_LENGTH);
    debug(SPACE);
    LOCCTR = START_ADDRESS;
    BLOCK_NUMBER = 0;
    pass2(INSTRUCTIONS, OBJCODE, LOCCTR, LITTAB, SYMBOL_TABLE, NOBASE, MRECORDS, BLOCK_NUMBER, BLOCK_LOCCTR);
    for (auto it : OBJCODE)
    {
        printVariant(it);
    }
    cout << "DONE" << endl;
    for (auto it : OBJCODE)
    {
        generateRECORDS(it, RECORDS);
    }
    cout << "WORKING" << endl;
    string ASSEMBLER_RECORD = GETRECORDS(LOCCTR, START_ADDRESS, NAME, RECORDS, PROGRAM_LENGTH, MRECORDS);
    cout << "NOT WORKING" << endl;
    removeNewlines(ASSEMBLER_RECORD);
    File_Name = "./../Output/" + get_file_name(File_Name) + "_generated.txt";
    ofstream outputFile(File_Name);

    if (!outputFile.is_open())
    {
        cerr << "Failed to open the file." << endl;
        return 1;
    }
    outputFile << ASSEMBLER_RECORD << endl;
    outputFile.close();

    cout << "The HEADER Record has been successfully generated." << endl;

    cout << (SPACE);
    exit(0);
}
