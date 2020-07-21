#include <stdlib.h>
#include "formal_languages.h"
#include "vars.h"

struct Vars * mkVarsUnion(struct Vars * lhs,struct Vars * rhs){
	struct Vars * node = malloc(sizeof(struct Vars));
	node->type = VARS_UNION;
	struct VarsUnion uni = {lhs,rhs};
	node->uni = uni;
	return node;
}
struct Vars * mkVarsConcat(struct Vars * lhs,struct Vars * rhs){
	struct Vars * node = malloc(sizeof(struct Vars));
	node->type = VARS_CONCAT;
	struct VarsConcat concat = {lhs,rhs};
	node->concat = concat;
	return node;
}
struct Vars * mkVarsKleene(struct Vars * child){
	struct Vars * node = malloc(sizeof(struct Vars));
	node->type = VARS_KLEENE;
	struct VarsKleene kleene = {child};
	node->kleene = kleene;
	return node;
}
struct Vars * mkVarsOutput(struct Vars * child,char * strOut){
	struct Vars * node = malloc(sizeof(struct Vars));
	node->type = VARS_OUTPUT;
	struct VarsOutput output = {child,strOut};
	node->output = output;
	return node;
}
struct Vars * mkVarsVars(char * id){
	struct Vars * node = malloc(sizeof(struct Vars));
	node->type = VARS_VAR;
	struct VarsVar var = {id};
	node->var = var;
	return node;
}
struct Vars * mkVarsRange(char from, char to){
	struct Vars * node = malloc(sizeof(struct Vars));
	node->type = VARS_RANGE;
	struct VarsRange range = {from,to};
	node->range = range;
	return node;
}
struct Vars * mkVarsWeightBefore(struct Vars * child,int weight){
	struct Vars * node = malloc(sizeof(struct Vars));
	node->type = VARS_WEIGHT_BEFORE;
	struct VarsWeightBefore weightBefore = {child,weight};
	node->weightBefore = weightBefore;
	return node;
}
struct Vars * mkVarsWeightAfter(struct Vars * child,int weight){
	struct Vars * node = malloc(sizeof(struct Vars));
	node->type = VARS_WEIGHT_AFTER;
	struct VarsWeightAfter weightAfter = {child,weight};
	node->weightAfter = weightAfter;
	return node;
}
struct Vars * mkVarsEpsilon(){
	struct Vars * node = malloc(sizeof(struct Vars));
	node->type = VARS_EPSILON;
	return node;
}

void freeVars(struct Vars * root){
    switch(root->type){
    case VARS_UNION:
        freeVars(root->uni.lhs);
        freeVars(root->uni.rhs);
        free(root);
        return;
    case VARS_CONCAT:
        freeVars(root->concat.lhs);
        freeVars(root->concat.rhs);
        free(root);
        return;
    case VARS_KLEENE:
        freeVars(root->kleene.child);
        free(root);
        return;
    case VARS_OUTPUT:
        freeVars(root->output.child);
        free(root->output.outStr);
        free(root);
        return;
    case VARS_VAR:
        free(root->var.id);
        free(root);
        return;
    case VARS_WEIGHT_AFTER:
        freeVars(root->weightAfter.child);
        free(root);
        return;
    case VARS_WEIGHT_BEFORE:
        freeVars(root->weightBefore.child);
        free(root);
        return;
    case VARS_RANGE:
        free(root);
        return;
    case VARS_EPSILON:
        free(root);
        return;
    }
    fprintf(stderr,"Unexpected type %d\n",root->type);
    exit(1);
}



