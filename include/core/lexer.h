#ifndef LEXER_H
#define LEXER_H

//LEXER

//tokens
enum TokenType 
{
    WORD, INT, FLOAT, WHITE_SPACE, SLASH, EOF_TOKEN
};

struct WordToken 
{
    int start;
    int size;
};

union TokenData
{
    struct WordToken m_word;
    int m_int;
    float m_float;
};

struct Token 
{
    int m_type;
    union TokenData m_data;
};

//Ctx used for reading tokens
struct LexerContext 
{
    const char* str; //file data
    int size;        //how big the file is
    int pos;         //where we are in the file
};

typedef struct Token Token;
typedef struct LexerContext LexerContext;

// init {ctx} 
void lexerctx_init(const char *str, int size, LexerContext* ctx);

// chech if {token} is a word and equal to {str}
int  wordtoken_equal(LexerContext* ctx, const char* str, Token* token);

// read the next {token} in the {ctx}
void read_token(LexerContext* ctx, Token* token);

#endif
