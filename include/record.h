#ifndef RECORD_H
#define RECORD_H

#include <bits/stdc++.h>
using namespace std;
#include "debug.h"
#include "def.h"
#include "utils.h"

void generateRECORDS(VariantType &v, vector<pair<string, int>> &RECORDS) {
    if (holds_alternative<formatOne>(v)) {
        string obj = intToHex(get<formatOne>(v).opcode);
        while (obj.size() != 2) {
            obj = '0' + obj;
        }
        RECORDS.push_back({obj, get<formatOne>(v).LOCCTR});
    } else if (holds_alternative<formatTwo>(v)) {
        formatTwo tmp = get<formatTwo>(v);
        string tmp1 = intToHex(tmp.opcode);
        while (tmp1.size() != 2) {
            tmp1 = '0' + tmp1;
        }
        string obj = tmp1 + intToHex(tmp.r1) + intToHex(tmp.r2);
        RECORDS.push_back({obj, tmp.LOCCTR});
    } else if (holds_alternative<formatThree>(v)) {
        formatThree tmp = get<formatThree>(v);
        string tmp1 = intToHex(tmp.opcode + (tmp.n ? 2 : 0) + (tmp.i ? 1 : 0));
        while (tmp1.size() != 2) {
            tmp1 = '0' + tmp1;
        }
        string tmp2 = intToHex((tmp.x ? 8 : 0) + (tmp.b ? 4 : 0) +
                               (tmp.p ? 2 : 0) + (tmp.e ? 1 : 0));
        string tmp3 = intToHex(tmp.displacement);
        tmp3 = getLastThreeCharacters(tmp3);
        while (tmp3.size() != 3) {
            tmp3 = '0' + tmp3;
        }
        string obj = tmp1 + tmp2 + tmp3;
        RECORDS.push_back({obj, tmp.LOCCTR});
    } else if (holds_alternative<formatFour>(v)) {
        formatFour tmp = get<formatFour>(v);
        string tmp1 = intToHex(tmp.opcode + (tmp.n ? 2 : 0) + (tmp.i ? 1 : 0));
        while (tmp1.size() != 2) {
            tmp1 = '0' + tmp1;
        }
        string tmp2 = intToHex((tmp.x ? 8 : 0) + (tmp.b ? 4 : 0) +
                               (tmp.p ? 2 : 0) + (tmp.e ? 1 : 0));

        string tmp3 = intToHex(tmp.address);
        while (tmp3.size() < 5) {
            tmp3 = '0' + tmp3;
        }
        string obj = tmp1 + tmp2 + tmp3;
        RECORDS.push_back({obj, tmp.LOCCTR});
    } else if (holds_alternative<formatData>(v)) {
        formatData tmp = get<formatData>(v);
        if (tmp.reserved) {
            RECORDS.push_back({"SKIP", tmp.LOCCTR});
            RECORDS.push_back({to_string(tmp.value), tmp.LOCCTR});
        } else {
            string obj = intToHex(tmp.value);
            if (tmp.word) {
                while (obj.size() % 6 != 0) {
                    obj = '0' + obj;
                }
                RECORDS.push_back({obj, tmp.LOCCTR});
            } else {
                if (tmp.loc_ctr) {
                    while (obj.size() < 4) {
                        obj = '0' + obj;
                    }
                    RECORDS.push_back({obj, tmp.LOCCTR});
                } else {
                    while (obj.size() % 2 != 0) {
                        obj = '0' + obj;
                    }
                    RECORDS.push_back({obj, tmp.LOCCTR});
                }
            }
        }
    }
    return;
}

string GETRECORDS(int &START_ADDRESS, string &NAME,
                  vector<pair<string, int>> &RECORDS, int &PROGRAM_LENGTH,
                  vector<string> &MRECORDS) {
    string RECORD = "";
    RECORD += "H";
    while (NAME.size() != 6) {
        NAME += ' ';
    }
    RECORD += NAME;
    string tmp = intToHex(START_ADDRESS);
    while (tmp.size() != 6) {
        tmp = '0' + tmp;
    }
    RECORD += tmp;
    tmp = intToHex(PROGRAM_LENGTH);
    while (tmp.size() != 6) {
        tmp = '0' + tmp;
    }
    RECORD += tmp;
    RECORD += "\n";
    for (ll i = 0; i < RECORDS.size(); i++) {
        ll max_text_len = 0x1E;
        ll cnt = 0;
        ll LOCCTR = RECORDS[i].second;
        string record;
        while (i < RECORDS.size() and RECORDS[i].first != "SKIP" and
               cnt + ((RECORDS[i].first.size()) / 2) <= max_text_len) {
            cnt += ((RECORDS[i].first.size()) / 2);
            record += RECORDS[i].first;
            i++;
        }
        if (i < RECORDS.size()) {
            if (RECORDS[i].first != "SKIP") {
                i--;
            }
        }
        RECORD += "T";
        tmp = intToHex(LOCCTR);
        while (tmp.size() != 6) {
            tmp = '0' + tmp;
        }
        RECORD += tmp;
        tmp = intToHex(cnt);
        while (tmp.size() != 2) {
            tmp = '0' + tmp;
        }
        RECORD += tmp;
        RECORD += record;
        RECORD += "\n";
        if (i < RECORDS.size()) {
            while (RECORDS[i].first == "SKIP" and i < RECORDS.size()) {
                i++;
                if (i + 1 < RECORDS.size()) {
                    if (RECORDS[i + 1].first == "SKIP") {
                        i++;
                    }
                }
            }
        }
    }
    for (auto it : MRECORDS) {
        RECORD += it;
        RECORD += "\n";
    }
    RECORD += "E";
    tmp = intToHex(START_ADDRESS);
    while (tmp.size() != 6) {
        tmp = '0' + tmp;
    }
    RECORD += tmp;
    RECORD += "\n";
    return RECORD;
}

#endif
