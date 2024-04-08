#include <bits/stdc++.h>
#include "def.h"
using namespace std;

typedef long long ll;
typedef unsigned long long ull;
typedef long double lld;

#define debug(x) cerr << #x <<" "; _print(x); cerr << endl;

ostream &operator << ( ostream& strm, Format tt )
{
   const string nameTT[] = {"ONE", "TWO", "THREE_FOUR"};
   return strm << nameTT[tt];
}

void _print(ll t) {cerr << t;}
void _print(int t) {cerr << t;}
void _print(string t) {cerr << t;}
void _print(char t) {cerr << t;}
void _print(lld t) {cerr << t;}
void _print(double t) {cerr << t;}
void _print(ull t) {cerr << t;}
void _print(bool t) {cerr << t;}
void _print(Opcode t) {cerr << "{ code> "; _print(t.code); cerr << " format> "; _print(t.format); cerr << "}";}
void _print(Instruction t) {cerr << "{"; _print(t.opcode); cerr << " data> "; _print(t.data); cerr << " format> "; _print(t.format); cerr << " address> "; _print(t.address); cerr << " type> "; _print(t.type); cerr << "}"; cerr << endl;}
void _print(formatOne t) {cerr << "{ opcode> "; _print(t.opcode); cerr << "}";}
void _print(formatTwo t) {cerr << "{ opcode> "; _print(t.opcode); cerr << " r1> "; _print(t.r1); cerr << " r2> "; _print(t.r2); cerr << "}";}
void _print(formatThree t) {cerr << "{ opcode> "; _print(t.opcode); cerr << " n> "; _print(t.n); cerr << " i> "; _print(t.i); cerr << " x> "; _print(t.x); cerr << " b> "; _print(t.b); cerr << " p> "; _print(t.p); cerr << " e> "; _print(t.e); cerr << " displacement> "; _print(t.displacement); cerr << "}";}
void _print(formatFour t) {cerr << "{ opcode> "; _print(t.opcode); cerr << " n> "; _print(t.n); cerr << " i> "; _print(t.i); cerr << " x> "; _print(t.x); cerr << " b> "; _print(t.b); cerr << " p> "; _print(t.p); cerr << " e> "; _print(t.e); cerr << " address> "; _print(t.address); cerr << "}";}
void _print(formatData t) {cerr << "{ data> "; _print(t.value); cerr << "}";}

template <class T, class V> void _print(pair<T,V> p) {cerr << "{"; _print(p.first); cerr << ":"; _print(p.second); cerr << "}"; cerr << endl;}
template <class T> void _print(vector <T> v) {cerr << "[ "; for (T i : v) {_print(i); cerr << ",";} cerr << "]";}
template <class T> void _print(set <T> v) {cerr << "[ "; for (T i : v) {_print(i); cerr << ",";} cerr << "]";}
template <class T> void _print(multiset <T> v) {cerr << "[ "; for (T i : v) {_print(i); cerr << ",";} cerr << "]";}
template <class T, class V> void _print(map <T, V> v) {cerr << "[ "; for (auto i : v) {_print(i); cerr << " ";} cerr << "]";}
template <class T, class V> void _print(unordered_map <T, V> v) {cerr << "[ "; for (auto i : v) {_print(i); cerr << " ";} cerr << "]";}
template <class T> void _print(stack <T> v) {cerr << "[ "; while (!v.empty()) {_print(v.top()); cerr << " "; v.pop();} cerr << "]";}
template <class T> void _print(queue <T> v) {cerr << "[ "; while (!v.empty()) {_print(v.front()); cerr << " "; v.pop();} cerr << "]";}
template <class T> void _print(priority_queue <T> v) {cerr << "[ "; while (!v.empty()) {_print(v.top()); cerr << " "; v.pop();} cerr << "]";}
template <class T> void _print(deque <T> v) {cerr << "[ "; for (T i : v) {_print(i); cerr << " ";} cerr << "]";}

