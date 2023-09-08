#include "resources/util/lexer.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>

void lexerctx_init(const char *str, int size, LexerContext* ctx) 
{
    ctx->str = str;
    ctx->size = size;
    ctx->pos = 0;
}

int wordtoken_equal(LexerContext* ctx, const char* str, Token* token)
{
    if (token->m_type != WORD) { return 0; } 
    int len = strlen(str);
    if (len != token->m_data.m_word.size ) { return 0; }

    for (int i = 0; i < len; i++) 
    {
        if (str[i] != ctx->str[token->m_data.m_word.start + i]) { return 0; }
    }

    return 1;
}

void read_token(LexerContext* ctx, Token* token) 
{

    if (ctx->pos >= ctx->size) { token->m_type = EOF_TOKEN; return; }

    int i = ctx->pos;

    while (!(isspace(ctx->str[i]) || isalpha(ctx->str[i]) || isdigit(ctx->str[i]) || ctx->str[i] == '-' || ctx->str[i] == '#') && i < ctx->size) { i++; } //ignore

    if (i >= ctx->size) { ctx->pos = i; token->m_type = EOF_TOKEN; return; }
    
    if (ctx->str[i] == '#') 
    {
        i++;
        while (ctx->str[i] != '\n' && i < ctx->size) { i++; }

        if (i >= ctx->size) { ctx->pos = i; token->m_type = EOF_TOKEN; return; }
    }

    int j = i;

    while (isspace(ctx->str[i]) && i < ctx->size) { i++; } //parse whitespace

    if (i > j) { ctx->pos = i; token->m_type = WHITE_SPACE; return; }

    if (isalpha(ctx->str[i])) //parse word
    {
        i++;
        while((isalpha(ctx->str[i]) || isdigit(ctx->str[i])) && i < ctx->size) { i++; }
        token->m_type = WORD;
        token->m_data.m_word.start = j;
        token->m_data.m_word.size = i - j;
        ctx->pos = i;
        return;
    }

    if (isdigit(ctx->str[i]) || ctx->str[i] == '-') //parse number
    {
        int negative = ctx->str[i] == '-' ? 1 : 0;
        j = negative ? j + 1 : j;

        i++;
        while(isdigit(ctx->str[i]) && i < ctx->size) { i++; }
        int result = 0;
        
        int start = j;
        int size = i - j;

        
        int pow = 1;
        for (int k = i - 1; k >= j; k--) 
        {
            result += pow * (ctx->str[k] - '0');
            pow *= 10;
        }

        if (ctx->str[i] == '.') 
        {
            i++;

            int start = i;
            while(isdigit(ctx->str[i]) && i < ctx->size) { i++; }

            int size = i - start;
            pow = 1;
            int decimal_result = 0;
            for (int k = i - 1; k >= start; k--) 
            {
                decimal_result += pow * (ctx->str[k] - '0');
                pow *= 10;
            }

            float f_value = (float)result + ((float)decimal_result / (float)pow);
            token->m_type = FLOAT;
            token->m_data.m_float = f_value * (negative ? -1.0f : 1.0f);
            ctx->pos = i;
            return;
        }
        else 
        {
            token->m_type = INT;
            token->m_data.m_int = result * (negative ? -1 : 1);
            ctx->pos = i;
            return;
        }
    }

    printf("LEXER::ERROR [read_token : no token found]");
}