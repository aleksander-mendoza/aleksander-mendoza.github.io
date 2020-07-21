#include <stdlib.h>
#include "ast.h"

struct AST * mkUnion(struct AST * lhs,struct AST * rhs){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = AST_UNION;
	struct ASTUnion uni = {lhs,rhs};
	node->uni = uni;
	return node;
}
struct AST * mkConcat(struct AST * lhs,struct AST * rhs){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = AST_CONCAT;
	struct ASTConcat concat = {lhs,rhs};
	node->concat = concat;
	return node;
}
struct AST * mkKleene(struct AST * child){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = AST_KLEENE;
	struct ASTKleene kleene = {child};
	node->kleene = kleene;
	return node;
}
struct AST * mkOutput(struct AST * child,char * strOut){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = AST_OUTPUT;
	struct ASTOutput output = {child,strOut};
	node->output = output;
	return node;
}
struct AST * mkRange(char from, char to){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = AST_RANGE;
	struct ASTRange range = {from,to};
	node->range = range;
	return node;
}
struct AST * mkWeightBefore(struct AST * child,int weight){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = AST_WEIGHT_BEFORE;
	struct ASTWeightBefore weightBefore = {child,weight};
	node->weightBefore = weightBefore;
	return node;
}
struct AST * mkWeightAfter(struct AST * child,int weight){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = AST_WEIGHT_AFTER;
	struct ASTWeightAfter weightAfter = {child,weight};
	node->weightAfter = weightAfter;
	return node;
}
struct AST * mkEpsilon(){
	struct AST * node = malloc(sizeof(struct AST));
	node->type = AST_EPSILON;
	return node;
}

void freeAST(struct AST * root){
    switch(root->type){
    case AST_UNION:
        freeAST(root->uni.lhs);
        freeAST(root->uni.rhs);
        free(root);
        return;
    case AST_CONCAT:
        freeAST(root->concat.lhs);
        freeAST(root->concat.rhs);
        free(root);
        return;
    case AST_KLEENE:
        freeAST(root->kleene.child);
        free(root);
        return;
    case AST_OUTPUT:
        freeAST(root->output.child);
        free(root->output.outStr);
        free(root);
        return;
    case AST_WEIGHT_AFTER:
        freeAST(root->weightAfter.child);
        free(root);
        return;
    case AST_WEIGHT_BEFORE:
        freeAST(root->weightBefore.child);
        free(root);
        return;
    case AST_RANGE:
        free(root);
        return;
    case AST_EPSILON:
        free(root);
        return;
    }
    fprintf(stderr,"Unexpected type %d\n",root->type);
    exit(1);
}

struct AST * copyAST(struct AST * root){
	switch(root->type){
    case AST_UNION:
        return mkUnion(copyAST(root->uni.lhs),copyAST(root->uni.rhs));
    case AST_CONCAT:
        return mkConcat(copyAST(root->concat.lhs),copyAST(root->concat.rhs));
    case AST_KLEENE:
        return mkKleene(copyAST(root->kleene.child));
    case AST_OUTPUT:
        return mkOutput(copyAST(root->output.child),copyStr(root->output.outStr));
    case AST_WEIGHT_AFTER:
        return mkWeightAfter(copyAST(root->weightAfter.child),root->weightAfter.weight);
    case AST_WEIGHT_BEFORE:
        return mkWeightBefore(copyAST(root->weightBefore.child),root->weightBefore.weight);
    case AST_RANGE:
        return mkRange(root->range.from,root->range.to);
    case AST_EPSILON:
        return mkEpsilon();
    }
    fprintf(stderr,"Unexpected type %d\n",root->type);
    exit(1);
}

