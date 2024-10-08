#include <lexer/lexer.h>

/// Last 4 bits correspond to token type
/// First 4 bits correspond to token sub-type
/// Basically same shit
#define TOKEN_EOF     0b00000000
#define TOKEN_UNKNOWN 0b11111111

#define TOKEN_PAREN   0b00000001
#define TOKEN_IDENT   0b00000010
#define TOKEN_NUMBER  0b00000011
#define TOKEN_COMMENT 0b00000100

// Parentheses
// ( or )
#define TOKEN_BRACKET  0b01000000
// [ or ]
#define TOKEN_SQUARE 0b00100000
// { or }
#define TOKEN_CURLY  0b01100000
// < or >
#define TOKEN_ANGLE  0b00010000
#define TOKEN_OPEN   0b10000000
#define TOKEN_CLOSE  0b00000000

// Identifier types
// anything except the 3 below
#define TOKEN_NORMAL_IDENT 0b00000000
// usually internal types
#define TOKEN_KEYWORD 0b10000000
// "smth"
#define TOKEN_STRING  0b01000000
// 'c'
#define TOKEN_CHAR    0b11000000
#define TOKEN_SYMBOL  0b00100000

// Number types
// anything except the 4 below will be treated as decimal, or it will FUCKING CRASH
#define TOKEN_DECIMAL 0b00000000
// 0x or NUM + h
#define TOKEN_HEX     0b10000000
// anything that starts with just 0
#define TOKEN_OCT     0b01000000
// 0b
#define TOKEN_BIN     0b11000000
// There will be no double sub-types so anything with . or f will be treated as float and recasted based on the context
#define TOKEN_FLOAT   0b00100000

#define TOKEN_MULTILINE_COMMENT  0b10000000
#define TOKEN_PREPROCESSOR_MACRO 0b01000000

#define IS_TYPE(t, type) (((t) & 0b00001111) == (type))

// Main checks
#define IS_PAREN(t) IS_TYPE(t, TOKEN_PAREN)
#define IS_IDENT(t) IS_TYPE(t, TOKEN_IDENT)
#define IS_NUMBER(t) IS_TYPE(t, TOKEN_NUMBER)
#define IS_COMMENT(t) IS_TYPE(t, TOKEN_COMMENT)

#define IS_OBRACKET(t) t == TOKEN_BRACKET | TOKEN_OPEN
#define IS_CBRACKET(t) t == TOKEN_BRACKET | TOKEN_CLOSE
#define IS_OSQUARE(t) t == TOKEN_SQUARE | TOKEN_OPEN
#define IS_CSQUARE(t) t == TOKEN_SQUARE | TOKEN_CLOSE
#define IS_OCURLY(t) t == TOKEN_CURLY | TOKEN_OPEN
#define IS_CCURLY(t) t == TOKEN_CURLY | TOKEN_CLOSE
#define IS_OANGLE(t) t == TOKEN_ANGLE | TOKEN_OPEN
#define IS_CANGLE(t) t == TOKEN_ANGLE | TOKEN_CLOSE

#define IS_BRACKET(t) (((t) & 0b01110000) == TOKEN_BRACKET && IS_PAREN(t))
#define IS_SQUARE(t) (((t) & 0b01110000) == TOKEN_SQUARE && IS_PAREN(t))
#define IS_CURLY(t) (((t) & 0b01110000) == TOKEN_CURLY && IS_PAREN(t))
#define IS_ANGLE(t) (((t) & 0b01110000) == TOKEN_ANGLE && IS_PAREN(t))
#define IS_OPEN(t) (((t) & 0b10000000) == TOKEN_OPEN && IS_PAREN(t))
#define IS_CLOSE(t) (((t) & 0b10000000) == TOKEN_CLOSE && IS_PAREN(t))

#define IS_NORMAL_IDENT(t) (((t) & 0b11110000) == TOKEN_NORMAL_IDENT && IS_IDENT(t))
#define IS_KEYWORD(t) (((t) & 0b11110000) == TOKEN_KEYWORD && IS_IDENT(t))
#define IS_STRING(t) (((t) & 0b11110000) == TOKEN_STRING && IS_IDENT(t))
#define IS_CHAR(t) (((t) & 0b11110000) == TOKEN_CHAR && IS_IDENT(t))
#define IS_SYMBOL(t) (((t) & 0b11110000) == TOKEN_SYMBOL && IS_IDENT(t))

#define IS_DECIMAL(t) (((t) & 0b11110000) == TOKEN_DECIMAL && IS_NUMBER(t))
#define IS_HEX(t) (((t) & 0b11110000) == TOKEN_HEX && IS_NUMBER(t))
#define IS_OCT(t) (((t) & 0b11110000) == TOKEN_OCT && IS_NUMBER(t))
#define IS_BIN(t) (((t) & 0b11110000) == TOKEN_BIN && IS_NUMBER(t))

#define IS_MULTILINE_COMMENT(t) (((t) & 0b11110000) == TOKEN_MULTILINE_COMMENT && IS_COMMENT(t))
#define IS_PREPROCESSOR_MACRO(t) (((t) & 0b11110000) == TOKEN_PREPROCESSOR_MACRO && IS_COMMENT(t))

#define IS_EOF(t) (t == TOKEN_EOF)
#define IS_UNKNOWN(t) (t == TOKEN_UNKNOWN)

bool tokgeteof(LexerRule* rule, TokenStream* stream, Token* t) {
	if(stream->idx == stream->src_length) {
		*t = (Token){.toktl = TOKEN_EOF, .line = stream->line, .column = stream->column};
		return true;
	}
	return false;
}

int main() {
	LexerRuleset set = {0};
	LexerRuleNew(set, TOKEN_EOF, tokgeteof); // EOF of the file, must be the first rule
	LexerRuleNew(set, TOKEN_PAREN | TOKEN_BRACKET | TOKEN_OPEN , "(");
	LexerRuleNew(set, TOKEN_PAREN | TOKEN_BRACKET | TOKEN_CLOSE, ")");
	LexerRuleNew(set, TOKEN_PAREN | TOKEN_SQUARE  | TOKEN_OPEN , "[");
	LexerRuleNew(set, TOKEN_PAREN | TOKEN_SQUARE  | TOKEN_CLOSE, "]");
	LexerRuleNew(set, TOKEN_PAREN | TOKEN_CURLY   | TOKEN_OPEN , "{");
	LexerRuleNew(set, TOKEN_PAREN | TOKEN_CURLY   | TOKEN_CLOSE, "}");
	LexerRuleNew(set, TOKEN_PAREN | TOKEN_ANGLE   | TOKEN_OPEN , "<");
	LexerRuleNew(set, TOKEN_PAREN | TOKEN_ANGLE   | TOKEN_CLOSE, ">");

	LexerRuleNew(set, TOKEN_NUMBER | TOKEN_DECIMAL, LexerCreateRegex("^[-+]?[0-9]+"));
	LexerRuleNew(set, TOKEN_NUMBER | TOKEN_HEX,     LexerCreateRegex("^0x[0-9a-fA-F]+|[0-9a-fA-F]+h"));
	LexerRuleNew(set, TOKEN_NUMBER | TOKEN_OCT,     LexerCreateRegex("^0[0-7]+"));
	LexerRuleNew(set, TOKEN_NUMBER | TOKEN_BIN,     LexerCreateRegex("^0b[01]+"));
	LexerRuleNew(set, TOKEN_NUMBER | TOKEN_FLOAT,   LexerCreateRegex("^[-+]?[0-9]*\\.[0-9]+([eE][-+]?[0-9]+)?"));

	LexerRuleNew(set, TOKEN_IDENT | TOKEN_CHAR,         LexerCreateRegex("^'.'"));
	LexerRuleNew(set, TOKEN_IDENT | TOKEN_STRING,       LexerCreateRegex("^\".*\""));
	LexerRuleNew(set, TOKEN_IDENT | TOKEN_KEYWORD,      LexerCreateRegex("^(char|double|float|int|long|short|signed|unsigned|void|_Bool|_Complex|_Imaginary|bool|complex|imaginary)"));
	LexerRuleNew(set, TOKEN_IDENT | TOKEN_NORMAL_IDENT, LexerCreateRegex("^[a-zA-Z_][a-zA-Z0-9_]*"));
	LexerRuleNew(set, TOKEN_IDENT | TOKEN_SYMBOL,		LexerCreateRegex("^(\\;|\\,|\\.|\\+|\\-|\\*)"));

	LexerRuleNew(set, TOKEN_COMMENT, LexerCreateRegex("^//.*"));
	LexerRuleNew(set, TOKEN_COMMENT | TOKEN_MULTILINE_COMMENT,  LexerCreateRegex("^/\\*.*\\*/"));
	LexerRuleNew(set, TOKEN_COMMENT | TOKEN_PREPROCESSOR_MACRO, LexerCreateRegex("^#.*"));
	LexerRuleNew(set, TOKEN_UNKNOWN, LexerCreateRegex(".*"));

	for(int i = 0; i < LexerRule_vec_length(&set.rules); i++) {
		LexerRule rule = set.rules[i];
		switch (rule.type) {
		case LEXER_TOKEN_RULE_TYPE_STRING:  printf("[%d:%d\t]:STRING RULE:  0b%b,    \t \"%.*s\"\n",    i, rule.token_type_type, rule.token_type_int, rule.len, rule.str);break;
		case LEXER_TOKEN_RULE_TYPE_REGEX:   printf("[%d:%d\t]:REGEX RULE:   0b%b,    \t %zu\n",      i, rule.token_type_type, rule.token_type_int, rule.regex->__fastmap);break;
		case LEXER_TOKEN_RULE_TYPE_FUNCTION:printf("[%d:%d\t]:FUNCTION RULE:0b%b,\t %p\n",          i, rule.token_type_type, rule.token_type_int, rule.fn);break;
		default:printf("Wut?\n");
		}
	}
	
	const char* test_c_source = "int main() { return 0; }";
	TokenStream lexer = TokenStreamNewWithExtSource(test_c_source, set);
	while (true) {
		Token token = lexer.next(&lexer);
		if (token.tokti == TOKEN_EOF)
			break;
		printf("[%zu:%zu]: 0b%b,    \t %.*s\n",
		       token.line, token.column, token.tokti, token.value_len, token.value);
	}
	return 0;
}