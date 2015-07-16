//
// Created by 戎 佳磊 on 15/6/20.
//

#include "LexicalAnalyser.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctype.h>
using namespace std;

vector<Tuple> LexOutput;
char  KeyWord[32][10] = {
        "auto","short","int","long","float","double","char","struct","union","eum","typedef","const",    "unsigned","extern","register","static","volatile","void","if","else","switch","goto","continue","break","default","sizeof","return","signed","case","for","do","while"
};
char Operator[38][5] = {
        "()","[]","->",".","!","~","++","--","+","-","*","&","/","%","<<",">>","<","<=",">",">=","==","!=","^","|","&&","||","?=","=","+=","-=","*=","/=","%=","&=","^=","|=","<<=",">>="
};

void LexicalAnalyser::Add(int key,string value){
        Tuple * t = new Tuple;
        t->key = key;
        t->value = value;
        LexOutput.push_back(*t);
        delete t;
        return;
}
LexicalAnalyser::LexicalAnalyser(char * filename) {
        FILE * fd = fopen(filename, "r");
        if ( fd  == NULL) {
                cerr  << "Error in open file\n";
                return;
        }
        fseek(fd, 0, SEEK_END);
        size_t size = ftell(fd);
        rewind(fd);
        Val = new char [size + 1];
        size_t read_size = fread(Val, sizeof(char), size, fd);
        Val[size] = '\0';
        if (read_size != size) {
                cerr << "Error in read File\n";
                delete []Val;
        }
        this->RemoveAnnotation();
}
LexicalAnalyser::~LexicalAnalyser() {
        if(Val){
                delete []Val;
        }
}
void LexicalAnalyser::RemoveAnnotation() {
        char * buff = new char[strlen(Val) + 1];
        char * buff_add = buff;
        char * pointer = Val;
        int flag = 0;

        while ( *pointer ) {
                //  annotation like   end util \n
                if (flag == 1) {
                        if (*pointer == '\n') {
                                flag = 0;
                                ++pointer;
                                continue;
                        }
                        else{
                                ++pointer;
                                continue;
                        }
                }
                if (flag == 2) {
                        // annotation like /* */
                        if (*pointer == '*') {
                                if (*(pointer + 1) == '/') {
                                        flag = 0;
                                        pointer += 2;
                                        continue;
                                }
                                else{
                                        ++pointer;
                                        continue;
                                }
                        }
                        else{
                                ++pointer;
                                continue;
                        }
                }
                if (*pointer != '/' && flag == 0) {
                        *(buff_add++) = *pointer;
                }
                if (*pointer  == '/' && flag == 0) {
                        if (*(pointer + 1) == '/') {
                                flag = 1;
                                pointer += 2;
                                continue;
                        }
                        if (*(pointer + 1) == '*') {
                                pointer += 2;
                                flag = 2;
                                continue;
                        }
                        else{
                                *(buff_add++) = *pointer;
                        }
                }
                ++pointer;
        }
        *buff_add = '\0';
        strcpy(Val, buff);
}
int LexicalAnalyser::TypeOper(char * s)const{
        for (int i = 0 ; i < 38; ++i) {
                if (strcmp(s, Operator[i]) == 0) {
                        return i + 42;
                }
        }
        return -1;
}
bool LexicalAnalyser::Getchar(char &s) {
        s = Val[pointer++];
        if(s == '\0')return false;
        return true;
}
bool LexicalAnalyser::Getchar_RemoveBlank(char &s) {
        while(IsBlank(Val[pointer]) == true)++pointer;
        s = Val[pointer++];
        if(s == '\0')return false;
        return true;
}
bool LexicalAnalyser::IsBlank(char s) const {
        if(s == '\n' || s == '\t' || s == ' ')return true;
        return false;
}
bool LexicalAnalyser::IsDigit(char s) const {
        if( s >= '0' && s <= '9')return true;
        else
                return false;
}
bool LexicalAnalyser::IsOperator(char s)const{
        if(s == '=' || s == '+' || s == '-' || s == '*' || s == '/' || s == '<' || s == '>' || s =='!' || s == '?' || s == '^' || s == '|' || s == '%' || s == '&' || s == '~' || s == '[' || s == '-' || s == ':' || s == ']')return true;
        return false;
}
bool LexicalAnalyser::IsDelim(char s)const{
        if (s == '(' || s == ')' || s == '{' || s == '}' || s == ',' || s == ';' )return true;
        return false;
}
int LexicalAnalyser::IsKeyWord(char * s)const{
        for (int i = 0; i < 32; ++i) {
                if (strcmp(s, KeyWord[i]) == 0) {
                        return i + 1;
                }
        }
        return 0;
}
void LexicalAnalyser::Run(){
        char s;
        bool status = true; // whether the file is end
        int status1 = 0; // persent []
        int status2 = 0; // persent ? :
        while (Getchar_RemoveBlank(s) == true && status == true) {
                if (s == '#') {
                        char * Token = new  char [1024];
                        int i = 1;
                        Token[0] = s ;
                        while ((status = Getchar(Token[i])) ) {
                                if (Token[i] == '\n') {
                                        Token[i] = '\0';
                                        break;
                                }
                                ++i;
                        }
                        if (status == false) {
                                Token[i] = '\0';
                        }
                        Add(PROCESSING, string(Token));
                        delete []Token;
                        continue;
                }
                if (s == '_' || isalpha(s) == true) {
                        //KeyWord or Identifier
                        char * Token = new char [1024];
                        Token[0] = s;
                        int i = 1;

                        while ((status = Getchar(Token[i])) == true) {
                                if (isdigit(Token[i]) == false && isalpha(Token[i]) == false && Token[i] != '_') {
                                        --i;
                                        --pointer;
                                        break;
                                }
                                ++i;
                        }
                        if (status == false) {
                                --i;
                        }
                        Token[ i + 1 ]= '\0';
                        int type = IsKeyWord(Token);
                        Add(type, string(Token));
                        delete []Token;
                        continue;
                }
                else if (IsDigit(s) == true  || s == '.') {
                        // number
                        int IntOrFloat = 0;
                        char * Token =new char [1024];
                        if(s == '.')IntOrFloat = 1;
                        Token[0] = s;
                        int i = 1;
                        int type ;
                        int flag = 0;
                        while ((status == Getchar(Token[i])) == true) {
                                if (Token[i] == '.') {
                                        IntOrFloat = 1;
                                        ++i;
                                        continue;
                                }
                                if (Token[i] == 'e') {
                                        ++i;
                                        continue;
                                }
                                if(Token[i] == 'x'){
                                        ++i;
                                        continue;
                                }
                                if (IsDigit(Token[i]) == false) {
                                        if (Token[i] == 'f' || Token[i] == 'F' ) {
                                                flag = 1;
                                                break;
                                        }
                                        else if ( Token[i] == 'l' || Token[i] == 'L') {
                                                flag =2;
                                                break;
                                        }
                                        else if (Token[i] == 'u'){
                                                flag = 3;

                                                break;
                                        }
                                        --pointer;
                                        --i;
                                        break;
                                }
                                ++i;
                        }

                        if (status == false) {
                                --i;
                        }
                        Token[i + 1] = '\0';
                        if(IntOrFloat == 0){
                            if (flag == 3)type = UNSIGNED_INT;
                            else if(flag == 2)type = LONG_INT;
                            else type = INT;
                        }
                        if(IntOrFloat == 1){
                            if(flag == 1)type = FLOAT_SINGLE_PRECISION;
                            else type = FLOAT_DOUBLE_PRECISION;
                        }
                        Add(type, string(Token));
                        delete []Token;
                        continue;
                }
                else if (s == '\"') {
                    //String Constant
                        char * Token = new char [1024];
                        int i = 0;
                        while ((status = Getchar(Token[i])) == true) {
                                if (Token[i] == '\"') {
                                        --i;
                                        break;
                                }
                                if(Token[i] == '\\'){
                                    ++i;
                                    Getchar(Token[i]);
                                    if(Token[i] == 'n'){
                                        Token[--i] = '\n';
                                    }
                                    else if(Token[i] == 'r'){
                                        Token[--i] = '\r';
                                    }
                                    else if(Token[i] == 't'){
                                        Token[--i] = '\t';
                                    }
                                    else if(Token[i] ==
                                            '\\'){
                                        Token[--i]= '\\';
                                        }
                                    else if(Token[i] == 'x'){
                                        char tmp[3];
                                        Getchar(tmp[0]);
                                        Getchar(tmp[1]);
                                        tmp[2] = '\0';
                                        long int val;
                                        char * sv;
                                        val = strtol(tmp,&sv,16);
                                        Token[--i] = char(val);
                                    }

                                }
                                ++i;
                        }
                        int type;
                        if (status == false) {
                                --i;
                                type = ERROR;
                        }
                        else type = STRING_CONSTANT;
                        Token[i + 1] = '\0';
                        int ret = this->DigitStringCheck(Token);
                        if(ret == -1) {
                            Add(type, string(Token));
                        }
                        else {
                            Add(type,std::to_string(ret));
                        }
                        delete []Token;
                        continue;
                }
                else if (s == '\'') {
                        char c ;
                        Getchar(c);
                        char next;
                        status = Getchar(next);
                        int type;
                        if (next != '\'') {
                                type = ERROR;
                        }
                        else{
                                type = CHAR_CONSTANT;
                        }
                        Add(type, string(1,s));
                        continue;
                }
                else if(IsDelim(s) == true){
                        int type = DELIMITER;
                        Add(type, string(1,s));
                        continue;
                }
                else if(IsOperator(s) == true){
                        if (s == '[') {
                                status1 ++;
                                continue;
                        }
                        if (s == ']') {
                                if (status1 > 0) {
                                        Add(TypeOper("[]"), string("[]"));
                                        status1 --;
                                        continue;
                                }
                                else{
                                        Add(ERROR, string("[]"));
                                        continue;
                                }
                        }
                        if (s == '?') {
                                status2 ++;
                                continue;
                        }
                        if (s == ':') {
                                if (status2 > 0) {
                                        Add(TypeOper("?:"), string("?:"));
                                        status2 -- ;
                                        continue;
                                }
                                else{
                                        Add(ERROR, string("?:"));
                                        continue;
                                }
                        }
                        char * Token =new char[4];
                        Token[0] = s;
                        if (s == '-') {
                                char next ;
                                if ((status ==  Getchar(next)) == true) {
                                        if (next == '=' || next == '>' || next == '-') {
                                                Token[1] = next;
                                                Token[2] = '\0';
                                                Add(TypeOper(Token), string(Token));
                                                continue;
                                        }
                                        else{
                                                Token[1] = '\0';
                                                --pointer; //roll back
                                                Add(TypeOper(Token), string(Token));
                                                continue;
                                        }
                                }
                        }
                        else if (s == '+') {
                                char next ;
                                if ((status ==  Getchar(next)) == true) {
                                        if (next == '=' || next == '+' ) {
                                                Token[1] = next;
                                                Token[2] = '\0';
                                                Add(TypeOper(Token), string(Token));
                                                continue;
                                        }
                                        else{
                                                Token[1] = '\0';
                                                --pointer; //roll back
                                                Add(TypeOper(Token), string(Token));
                                                continue;
                                        }
                                }
                        }
                        else if (s == '<') {
                                char next ;
                                if ((status ==  Getchar(next)) == true) {
                                        if (next == '<') {
                                                char third;
                                                if ((status == Getchar(third)) == true) {
                                                        if (third == '=') {
                                                                Token[1] = next;
                                                                Token[2] = third;
                                                                Token[3] = '\0';
                                                                Add(TypeOper(Token), string(Token));
                                                                continue;
                                                        }
                                                }
                                                else{
                                                        Token[1] = next;
                                                        Token[2] = '\0';
                                                        --pointer;
                                                        Add(TypeOper(Token), string(Token));
                                                        continue;
                                                }
                                        }
                                        else if(next == '='){
                                                Token[1] = next;
                                                Token[2] = '\0';
                                                Add(TypeOper(Token), string(Token));
                                                continue;

                                        }

                                        else{
                                                --pointer; //roll back
                                                Token[1] = '\0';
                                                Add(TypeOper(Token), string(Token));
                                                continue;
                                        }
                                }

                                Add(TypeOper("<"), string(Token));
                                continue;
                        }
                        else if (s == '>') {
                                char next ;
                                if ((status ==  Getchar(next)) == true) {
                                        if (next == '>') {
                                                char third;
                                                if ((status == Getchar(third)) == true) {
                                                        if (third == '=') {
                                                                Token[1] = next;
                                                                Token[2] = third;
                                                                Token[3] = '\0';
                                                                Add(TypeOper(Token), string(Token));
                                                                continue;
                                                        }
                                                }
                                                else{
                                                        Token[1] = next;
                                                        Token[2] = '\0';
                                                        --pointer;
                                                        Add(TypeOper(Token), string(Token));
                                                        continue;
                                                }
                                        }
                                        else if(next == '='){
                                                Token[1] = next;
                                                Token[2] = '\0';
                                                Add(TypeOper(Token), string(Token));
                                                continue;

                                        }
                                        else{
                                                Token[1] = '\0';
                                                --pointer; //roll back
                                                Add(TypeOper(Token), string(Token));
                                                continue;
                                        }
                                }

                                Add(TypeOper(">"), string(Token));
                                continue;
                        }
                        else if (s == '&') {
                                char next ;
                                if ((status ==  Getchar(next)) == true) {
                                        if (next == '=' || next == '&' ) {
                                                Token[1] = next;
                                                Token[2] = '\0';
                                                Add(TypeOper(Token), string(Token));
                                                continue;
                                        }
                                        else{
                                                --pointer; //roll back
                                                Token[1] = '\0';
                                                Add(TypeOper(Token), string(Token));
                                                continue;
                                        }
                                }
                        }
                        else if (s == '|') {
                                char next ;
                                if ((status ==  Getchar(next)) == true) {
                                        if (next == '=' || next == '|' ) {
                                                Token[1] = next;
                                                Token[2] = '\0';
                                                Add(TypeOper(Token), string(Token));
                                                continue;
                                        }
                                        else{
                                                Token[1] = '\0';
                                                --pointer; //roll back
                                                Add(TypeOper(Token), string(Token));
                                                continue;
                                        }
                                }
                        }
                        else if (s == '!' || s == '^' || s == '*' || s == '/' || s == '=' ) {
                                char next;
                                if ((status ==  Getchar(next)) == true) {
                                        if (next == '=') {
                                                Token[1] = next;
                                                Token[2] = '\0';
                                                Add(TypeOper(Token), string(Token));
                                        }
                                        else{
                                                --pointer; //roll back
                                                Token[1] = '\0';
                                                Add(TypeOper(Token), string(Token));
                                        }
                                }
                        }
                        else if (s == '.') {
                                char * Token = new char [1024];
                                int i = 1;
                                Token[0] = '.';
                                if ((status =  Getchar(Token[i])) == true) {
                                        if (this->IsDigit(Token[i]) == true) {
                                                ++i;
                                                status = Getchar(Token[i]);
                                                while (IsDigit(Token[i]) == true  || Token[i] == 'e')  {
                                                        ++i;
                                                        status = Getchar(Token[i]);
                                                }
                                                --pointer;
                                                Token[i] = '\0';
                                                int type;
                                                if (Token[i-1] == 'e') {
                                                        type = ERROR;
                                                }
                                                else {
                                                        type = FLOAT_DOUBLE_PRECISION;
                                                }
                                                Add(type, string(Token));
                                                continue;
                                        }
                                        else{
                                                Tuple tmp;
                                                Add(TypeOper("."),string("."));
                                                continue;
                                        }
                                }
                        }
                        else{
                                Token[1] = '\0';
                                Add(TypeOper(Token), string(Token));
                        }
                        delete []Token;
                        continue;
                }

                else{
                        Add(ERROR, string(1,s));
                        continue;
                }

        }
}
void LexicalAnalyser::Export(char *filename) const {
        ofstream fout;
        fout.open(filename);
        for (auto index = LexOutput.begin(); index != LexOutput.end(); ++index) {
                if (index->key == ERROR) fout << "Error\t";
                else if(index->key == IDENTIFIER)fout << "Identifier\t";
                else if(index->key >= 1 && index->key <= 32)fout << "KeyWord\t";
                else if(index->key == STRING_CONSTANT)fout << "Constant String\t";
                else if(index->key == CHAR_CONSTANT)fout << "Constant Char\t";
                else if(index->key == INT)fout << "Int\t";
                else if(index->key == UNSIGNED_INT)fout << "UnsignInt\t";
                else if(index->key == LONG_INT)fout << "LongInt\t";
                else if(index->key == FLOAT_SINGLE_PRECISION)fout << "Single_Float_Constant\t";
                else if(index->key == FLOAT_DOUBLE_PRECISION)fout << "Double_Float_Constant\t";
                else if(index->key == PROCESSING)fout << "PROCESSING\t";
                else if(index->key == DELIMITER)fout << "DELIMITER\t";
                else if(index->key >= 42 && index->key <= 79)fout << "OPERATOR\t";
                else fout << "Error\t";

                fout << index->value<<endl;
        }
}
int LexicalAnalyser::DigitStringCheck(char * s) const {
    int type = 0;
    string _s(s);
    int judge = 0;
    if( _s[0] == '0'){
        if(_s[1] == 'x'){
            type = 2;
        }
        else type = 1;
    }
    int i = type;
    for(; i < _s.length();++i){
        if(type == 2){
            if(IsDigit(_s[i]) == true || (_s[i] <= 'f' && _s[i] >= 'a') || (_s[i] <= 'F' && _s[i] >= 'A') || _s[i] == '.')continue;
            else {
                judge = 1;
                break;
            }
        }
        else {
            if(IsDigit(_s[i]) == true || _s[i] == '.')continue;
            else {
                judge = 1;
                break;
            }
        }
    }

    long int ret = 0;
    int octal =0;
    if(type == 1)octal = 8;
    else if(type == 2)octal = 16;
    else octal  =10;
    if(judge){
        return -1;
    }
    else {
        char * tmp;
        ret = strtol(s,&tmp,octal);
        return ret;
    }

}