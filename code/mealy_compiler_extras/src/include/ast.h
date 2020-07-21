#ifndef H_AST
#define H_AST
struct AST;

struct ASTLetter{
	char literal;
};

struct ASTUnion{
	struct AST * lhs;
	struct AST * rhs;
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
struct ASTWeightBefore{
	struct AST * child;
	int weight;
};
struct ASTWeightAfter{
	struct AST * child;
	int weight;
};
struct ASTRange{
	char from;//inclusive
	char to;//inclusive
};
enum ASTType{
	AST_UNION,AST_CONCAT,AST_KLEENE,AST_OUTPUT,
	AST_EPSILON,AST_RANGE,AST_WEIGHT_BEFORE,AST_WEIGHT_AFTER
};
struct AST{
	enum ASTType type;
	union{
		struct ASTUnion uni;
		struct ASTConcat concat;
		struct ASTKleene kleene;
		struct ASTOutput output;
		struct ASTRange range;
		struct ASTWeightAfter weightAfter;
		struct ASTWeightBefore weightBefore;
	};
};

struct AST * mkUnion(struct AST * lhs,struct AST * rhs);
struct AST * mkConcat(struct AST * lhs,struct AST * rhs);
struct AST * mkKleene(struct AST * child);
struct AST * mkOutput(struct AST * child,char * strOut);
struct AST * mkWeightBefore(struct AST * child,int weight);
struct AST * mkWeightAfter(struct AST * child,int weight);
struct AST * mkRange(char from,char to);
struct AST * mkEpsilon();

void freeAST(struct AST * root);
struct AST * copyAST(struct AST * root);
#endif