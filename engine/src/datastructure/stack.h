#ifndef __STACK_H__
#define __STACK_H__

/* Resizable stack */
#define STACK_HEADER(TNAME, FNAME, TYPE)\
typedef struct\
{\
	u32 Length, Capacity;\
	TYPE *Elements;\
} Stack##TNAME;\
\
int stack_##FTYPE##_init(Stack##TNAME *stack, u32 initial_capacity);\
int stack_##FTYPE##_push(Stack##TNAME *stack, TYPE value);\
int stack_##FTYPE##_pop(Stack##TNAME *stack, TYPE *value);\
int stack_##FTYPE##_peek(Stack##TNAME *stack, TYPE *value);\
void stack_##FTYPE##_destroy(Stack##TNAME *stack);

#define STACK_IMPLEMENTATION(TNAME, FNAME, TYPE)\
int stack_##FTYPE##_init(Stack##TNAME *stack, u32 initial_capacity)\
{\
	stack->Length = 0;\
	stack->Capacity = initial_capacity;\
	if((stack->Elements = malloc(stack->Capacity * sizeof(TYPE))))\
	{\
		return -1;\
	}\
\
	return 0;\
}\
\
int stack_##FTYPE##_push(Stack##TNAME *stack, TYPE value)\
{\
	if(stack->Length + 1 >= stack->Capacity)\
	{\
		stack->Capacity *= 2;\
		TYPE *elems;\
		if((elems = realloc(stack->Elements, stack->Capacity * sizeof(TYPE))))\
		{\
			return -1;\
		}\
\
		stack->Elements = elems;\
	}\
\
	stack->Length++ = value;\
	return 0;\
}\
\
int stack_##FTYPE##_pop(Stack##TNAME *stack, TYPE *value)\
{\
	if(stack->Length == 0)\
	{\
		return -1;\
	}\
\
	*value = stack->Elements[--stack->Length];\
	return 0;\
}\
\
int stack_##FTYPE##_peek(Stack##TNAME *stack, TYPE *value)\
{\
	if(stack->Length == 0)\
	{\
		return -1;\
	}\
\
	*value = stack->Elements[stack->Length - 1];\
}\
\
void stack_##FTYPE##_destroy(stack##TNAME *stack)\
{\
	free(stack->Elements);\
}

#endif

