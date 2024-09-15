#include <lexer/lexer.h>
__define_tiny_templated_vec_impl(LexerRule);
void LexerRuleNew_(LexerRuleset* ruleset, enum LEXER_TOKEN_TYPE_TYPE t,  void* token_type, enum LEXER_TOKEN_RULE_TYPE type, void* rule) {
    assert(ruleset != NULL);
    assert(rule != NULL);
    if(ruleset->rules == NULL) {
        ruleset->rules = LexerRule_vec_new(32); // Usually this is enough
    }
    LexerRule _rule = {0};
    switch (t) {
    case LEXER_TOKEN_TYPE_TYPE_STRING: {
        _rule.token_type_type = LEXER_TOKEN_TYPE_TYPE_STRING;
        char* _str = *(char**)token_type;
        char* copy = malloc(strlen(_str) + 1);
        strcpy(copy, _str);
        _rule.str = copy;
        _rule.len = strlen(copy);
    }break;
    case LEXER_TOKEN_TYPE_TYPE_WSTRING: {
        _rule.token_type_type = LEXER_TOKEN_TYPE_TYPE_WSTRING;
        wchar_t* _str = token_type;
        wchar_t* copy = malloc(wcslen(_str) + 1);
        wcscpy(copy, _str);
        _rule.wstr = copy;
        _rule.wlen = wcslen(copy);
    }break;
    case LEXER_TOKEN_TYPE_TYPE_CHAR: {
        _rule.token_type_type = LEXER_TOKEN_TYPE_TYPE_CHAR;
        _rule.token_type_char = *(char*)token_type;
    }break;
    case LEXER_TOKEN_TYPE_TYPE_SHORT: {
        _rule.token_type_type = LEXER_TOKEN_TYPE_TYPE_SHORT;
        _rule.token_type_short = *(short*)token_type;
    }break;
    case LEXER_TOKEN_TYPE_TYPE_INT: {
        _rule.token_type_type = LEXER_TOKEN_TYPE_TYPE_INT;
        _rule.token_type_int = *(int*)token_type;
    }break;
    case LEXER_TOKEN_TYPE_TYPE_LONG: {
        _rule.token_type_type = LEXER_TOKEN_TYPE_TYPE_LONG;
        _rule.token_type_long = *(long*)token_type;
    }break; 
    case LEXER_TOKEN_TYPE_TYPE_FLOAT: {
        _rule.token_type_type = LEXER_TOKEN_TYPE_TYPE_FLOAT;
        _rule.token_type_float = *(float*)token_type;
    }break;
    case LEXER_TOKEN_TYPE_TYPE_DOUBLE: {
        _rule.token_type_type = LEXER_TOKEN_TYPE_TYPE_DOUBLE;
        _rule.token_type_double = *(double*)token_type;
    }break;
    default: assert(0);
    }
    _rule.type = type;
    switch(type) {
    case LEXER_TOKEN_RULE_TYPE_STRING: {
        char* _str = rule;
        char* copy = malloc(strlen(_str) + 1);
        strcpy(copy, _str);
        _rule.str = copy;
        _rule.len = strlen(copy);
    } break;
    case LEXER_TOKEN_RULE_TYPE_WSTRING: {
        wchar_t* _str = rule;
        wchar_t* copy = malloc(wcslen(_str) + 1);
        wcscpy(copy, _str);
        _rule.wstr = copy;
        _rule.wlen = wcslen(copy);
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

regex_t* LexerCreateRegex(const char* _regex) {
    regex_t* regex = (regex_t*)malloc(sizeof(regex_t));
    if(regcomp(regex, _regex, 0) != 0) {
        LexerError(__FILE__, __LINE__, "Failed to compile regex", __extension__ __PRETTY_FUNCTION__);
    }
    return regex;
}

#include <iconv.h>

regex_t* LexerCreateRegexW(const wchar_t* _regex) {
    iconv_t cd = iconv_open("UTF-8", "WCHAR_T");
    if(cd == (iconv_t)-1) {
        LexerError(__FILE__, __LINE__, "Failed to open iconv", __extension__ __PRETTY_FUNCTION__);
    }

    size_t len = wcslen(_regex);
    char* buf = (char*)malloc(len * 4 + 1);
    char* in = (char*)_regex;
    size_t in_bytes_left = len * sizeof(wchar_t);
    char* out = buf;
    size_t out_bytes_left = len * 4;
    size_t ret = iconv(cd, &in, &in_bytes_left, &out, &out_bytes_left);
    if(ret == (size_t)-1) {
        LexerError(__FILE__, __LINE__, "Failed to convert regex", __extension__ __PRETTY_FUNCTION__);
    }
    iconv_close(cd);
    regex_t* reg = (regex_t*)malloc(sizeof(regex_t));
    if(regcomp(reg, buf, 0) != 0) {
        LexerError(__FILE__, __LINE__, "Failed to compile regex", __extension__ __PRETTY_FUNCTION__);
    }
    free(buf);
    return reg;
}
