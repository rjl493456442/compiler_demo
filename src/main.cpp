#include <iostream>
#include "LexicalAnalyser.h"
#include "SyntaxAnaylyser.h"
#include <stack>
using namespace std;
LexicalAnalyser * lex;
char  VT[10] = {
        'p','i','(',')','+','-','*','/','=','#'
};
char VN[6] = {
        'S','E','M','T','N','F'
};
const char * rule[12] = {
        "p=E","TM","+TM","-TM","-1","FN","*FN","/FN","-1","p","i","(E)"
};
int FindVT(char s){
    for(int i = 0; i < 10;++i){
        if (VT[i] == s) return i;
    }
    return -1;
}
int FindVN(char s){
    for(int i = 0; i < 6;++i){
        if (VN[i] == s) return i;
    }
    return -1;
}
int rule_index[6][10] = {
        {0,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        {1,1,1,-1,-1,-1,-1,-1,1,-1},
        {-1,-1,-1,4,2,3,-1,-1,-1,4},
        {5,5,5,-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,8,8,8,6,7,-1,8},
        {9,10,11,-1,-1,-1,-1,-1,-1,-1},
};
char Convert(int key){
    if(key == 0)return 'p';
    else if(key >= 35 && key <= 39)return 'i';
    else if(key == 41)return '#';
    else if(key == 50)return '+';
    else if(key == 51)return '-';
    else if(key == 52)return '*';
    else if(key == 54)return '/';
    else if(key == 69)return '=';
    return '\0';
}
bool SyntaxAnalyser(){
    int status = SYNTAX_BEGIN;
    stack<Tuple> input_string;
    stack<char> analyser;
    analyser.push('#');
    analyser.push('S');
    Tuple tmp;
    tmp.key = 41;
    tmp.value = "#";
    input_string.push(tmp);
    char a;
    Tuple t;
    char x;
    char r[5];
    long i ;
    for( i = LexOutput.size() - 1; i >= 0; --i){
        input_string.push(LexOutput.at(i));
    }
    while(1){
        switch (status){
            case SYNTAX_BEGIN:
                a = analyser.top();
                analyser.pop();
                t =  input_string.top();
                if(t.key == 40) x = t.value[0];
                else
                    x = Convert(t.key);
                if(a == '#')return  true;
                if(a == x) {
                    status = SYNTAX_BEGIN;
                    input_string.pop();
                    break;
                }
                if(FindVN(a) == -1 || FindVT(x) == -1) {
                    status = SYNTAX_ERROR;
                    break;
                }
                else{

                    int row = FindVN(a);
                    int col = FindVT(x);
                   // cout << "r" << row << "c" <<col <<endl;
                    int pos = rule_index[row][col];
                    if (pos == -1){
                        status = SYNTAX_ERROR;
                        break;
                    }

                    strcpy(r,rule[pos]);
                    if (strcmp(r,"-1") == 0){
                        status = SYNTAX_BEGIN;
                        break;
                    }
                    else {
                        for(int i = strlen(r) - 1; i >= 0; --i){
                            analyser.push(r[i]);
                        }
                        status = SYNTAX_BEGIN;
                        break;
                    }
                }
            case SYNTAX_ERROR:
                return false;
        }
    }


}
int main(int argc ,char * argv[]) {
    char  source[20];
    char dest[20];
    if(argc != 3){
        cerr << "InValid argument\n";
        exit(0);
    }
    strcpy(source,argv[1]);
    strcpy(dest,argv[2]);
    FILE * fin,* fout;
    if((fin = fopen(source,"r") )  == NULL ){
        cerr << "Error in open source file\n";
    }
    if((fout = fopen(dest,"w") )  == NULL ){
        cerr << "Error in open source file\n";
    }
    lex  =  new LexicalAnalyser(source);
    lex->Run();
    for(auto index = LexOutput.begin() ; index != LexOutput.end(); ++ index){
        cout << index->key << "\t" << index->value << endl;
    }
    lex->Export(dest);
	if(SyntaxAnalyser() == 1){
		cout << "SyntaxCheck Result: True\n";
	}
	else{
		cout << "SyntaxCheck Result: False\n";
	}
    delete lex;

    return 0;
}
