//
// Created by 戎 佳磊 on 15/6/20.
//
#include <iostream>
#include <vector>
#ifndef COMPILER_LEXICALANALYSER_H
#define COMPILER_LEXICALANALYSER_H

#endif //COMPILER_LEXICALANALYSER_H
#define ERROR -1
#define IDENTIFIER 0
#define STRING_CONSTANT 33
#define CHAR_CONSTANT 34
#define INT 35
#define UNSIGNED_INT 36
#define LONG_INT 37
#define FLOAT_SINGLE_PRECISION 38
#define FLOAT_DOUBLE_PRECISION 39
#define DELIMITER 40

#define PROCESSING 41

// Operator From 42 - 79
class LexicalAnalyser{
    char * Val;
    int pointer;
public:
    LexicalAnalyser(char * );
    ~LexicalAnalyser();
    bool Getchar(char &);
    bool Getchar_RemoveBlank(char &);
    bool IsOperator(char)const;
    bool IsDelim(char)const;
    bool IsBlank(char)const;
    bool IsDigit(char)const;
    void RemoveAnnotation();
    int IsKeyWord(char * )const;
    void Run();
    void Add(int,std::string);
    int TypeOper(char * )const;
    void Export(char * filename)const;
    int DigitStringCheck(char *)const;

};
typedef struct tuple{
    int key;
    std::string value;
}Tuple;
extern std::vector<Tuple> LexOutput;