#include <lexer/lexer.h>
__define_tiny_templated_vec_impl(LexerRule);
void LexerRuleNew_(LexerRuleset* ruleset, int token_type, enum LEXER_TOKEN_RULE_TYPE type, void* rule) {
    assert(ruleset != NULL);
    assert(rule != NULL);
    if(ruleset->rules == NULL) {
        ruleset->rules = LexerRule_vec_new(32); // Usually this is enough
    }
    LexerRule _rule;
    _rule.type = type;
    switch(type) {
    case LEXER_TOKEN_RULE_TYPE_STRING: {
        char* _str = rule;
        char* copy = malloc(strlen(_str) + 1);
        strcpy(copy, _str);
        _rule.str = copy;
    } break;
    case LEXER_TOKEN_RULE_TYPE_WSTRING: {
        wchar_t* _str = rule;
        wchar_t* copy = malloc(wcslen(_str) + 1);
        wcscpy(copy, _str);
        _rule.wstr = copy;
    } break;
    case LEXER_TOKEN_RULE_TYPE_REGEX: {
        __warnattr("TODO: Implement regex copying");
        _rule.regex = rule;
    } break;
    case LEXER_TOKEN_RULE_TYPE_FUNCTION: {
        _rule.fn = rule;
    } break;
    default: assert(0);
    }

    LexerRule_vec_push(&ruleset->rules, _rule);
}

void LexerError(const char* file, int line, const char* message, const char* function) {
    ((file != ((void*)0)) ? (void) (0) : __assert_fail (message, file, line, function));
}
