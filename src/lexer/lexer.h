#include <util.h>
#include <tinyvec.h>

PP_TSTRUCT(LexerRule);
PP_TSTRUCT(LexerRuleset);
PP_TSTRUCT(Token);
PP_TSTRUCT(TokenStream);

PP_TFUNC(bool, PFN_LEXER_RULE, LexerRule* rule, TokenStream* stream, Token* t);

/// @brief Used by the lexer to identify the type of a token rule, also used by the macros
enum LEXER_TOKEN_RULE_TYPE {
    LEXER_TOKEN_RULE_TYPE_STRING  = 0, // char* rules
    LEXER_TOKEN_RULE_TYPE_REGEX   = 2, // regex rules, basically a struct containing a compiled regex
    LEXER_TOKEN_RULE_TYPE_FUNCTION= 3, // user defined function
};

enum LEXER_TOKEN_TYPE_TYPE {
    LEXER_TOKEN_TYPE_TYPE_STRING  = 0, // char* type
    LEXER_TOKEN_TYPE_TYPE_CHAR    = 2, // char type
    LEXER_TOKEN_TYPE_TYPE_SHORT   = 3, // SHORT type
    LEXER_TOKEN_TYPE_TYPE_INT     = 4, // INT type
    LEXER_TOKEN_TYPE_TYPE_LONG    = 5, // LONG type
    LEXER_TOKEN_TYPE_TYPE_FLOAT   = 6, // FLOAT type, idk what idiots would use FLOAT
    LEXER_TOKEN_TYPE_TYPE_DOUBLE  = 7, // DOUBLE type same as FLOAT
};

struct LexerRule {
    enum LEXER_TOKEN_RULE_TYPE type;
    enum LEXER_TOKEN_TYPE_TYPE token_type_type;
    PP_ANON_UNION(
        PP_ANON_STRUCT(char* token_type_str; size_t token_type_str_len;);
        PP_ANON_STRUCT(wchar_t* token_type_wstr; size_t token_type_wstr_len;);
        char token_type_char;
        short token_type_short;
        int token_type_int;
        long token_type_long;
        float token_type_float;
        double token_type_double;
    );
    PP_ANON_UNION(
        PFN_LEXER_RULE fn;
        PP_ANON_STRUCT(char* str; size_t len;);
        PP_ANON_STRUCT(regex_t* regex;);
    );
};

struct Token {
    PP_ANON_UNION(
        PP_ANON_STRUCT(char*    toktstr;  size_t toktlen;);
        char   toktc;
        short  tokts;
        int    tokti;
        long   toktl;
        float  toktf;
        double toktd;
    );
    size_t value_len;
    char*  value;
    size_t line;
    size_t column;
};

__define_tiny_templated_vec(LexerRule);

struct LexerRuleset {
    LexerRule* rules; // tinyvec
};

#define PaprseTokenType(token_type) enum LEXER_TOKEN_TYPE_TYPE ___TEMP = _Generic(token_type, \
    const char*    : LEXER_TOKEN_TYPE_TYPE_STRING, \
    char*          : LEXER_TOKEN_TYPE_TYPE_STRING, \
    unsigned char  : LEXER_TOKEN_TYPE_TYPE_CHAR, \
    char           : LEXER_TOKEN_TYPE_TYPE_CHAR, \
    unsigned short : LEXER_TOKEN_TYPE_TYPE_SHORT, \
    short          : LEXER_TOKEN_TYPE_TYPE_SHORT, \
    unsigned int   : LEXER_TOKEN_TYPE_TYPE_INT, \
    int            : LEXER_TOKEN_TYPE_TYPE_INT, \
    unsigned long  : LEXER_TOKEN_TYPE_TYPE_LONG, \
    long           : LEXER_TOKEN_TYPE_TYPE_LONG, \
    float          : LEXER_TOKEN_TYPE_TYPE_FLOAT, \
    double         : LEXER_TOKEN_TYPE_TYPE_DOUBLE, \
    default        : LEXER_TOKEN_TYPE_TYPE_CHAR\
)

#define PTT(token_type) PaprseTokenType(token_type)

#define LexerRuleNew(ruleset, token_type, rule) { PaprseTokenType(token_type); PP_TYPEOF(token_type) ___TEMP_2 = token_type; _Generic(rule, \
    const char*     : LexerRuleNew_(&ruleset, ___TEMP, &___TEMP_2, LEXER_TOKEN_RULE_TYPE_STRING,  rule), \
    char*           : LexerRuleNew_(&ruleset, ___TEMP, &___TEMP_2, LEXER_TOKEN_RULE_TYPE_STRING,  rule), \
    const regex_t*  : LexerRuleNew_(&ruleset, ___TEMP, &___TEMP_2, LEXER_TOKEN_RULE_TYPE_REGEX,   rule), \
    regex_t*        : LexerRuleNew_(&ruleset, ___TEMP, &___TEMP_2, LEXER_TOKEN_RULE_TYPE_REGEX,   rule), \
    PFN_LEXER_RULE  : LexerRuleNew_(&ruleset, ___TEMP, &___TEMP_2, LEXER_TOKEN_RULE_TYPE_FUNCTION,rule), \
    default         : LexerError(__FILE__, __LINE__, "Unknown rule type", __extension__ __PRETTY_FUNCTION__) \
);}

/// @brief Creates a new lexer rule
/// @param ruleset The ruleset to add the rule to
/// @param token_type The type of the token
/// @param type The type of the rule
/// @param rule The rule itself type recasted to the void*
void LexerRuleNew_(LexerRuleset* ruleset, enum LEXER_TOKEN_TYPE_TYPE t,  void* token_type, enum LEXER_TOKEN_RULE_TYPE type, void* rule);
void LexerError(const char* file, int line, const char* message, const char* function);

regex_t* LexerCreateRegex(const char* regex);

#define TokenStreamNew(___creaton_data, ruleset) _Generic(___creation_data \
    const char*     : TokenStreamNew_(OpenFileHandle (___creation_data,  "r"), ruleset), \
    char*           : TokenStreamNew_(OpenFileHandle (___creation_data,  "r"), ruleset), \
)

struct TokenStream {
    LexerRuleset ruleset;
    Token current;
    size_t idx;
    size_t src_length;
    char*  src;
    size_t line;
    size_t column;
    PP_FUNCVAR(Token, next, TokenStream*);
    PP_FUNCVAR(Token, peek, TokenStream*);
};

TokenStream TokenStreamNew_(FILE* file_handle, LexerRuleset ruleset);
TokenStream TokenStreamNewWithExtSource(const char* source, LexerRuleset ruleset);
Token TokenStreamNext(TokenStream* stream);
Token TokenStreamPeek(TokenStream* stream);
void TokenStreamDestroy(TokenStream* stream);

void SkipWhiteSpaces(TokenStream* stream);