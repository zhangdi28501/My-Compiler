// main function
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <unordered_map>
// system call about files
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
// user defile
#include "c_minus.h"
#include "vmachine.h"
using namespace std;

int token;      //type of the token we got
int lineno = 1; //line number of program
int decl_type;  //type of the whole declaration
int exp_type;   //type of expression answer
long long Hash;       //mark the cuurent id in lexical
int layer;      //mark which layer an id belongs to in symtab

bool is_decl = true;
//when lexical parse an id, judge legal or not
//if in decl mode, new id can be add into symtab, else illegal

int token_val = 0;      //default literal int value
double token_d_val = 0; //literal double value
char token_str_val[64]; //store literal constant of string
// char *src;              //buffer of file input

char *reserve[12] = {"char", "int", "double", "else", "enum", "if",
                     "return", "sizeof", "while", "and", "or", "not"};

// system function name
char *sys[8] = {"open", "read", "close", "printf",
                "malloc", "memset", "memcmp", "exit"};
//notice we onlt deal with dec value
/**
 * symbal table, using hash to find an id quickly
 * hash value is key, ID info is the value
 * symtab.back() is the current layer
 * search id from back to begin
 * initiate only one global layer
 * push back when enter a local area
 * pop back when leave a local area
 */
vector<unordered_map<long long, ID>> symtab(1);
vector<unordered_map<long long, ID>> ID_MAIN(1);
//TODO add function stack
//unordered_map<int, vector<int>> func_inst; // + fun_para
//unordered_map<int, vector<int>> func_var; // + fun_var
//int * 数组不太好动态伸缩长度，这里改用vector变长数组

//calculat the hash value of an id
long long hash_str(char *s)
{
    long long ans = 0;
    while (*s != '\0')
        ans = ans * 147 + *s++;
    return ans;
}

void open_src(int fd, char **argv)
{
    int n;

    if((fd = open(*argv, 0)) < 0)
    { // fail to open target file
        cout << "couldn't open source file!\n";
        exit(1);
    }
    src = new char[BUFSIZE];
    if((n = read(fd, src, BUFSIZE)) < 0)
    { // faile to read target file
        cout << "read file error\n";
        exit(1);
    }

    lineno = 1;

    src[n] = '\0'; //add EOF
    close(fd);

}

//put build-in function into symtab
void init_symtab(int fd, char **argv)
{
    int i;
    long long hash;
    int j = OPEN;

    for (i = 0; i < 8; i++)
    {
        hash = hash_str(sys[i]);
        symtab[0][hash].Class = Sys;    //system function
        symtab[0][hash].Type = INT;     //variable data type or function return type
        symtab[0][hash].In_value = j++; //build-in function type
        // TODO initiate system function instruction stack
        symtab[0][hash].Name = sys[i];
    }

    next(); symtab[0][Hash].TOKEN = Char; // handle void type
    //next(); // ID_MAIN[0][Hash] = symtab[0][Hash];   // keep track of main
    //ID_MAIN_addr = (int *)ID_MAIN[0][hash].In_value;

    // read the source file
    if ((fd = open(*argv, 0)) < 0)
    {
        printf("could not open(%s)\n", *argv);
        exit(1);
    }

    if (!(src = oldsrc = (char*)malloc(poolsize)))
    {
        printf("could not malloc(%d) for source area\n", poolsize);
        exit(1);
    }
    // read the source file
    if ((i = read(fd, src, poolsize - 1)) <= 0)
    {
        printf("read() returned %d\n", i);
        exit(1);
    }
    src[i] = 0; // add EOF character
    close(fd);
}

int main(int argc, char **argv)
{
    int fd; // linux系统调用文件操作
    long long *temp;

    argc--;
    argv++;

    if (argc > 0 && **argv == '-' && (*argv)[1] == 's') {
        assembly = 1;
        --argc;
        ++argv;
    }
    /*  初始化编译器系统  */
    open_src(fd, argv);
    initVirtulMachine();
    init_symtab(fd, argv);
    /* 初始化完毕 */

    /* program: {glo_decl}+ */
    program();

    /* 如果没有main() */
    if (!(PC = (long long *)symtab[0][348352625].In_value))
    {
        cout << "main() not defined" << endl;
        return -1;
    }
    // dump_text();
    if (assembly) {
        // only for compile
        return 0;
    }

    // 初始化虚拟机的栈，当 main 函数结束时退出进程
    // stack位栈段，地址为栈底地址
    SP = (long long *)((long long)(stack) + poolsize);
    *--SP = EXIT; // call exit if main returns
    *--SP = PUSH;
    temp = SP;
    *--SP = argc;
    *--SP = (long long)(argv);
    *--SP = (long long)(temp);

    return eval();
}
