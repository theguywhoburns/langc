#include <util.h>
#include <tinyvec.h>

PP_TFUNC(void*, PFN_LEXER_RULE, void* type, char* source, size_t index, size_t length);

/// @brief Used by the lexer to identify the type of a token rule, also used by the macros
enum LEXER_TOKEN_RULE_TYPE {
    LEXER_TOKEN_RULE_TYPE_STRING  = 0, // char* rules
    LEXER_TOKEN_RULE_TYPE_WSTRING = 1, // wchar_t* rules
    LEXER_TOKEN_RULE_TYPE_REGEX   = 2, // regex rules, basically a struct containing a compiled regex
    LEXER_TOKEN_RULE_TYPE_FUNCTION= 3, // user defined function
};

typedef struct LexerRule {
    enum LEXER_TOKEN_RULE_TYPE type;
    int token_type; // TODO: make the token type a union so the user can pass in strings as types too
    PP_ANON_UNION(
        PFN_LEXER_RULE fn;
        PP_ANON_STRUCT(char* str; size_t len;);
        PP_ANON_STRUCT(wchar_t* wstr; size_t wlen;);
        PP_ANON_STRUCT(regex_t* regex;);
    );
} LexerRule;

__define_tiny_templated_vec(LexerRule);

typedef struct LexerRuleset {
    LexerRule* rules; // tinyvec
} LexerRuleset;

#define LexerRuleNew(ruleset, token_type, rule) _Generic(rule, \
    const char*     : LexerRuleNew_(&ruleset, token_type, LEXER_TOKEN_RULE_TYPE_STRING,  rule), \
    char*           : LexerRuleNew_(&ruleset, token_type, LEXER_TOKEN_RULE_TYPE_STRING,  rule), \
    const wchar_t*  : LexerRuleNew_(&ruleset, token_type, LEXER_TOKEN_RULE_TYPE_WSTRING, rule), \
    wchar_t*        : LexerRuleNew_(&ruleset, token_type, LEXER_TOKEN_RULE_TYPE_WSTRING, rule), \
    const regex_t*  : LexerRuleNew_(&ruleset, token_type, LEXER_TOKEN_RULE_TYPE_REGEX,   rule), \
    regex_t*        : LexerRuleNew_(&ruleset, token_type, LEXER_TOKEN_RULE_TYPE_REGEX,   rule), \
    PFN_LEXER_RULE  : LexerRuleNew_(&ruleset, token_type, LEXER_TOKEN_RULE_TYPE_FUNCTION,rule), \
    default         : LexerError(__FILE__, __LINE__, "Unknown rule type", __extension__ __PRETTY_FUNCTION__) \
)
/// @brief Creates a new lexer rule
/// @param ruleset The ruleset to add the rule to
/// @param token_type The type of the token
/// @param type The type of the rule
/// @param rule The rule itself type recasted to the void*
void LexerRuleNew_(LexerRuleset* ruleset, int token_type, enum LEXER_TOKEN_RULE_TYPE type, void* rule);
void LexerError(const char* file, int line, const char* message, const char* function);