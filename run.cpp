#include<bits/stdc++.h>
#include <variant>
#include "debug.h"
using VariantType = variant<formatOne,formatTwo,formatThree,formatFour,formatData>;

map<string,Opcode> OPTAB;
vector<Instruction> INSTRUCTIONS;
map<string,int> SYMBOL_TABLE;
vector<pair<string,pair<string,int>>> LIT_INTERMEDIATE;
map<string,int> LITTAB;

vector<VariantType> OBJCODE;
vector<string> RECORDS;
vector<string> MRECORDS;
string NAME;
int START_ADDRESS;
int LOCCTR;
int PROGRAM_LENGTH;
string BASE;

#define data_directive() {                               \
        instruction.address = LOCCTR;                    \
        instruction.data = tokens[2];                    \
        instruction.format = Format::DATA;               \
        instruction.opcode.code = 0;                     \
        instruction.opcode.format = Format::DATA;        \
}

void printVariant(const VariantType& v) {
    if (holds_alternative<formatOne>(v)) {
        debug(get<formatOne>(v))
    } else if (holds_alternative<formatTwo>(v)) {
        debug(get<formatTwo>(v))
    } else if (holds_alternative<formatThree>(v)) {
        debug(get<formatThree>(v))
    } else if(holds_alternative<formatFour>(v)) {
        debug(get<formatFour>(v))
    } else if(holds_alternative<formatData>(v)) {
        debug(get<formatData>(v))
    }
}

static const unordered_map<string, Register> registerMap = {
    {"A", Register::A},
    {"X", Register::X},
    {"L", Register::L},
    {"B", Register::B},
    {"S", Register::S},
    {"T", Register::T},
    {"F", Register::F},
    {"PC", Register::PC},
    {"SW", Register::SW}
};

string trim(const string& str) {
    string result = str;
    result.erase(result.begin(), find_if(result.begin(), result.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
    result.erase(find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), result.end());
    return result;
}

string intToHex(int value) {
    stringstream stream;
    stream << hex << value;
    return stream.str();
}

int hexToInt(const string& hexString) {
    stringstream stream(hexString);
    int value;
    stream >> hex >> value;
    return value;
}

pair<string, string> get_registers(const string& inputString) {
    istringstream iss(inputString);
    string token1, token2;
    getline(iss, token1, ',');
    getline(iss, token2);
    return {token1, token2};
}

string charToHex(char c) {
    stringstream ss;
    ss << hex << setw(2) << setfill('0') << static_cast<int>(c);
    return ss.str();
}

string stringToHex(const string& input) {
    string hexString;
    for (char c : input) {
        hexString += charToHex(c);
    }
    return hexString;
}

void generateRECORD(const VariantType& v) {
    if (holds_alternative<formatOne>(v)) {
        string obj = intToHex(get<formatOne>(v).opcode);
        while(obj.size() != 2) {
            obj = '0' + obj;
        }
        RECORDS.push_back(obj);
    } else if (holds_alternative<formatTwo>(v)) {
        formatTwo tmp = get<formatTwo>(v);
        string tmp1 = intToHex(tmp.opcode);
        while(tmp1.size() != 2) {
            tmp1 = '0' + tmp1;
        }
        string obj = tmp1 + intToHex(tmp.r1) + intToHex(tmp.r2);
        RECORDS.push_back(obj);
    } else if (holds_alternative<formatThree>(v)) {
        formatThree tmp = get<formatThree>(v);
        string tmp1 = intToHex(tmp.opcode + (tmp.n ? 2 : 0) + (tmp.i ? 1 : 0));
        while(tmp1.size() != 2) {
            tmp1 = '0' + tmp1;
        }
        string tmp2 = intToHex((tmp.x ? 8 : 0) + (tmp.b ? 4 : 0) + (tmp.p ? 2 : 0) + (tmp.e ? 1 : 0));
        string tmp3 = intToHex(tmp.displacement);
        while(tmp3.size() != 3) {
            tmp3 = '0' + tmp3;
        }
        string obj = tmp1 + tmp2 + tmp3;
        RECORDS.push_back(obj);
    } else if(holds_alternative<formatFour>(v)) {
        formatFour tmp = get<formatFour>(v);
        string tmp1 = intToHex(tmp.opcode + (tmp.n ? 2 : 0) + (tmp.i ? 1 : 0));
        while(tmp1.size() != 2) {
            tmp1 = '0' + tmp1;
        }
        string tmp2 = intToHex((tmp.x ? 8 : 0) + (tmp.b ? 4 : 0) + (tmp.p ? 2 : 0) + (tmp.e ? 1 : 0));
        string tmp3 = intToHex(tmp.address);
        while(tmp3.size() != 5) {
            tmp3 = '0' + tmp3;
        }
        string obj = tmp1 + tmp2 + tmp3;
        RECORDS.push_back(obj);
    } else if(holds_alternative<formatData>(v)) {
        formatData tmp = get<formatData>(v);
        if(tmp.reserved) {
            RECORDS.push_back("SKIP");
            RECORDS.push_back(to_string(tmp.value));
        } else {
            string obj = intToHex(tmp.value);
            if(tmp.word) {
                while(obj.size()%6 != 0) {
                    obj = '0' + obj;
                }
                RECORDS.push_back(obj);
            } else {
                while(obj.size()%2 != 0) {
                    obj = '0' + obj;
                }
                RECORDS.push_back(obj);
            }
        }
    }
}

void printRECORDS() {
    debug(RECORDS)
    cout << SPACE << endl;
    LOCCTR = START_ADDRESS;
    cout << "H";
    while(NAME.size() != 6) {
        NAME += ' ';
    }
    cout << NAME;
    string tmp = intToHex(START_ADDRESS);
    while(tmp.size() != 6) {
        tmp = '0' + tmp;
    }
    cout << tmp;
    tmp = intToHex(PROGRAM_LENGTH);
    while(tmp.size() != 6) {
        tmp = '0' + tmp;
    }
    cout << tmp << endl;
    for(ll i = 0; i < RECORDS.size(); i++) {
        ll max_text_len = 0x1E;
        ll cnt = 0;
        string record;
        while(i < RECORDS.size() and RECORDS[i] != "SKIP" and cnt + ((RECORDS[i].size())/2) <= max_text_len) {
            cnt += ((RECORDS[i].size()) / 2);
            record += RECORDS[i];
            i++;
        }
        cout << "T";
        tmp = intToHex(LOCCTR);
        while(tmp.size() != 6) {
            tmp = '0' + tmp;
        }
        cout << tmp;
        tmp = intToHex(cnt);
        while(tmp.size() != 2) {
            tmp = '0' + tmp;
        }
        cout << tmp;
        cout << record;
        LOCCTR += cnt;
        cout << endl;
        if(RECORDS[i] == "SKIP") {
            i++;
            LOCCTR += stoi(RECORDS[i]);
        }
    }
    cout << "E";
    tmp = intToHex(START_ADDRESS);
    while(tmp.size() != 6) {
        tmp = '0' + tmp;
    }
    cout << tmp << endl;
    for(auto it : MRECORDS) {
        cout << it << endl;
    }
}

void pre_process(string line) {
    Opcode opcode;
    vector<string> tokens;
    istringstream iss(line);
    string token;
    while (getline(iss, token, '|')) {
        tokens.push_back(trim(token));
    }

    if (tokens.size() == 3) {
        opcode.code = hexToInt(tokens[2]);
        if (tokens[1] == "1") {
            opcode.format = Format::ONE;
        } else if (tokens[1] == "2") {
            opcode.format = Format::TWO;
        } else if (tokens[1] == "3/4") {
            opcode.format = Format::THREE;
        } else {
            cerr << "Invalid format value." << endl;
            exit(0);
        }
        OPTAB[tokens[0]] = opcode;
    } else {
        cerr << "Insufficient data values for opcode." << endl;
        exit(0);
    }
}

Register stringToRegister(const string& str) {
    auto it = registerMap.find(str);
    if (it != registerMap.end()) {
        return it->second;
    } else {
        cerr << "Invalid register string" << endl;
        exit(0);
    }
}

void program_initialisation(string line) {
    vector<string> tokens;
    istringstream iss(line);
    string token;
    while(iss >> token) {
        tokens.push_back(trim(token));
    }
    if(tokens[1] != "START") {
        cerr << "Invalid program start." << endl;
        exit(0);
    }
    NAME = tokens[0];
    START_ADDRESS = hexToInt(tokens[2]);
    LOCCTR = START_ADDRESS;
}

void pass1(string line) {
    Instruction instruction;
    vector<string> tokens;
    istringstream iss(line);
    string token;
    while(iss >> token) {
        tokens.push_back(trim(token));
    }
    if(tokens[0] == "LTORG") {
        for(auto it : LIT_INTERMEDIATE) {
            LITTAB[it.first] = LOCCTR;
            instruction.address = LOCCTR;
            instruction.data = it.second.first;
            instruction.format = Format::DATA;
            instruction.opcode.code = 0;
            instruction.opcode.format = Format::DATA;
            instruction.type = DataType::BYTE;
            LOCCTR += it.second.second;
            INSTRUCTIONS.push_back(instruction);
        }
        LIT_INTERMEDIATE.clear();
        return;
    }
    if(tokens[0] == "RSUB") {
        tokens.insert(tokens.begin(), "");
        tokens.push_back("");
    }
    if(tokens[1] == "RSUB") {
        tokens.push_back("");
    }
    if(tokens.size() == 2) {
        tokens.insert(tokens.begin(), "");
    }
    if(tokens[1] == "END") {
        if(SYMBOL_TABLE.find(tokens[2]) == SYMBOL_TABLE.end() or SYMBOL_TABLE[tokens[2]] != START_ADDRESS) {
            cerr << "Invalid program end." << endl;
            exit(0);
        }
        return;
    }
    if(!tokens[0].empty()) {
        if(SYMBOL_TABLE.find(tokens[0]) != SYMBOL_TABLE.end()) {
            cerr << "Duplicate symbol found." << endl;
            exit(0);
        }
        SYMBOL_TABLE[tokens[0]] = LOCCTR;
    }
    if(tokens[1] == "BASE") {
        BASE = tokens[2];
        return;
    }
    if(tokens[1] == "WORD") {
        data_directive();
        instruction.type = DataType::WORD;
        LOCCTR += 3;
        INSTRUCTIONS.push_back(instruction);
        return;
    } else if(tokens[1] == "RESW") {
        data_directive();
        instruction.type = DataType::RESW;
        LOCCTR += 3 * stoi(tokens[2]);
        INSTRUCTIONS.push_back(instruction);
        return;
    } else if(tokens[1] == "RESB") {
        data_directive();
        instruction.type = DataType::RESB;
        LOCCTR += stoi(tokens[2]);
        INSTRUCTIONS.push_back(instruction);
        return;
    } else if(tokens[1] == "BYTE") {
        data_directive();
        instruction.type = DataType::BYTE;
        if(tokens[2][0] == 'X') {
            LOCCTR += (tokens[2].length() - 3) / 2;
        } else if(tokens[2][0] == 'C') {
            LOCCTR += tokens[2].length() - 3;
        } else {
            cerr << "Invalid byte declaration." << endl;
            exit(0);
        }
        
        INSTRUCTIONS.push_back(instruction);
        return;
    } else if (tokens[1][0] == '+') {
        if(OPTAB.find(tokens[1].substr(1)) == OPTAB.end()) {
            cerr << "Invalid opcode." << endl;
            exit(0);
        }
    } else if (OPTAB.find(tokens[1]) == OPTAB.end()) {
        cerr << "Invalid opcode." << endl;
        exit(0);
    }
    if(tokens[1][0] == '+') {
        instruction.format = Format::FOUR;
        tokens[1] = tokens[1].substr(1);
    } else {
        instruction.format = OPTAB[tokens[1]].format;
    }
    instruction.opcode = OPTAB[tokens[1]];
    instruction.address = LOCCTR;
    // Checking for literals
    if(tokens[2][0] == '=') {
        if(LITTAB.find(tokens[2]) == LITTAB.end()) {
            string tmp;
            if(tokens[2][1] == 'C') {
                tmp = tokens[2].substr(1, tokens[2].length() - 1);
                LIT_INTERMEDIATE.push_back({tmp, {tmp, tmp.size()}});
                instruction.data = tmp;
            } else if(tokens[2][1] == 'X') {
                tmp = tokens[2].substr(1, tokens[2].length() - 1);
                LIT_INTERMEDIATE.push_back({tmp, {tmp, tmp.size() / 2}});
                instruction.data = tmp;
            } else {
                cerr << "Invalid literal declaration." << endl;
                exit(0);
            }
        }
    } else {
        instruction.data = tokens[2];
    }
    if (instruction.format == Format::ONE) {
        LOCCTR += 1;
    } else if (instruction.format == Format::TWO) {
        LOCCTR += 2;
    } else if (instruction.format == Format::THREE) {
        LOCCTR += 3;
    } else if (instruction.format == Format::FOUR) {
        LOCCTR += 4;
    }
    INSTRUCTIONS.push_back(instruction);
}

void pass2() {
    for(auto it : INSTRUCTIONS) {
        if(it.format == Format::ONE) {
            LOCCTR+=1;
            formatOne obj;
            obj.opcode = it.opcode.code;
            OBJCODE.push_back(obj);
        } else if(it.format == Format::TWO) {
            LOCCTR+=2;
            auto REG = get_registers(it.data);
            formatTwo obj;
            obj.opcode = it.opcode.code;
            obj.r1 = stringToRegister(REG.first);
            obj.r2 = stringToRegister(REG.second);
            OBJCODE.push_back(obj);
        } else if (it.format == Format::THREE) {
            LOCCTR+=3;
            formatThree obj;
            if(it.opcode.code == 0x4C) {
                obj.n = true;
                obj.i = true;
                obj.x = false;
                obj.b = false;
                obj.p = false;
                obj.e = false;
                obj.opcode = it.opcode.code;
                obj.displacement = 0;
                OBJCODE.push_back(obj);
                continue;
            }
            obj.e = false;
            obj.opcode = it.opcode.code;
            if(it.data[0] == '#') {
                obj.n = false;
                obj.i = true;
            } else if(it.data[0] == '@') {
                obj.n = true;
                obj.i = false;
            } else {
                obj.n = true;
                obj.i = true;
            }
            size_t found = it.data.find(",X");
            if(found != string::npos) {
                obj.x = true;
            } else {
                obj.x = false;
            }
            if(obj.i and !obj.n) {
                if(SYMBOL_TABLE.find(it.data) != SYMBOL_TABLE.end()) {
                    obj.displacement = SYMBOL_TABLE[it.data];
                } else {
                    obj.displacement = stoi(it.data);
                }
            } else if(SYMBOL_TABLE[it.data] - LOCCTR <= 0xFFF and SYMBOL_TABLE[it.data] - LOCCTR >= 0) {
                obj.p = true;
                obj.b = false;
                obj.displacement = SYMBOL_TABLE[it.data] - LOCCTR;
            } else if(SYMBOL_TABLE[it.data] - SYMBOL_TABLE["BASE"] < 4095 and SYMBOL_TABLE[it.data] - SYMBOL_TABLE["BASE"] > 0) {
                obj.p = false;
                obj.b = true;
                obj.displacement = SYMBOL_TABLE[it.data] - SYMBOL_TABLE["BASE"];
            } else if(LITTAB[it.data] - LOCCTR <= 0xFF and LITTAB[it.data] - LOCCTR >= 0) {
                obj.p = true;
                obj.b = false;
                obj.displacement = LITTAB[it.data] - LOCCTR;
            } else if(LITTAB[it.data] - SYMBOL_TABLE["BASE"] < 4095 and LITTAB[it.data] - SYMBOL_TABLE["BASE"] > 0) {
                obj.p = false;
                obj.b = true;
                obj.displacement = LITTAB[it.data] - SYMBOL_TABLE["BASE"];
            } else {
                debug(it.data)
                debug(LITTAB[it.data])
                debug(LOCCTR)
                cout << "Displacement can't be reached from PC or BASE." << endl;
                exit(0);
            }
            OBJCODE.push_back(obj);
        } else if (it.format == Format::FOUR) {
            string addr = intToHex(LOCCTR + 1);
            while(addr.size() != 6) {
                addr = '0' + addr;
            }
            addr = 'M' + addr + "05";
            MRECORDS.push_back(addr);
            LOCCTR+=4;
            formatFour obj;
            obj.opcode = it.opcode.code;
            obj.e = true;
            if(it.data[0] == '#') {
                it.data = it.data.substr(1);
                obj.n = false;
                obj.i = true;
            } else if(it.data[0] == '@') {
                it.data = it.data.substr(1);
                obj.n = true;
                obj.i = false;
            } else {
                obj.n = true;
                obj.i = true;
            }
            size_t found = it.data.find(",X");
            if(found != string::npos) {
                it.data = it.data.substr(0, found);
                obj.x = true;
            } else {
                obj.x = false;
            }
            obj.b = false;
            obj.p = false;
            if(obj.i and !obj.n) {
                if(SYMBOL_TABLE.find(it.data) != SYMBOL_TABLE.end()) {
                    obj.address = SYMBOL_TABLE[it.data];
                } else {
                    obj.address = stoi(it.data);
                }
            } else if(SYMBOL_TABLE[it.data] > 0 and SYMBOL_TABLE[it.data] < 0xFFFFF) {
                obj.address = SYMBOL_TABLE[it.data];
            } else {
                cout << "Invalid Symbol or Address out of range." << endl;
                exit(0);
            }
            OBJCODE.push_back(obj);
        } else if (it.format == Format::DATA) {
            if (it.type == DataType::WORD) {
                LOCCTR+=3;
                formatData obj;
                obj.value = stoi(it.data);
                obj.reserved = false;
                obj.word = true;
                OBJCODE.push_back(obj);
            } else if (it.type == DataType::BYTE) {
                formatData obj;
                if(it.data[0] == 'X') {
                    LOCCTR += ((it.data.length() - 3) / 2);
                    obj.value = hexToInt(it.data.substr(2, it.data.length() - 3));
                } else if (it.data[0] == 'C') {
                    LOCCTR += (it.data.length() - 3);
                    string temp = it.data.substr(2, it.data.length() - 3);
                    obj.value = hexToInt(stringToHex(temp));
                }
                obj.reserved = false;
                obj.word = false;
                OBJCODE.push_back(obj);
            } else if (it.type == DataType::RESW) {
                LOCCTR += 3 * stoi(it.data);
                formatData obj;
                obj.value = 3 * stoi(it.data);
                obj.reserved = true;
                obj.word = true;
                OBJCODE.push_back(obj);
            } else if (it.type == DataType::RESB) {
                LOCCTR += stoi(it.data);
                formatData obj;
                obj.value = stoi(it.data);
                obj.reserved = true;
                obj.word = false;
                OBJCODE.push_back(obj);
            }
        }
    }
}

int main() {
    // Processing the opcodes
    ifstream opcodeFile("opcode.info");
    if (!opcodeFile.is_open()) {
        cerr << "Failed to open the file." << endl;
        exit(0);
    }
    string line;
    getline(opcodeFile, line); // Skipping the header
    while (getline(opcodeFile, line)) {
        pre_process(line);
    }
    opcodeFile.close();
    // debug(OPTAB);

    // First pass of the assembler
    ifstream programFile("input.sic");
    if(!programFile.is_open()) {
        cerr << "Failed to open the file." << endl;
        exit(0);
    }
    getline(programFile, line); // Starting address and program name
    program_initialisation(line);
    while(getline(programFile, line)) {
        pass1(line);
    }
    if(LIT_INTERMEDIATE.size() != 0) {
        Instruction instruction;
        for(auto it : LIT_INTERMEDIATE) {
            LITTAB[it.first] = LOCCTR;
            instruction.address = LOCCTR;
            instruction.data = it.second.first;
            instruction.format = Format::DATA;
            instruction.opcode.code = 0;
            instruction.opcode.format = Format::DATA;
            instruction.type = DataType::BYTE;
            LOCCTR += it.second.second;
            INSTRUCTIONS.push_back(instruction);
        }
        LIT_INTERMEDIATE.clear();
    }
    programFile.close();
    PROGRAM_LENGTH = LOCCTR - START_ADDRESS;
    SYMBOL_TABLE["BASE"] = SYMBOL_TABLE[BASE];
    cout << (SPACE);
    debug(SYMBOL_TABLE)
    debug(LITTAB)
    debug(SPACE)
    debug(INSTRUCTIONS)
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
    pass2();
    for(auto it : OBJCODE) {
        printVariant(it);
    }
    for(auto it : OBJCODE) {
        generateRECORD(it);
    }
    printRECORDS();
    cout << (SPACE);
    exit(0);
}