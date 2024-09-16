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
    if(regcomp(regex, _regex, REG_EXTENDED) != 0) {
        LexerError(__FILE__, __LINE__, "Failed to compile regex", __extension__ __PRETTY_FUNCTION__);
    }
    return regex;
}

Token ParseNewToken(TokenStream* stream);

TokenStream TokenStreamNew_(FILE* file_handle, LexerRuleset ruleset) {
    assert(file_handle != NULL && 
    ferror(file_handle)==0 );
    assert(ruleset.rules != NULL && "No rules present in the ruleset");
    TokenStream stream = {.next = TokenStreamNext, .peek = TokenStreamPeek};
    fseek(file_handle, 0, SEEK_END);
    stream.src_length = ftell(file_handle);
    fseek(file_handle, 0, SEEK_SET);
    stream.src = malloc(sizeof(char) + stream.src_length);
    assert(stream.src != NULL && "FAILED TO ALLOCATE FILE SOURCE, EXITING...");
    fread(stream.src, stream.src_length, 1, file_handle);
    stream.ruleset = ruleset;
    stream.current = ParseNewToken(&stream);
    return stream;
}

TokenStream TokenStreamNewWithExtSource(const char* source, LexerRuleset ruleset) {
    assert(source != NULL && "Source is null");
    assert(ruleset.rules != NULL && "No rules present in the ruleset");

    TokenStream stream = {.next = TokenStreamNext, .peek = TokenStreamPeek};
    stream.src_length = strlen(source);
    stream.src = source;
    stream.ruleset = ruleset;
    stream.current = ParseNewToken(&stream);
    return stream;
}

void TokenStreamDestroy(TokenStream* stream) {
    if(stream->src) free(stream->src);
    *stream = (TokenStream){0};
}

Token TokenStreamNext(TokenStream* stream) {
    Token ret = stream->current;
    stream->current = ParseNewToken(stream);
    return ret;
}

Token TokenStreamPeek(TokenStream* stream) {
    return stream->current;
}

void SetTokenType(Token* tok, LexerRule r) {
    #define ___TOKTHELPER(_enum, type, tokt, prefix) case _enum:tok->tokt = r.token_type_##prefix;break;
    switch (r.token_type_type) {
    ___TOKTHELPER(LEXER_TOKEN_TYPE_TYPE_CHAR  ,char  , toktc, char);
    ___TOKTHELPER(LEXER_TOKEN_TYPE_TYPE_SHORT ,short , tokts, short);
    ___TOKTHELPER(LEXER_TOKEN_TYPE_TYPE_INT   ,int   , tokti, int);
    ___TOKTHELPER(LEXER_TOKEN_TYPE_TYPE_LONG  ,long  , toktl, long);
    ___TOKTHELPER(LEXER_TOKEN_TYPE_TYPE_FLOAT ,float , toktf, float);
    ___TOKTHELPER(LEXER_TOKEN_TYPE_TYPE_DOUBLE,double, toktd, double);
    ___TOKTHELPER(LEXER_TOKEN_TYPE_TYPE_STRING,char*, toktstr,str);
    default:assert(0);break;
    }
    #undef ___TOKTHELPER
}

void UpdateLineAndColumn(TokenStream* stream, const char* token_value, size_t token_len) {
    for (size_t i = 0; i < token_len; i++) {
        if (token_value[i] == '\n') {
            stream->line++;
            stream->column = 1; // Reset column at the start of a new line
        } else {
            stream->column++;
        }
    }
}

void SkipWhiteSpaces(TokenStream* stream) {
    while (stream->idx < stream->src_length && isspace(stream->src[stream->idx])) { stream->idx++; if(stream->src[stream->idx] == '\n' || stream->src[stream->idx] == '\r') { stream->line++; stream->column = 0;} else {stream->column++;}} 
}

Token ParseNewToken(TokenStream* stream) {
    Token ret = {0};
    SkipWhiteSpaces(stream);
    ret.line = stream->line;    // Initialize the token's starting line
    ret.column = stream->column; // Initialize the token's starting column
    for (size_t i = 0; i < LexerRule_vec_length(&stream->ruleset.rules); i++) {
        LexerRule rule = stream->ruleset.rules[i];
        switch (rule.type) {
        case LEXER_TOKEN_RULE_TYPE_STRING: {
            if((stream->idx >= stream->src_length || stream->idx + rule.len > stream->src_length) || strncmp(stream->src + stream->idx, rule.str, rule.len) != 0) continue;
            
            SetTokenType(&ret, rule);
            ret.value = stream->src + stream->idx;
            ret.value_len = rule.len;
            UpdateLineAndColumn(stream, ret.value, ret.value_len);  // Update line and column
            goto found;
        } break;

        case LEXER_TOKEN_RULE_TYPE_FUNCTION: {
            Token func_token;
            if(!(*rule.fn)(&rule, stream, &func_token)) continue;
            SetTokenType(&func_token, rule);
            UpdateLineAndColumn(stream, func_token.value, func_token.value_len); // Update line and column
            return func_token;
        } break;

        case LEXER_TOKEN_RULE_TYPE_REGEX: {
            regmatch_t match;
            int rc = regexec(rule.regex, stream->src + stream->idx, 1, &match, 0);
            if (rc == 0) {
                ret.value = stream->src + stream->idx;
                ret.value_len = match.rm_eo - match.rm_so;
                SetTokenType(&ret, rule);
                UpdateLineAndColumn(stream, ret.value, ret.value_len); // Update line and column
                goto found;
            }
            continue;
        } break;

        default:
            assert(false && "Unknown rule type");
            break;
        }
    }

    // If no rule matched, print an error message
    fprintf(stderr, "Failed to parse token from stream at line %zu, column %zu\n", 
            stream->line, stream->column);
    return ret;

found:
    stream->idx += ret.value_len; // Update stream index after a token is parsed
    return ret;
}