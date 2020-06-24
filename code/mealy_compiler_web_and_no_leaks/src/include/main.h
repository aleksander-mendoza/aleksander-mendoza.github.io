#ifndef H_MAIN
#define H_MAIN
struct AST;

struct ASTLetter{
	char literal;
};

struct ASTUnion{
	struct AST * lhs;//left-hand side
	struct AST * rhs;//right-hand side
};

struct ASTConcat{
	struct AST * lhs;
	struct AST * rhs;
};

struct ASTKleene{
	struct AST * child;
};

struct ASTOutput{
	struct AST * child;
	char * outStr;
};

struct AST{
	/**0 for union;1 for concatenation; 
	2 for kleene closure; 3 for printing output; 
	4 for letter literal; 5 for epsilon literal*/
	int type;
	union{
		struct ASTUnion uni;
		struct ASTConcat concat;
		struct ASTKleene kleene;
		struct ASTOutput output;
		struct ASTLetter letter;
	};
};

struct AST * mkUnion(struct AST * lhs,struct AST * rhs);
struct AST * mkConcat(struct AST * lhs,struct AST * rhs);
struct AST * mkKleene(struct AST * child);
struct AST * mkOutput(struct AST * child,char * strOut);
struct AST * mkLetter(char literal);
struct AST * mkEpsilon();

#endif