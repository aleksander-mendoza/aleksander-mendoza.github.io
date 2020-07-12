#include <stdlib.h>
#include "ast.h"

struct AST * mkUnion(struct AST * lhs,struct AST * rhs){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 0;
	struct ASTUnion uni = {lhs,rhs};
	node->uni = uni;
	return node;
}
struct AST * mkConcat(struct AST * lhs,struct AST * rhs){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 1;
	struct ASTConcat concat = {lhs,rhs};
	node->concat = concat;
	return node;
}
struct AST * mkKleene(struct AST * child){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 2;
	struct ASTKleene kleene = {child};
	node->kleene = kleene;
	return node;
}
struct AST * mkOutput(struct AST * child,char * strOut){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 3;
	struct ASTOutput output = {child,strOut};
	node->output = output;
	return node;
}
struct AST * mkLetter(char c){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 4;
	struct ASTLetter letter = {c};
	node->letter = letter;
	return node;
}
struct AST * mkEpsilon(){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = 5;
	return node;
}

