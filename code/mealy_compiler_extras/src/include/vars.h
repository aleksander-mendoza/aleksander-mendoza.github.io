#ifndef H_VARS
#define H_VARS
struct Vars;

struct VarsUnion{
	struct Vars * lhs;
	struct Vars * rhs;
};

struct VarsConcat{
	struct Vars * lhs;
	struct Vars * rhs;
};

struct VarsKleene{
	struct Vars * child;
};

struct VarsOutput{
	struct Vars * child;
	char * outStr;
};
struct VarsRange{
	char from;
	char to;
};
struct VarsWeightBefore{
	struct Vars * child;
	int weight;
};
struct VarsWeightAfter{
	struct Vars * child;
	int weight;
};
struct VarsVar{
	char * id;
};
enum VarType{
	VARS_UNION,VARS_CONCAT,VARS_KLEENE,VARS_OUTPUT,
	VARS_EPSILON,VARS_RANGE,VARS_WEIGHT_BEFORE,VARS_WEIGHT_AFTER,
	VARS_VAR
};
struct Vars{
	enum VarType type;
	union{
		struct VarsUnion uni;
		struct VarsConcat concat;
		struct VarsKleene kleene;
		struct VarsOutput output;
		struct VarsRange range;
		struct VarsWeightBefore weightBefore;
		struct VarsWeightAfter weightBefore;
		struct VarsVar var;
	};
};

struct Vars * mkVarsUnion(struct Vars * lhs,struct Vars * rhs);
struct Vars * mkVarsConcat(struct Vars * lhs,struct Vars * rhs);
struct Vars * mkVarsKleene(struct Vars * child);
struct Vars * mkVarsOutput(struct Vars * child,char * strOut);
struct Vars * mkVarsVar(char * var);
struct Vars * mkVarsWeightBefore(struct Vars * child,int weight);
struct Vars * mkVarsWeightAfter(struct Vars * child,int weight);
struct Vars * mkVarsRange(char from,char to);
struct Vars * mkVarsEpsilon();

void freeVars(struct Vars * root);

#endif