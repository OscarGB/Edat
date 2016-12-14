#include "operation.h"

/* LIMIT OPERATION: operation whose result is the first N results of another source operation */

typedef struct {
    operation_t* suboperation;
    int limit;
    int count;
} operation_limit_args_t;

void
operation_limit_reset(void* vargs) {
    operation_limit_args_t* args = NULL;

	if(!vargs){
		return;
	}

	args = vargs;
	args->count = 0;
	operation_reset(args->suboperation);
	return;
}

int operation_limit_next(void* vargs) {
    operation_limit_args_t* args = NULL;

	if(!vargs) return 0;

	args = vargs;

	args->count++;
	if(operation_next(args->suboperation) == 0){
		return 0;
	}

	return (args->limit >= args->count);
	

}

void* operation_limit_get(int col, void* vargs) {
    operation_limit_args_t* args = NULL;

    if(!vargs){
    	return NULL;
    }

    args = vargs;

    return operation_get(col, args->suboperation);

}

void operation_limit_close(void* vargs) {
    operation_limit_args_t* args = NULL;

    if(!vargs){
    	return;
    }

    args = vargs;

    operation_close(args->suboperation);
    free(args);
    return;
}

operation_t*
operation_limit_create(operation_t* suboperation, int limit) {
	operation_t* operation = NULL;
	operation_limit_args_t* args = NULL;
	int ncols;

	if(!suboperation || limit < 0){
		return NULL;
	}

	operation_reset(suboperation);

	ncols = operation_ncols(suboperation);

	operation = malloc(sizeof(operation_t));
	if(!operation){
		return NULL;
	}

	operation->reset = operation_limit_reset;
	operation->next = operation_limit_next;
	operation->get = operation_limit_get;
	operation->close = operation_limit_close;
	operation->ncols = ncols;

	args = malloc(sizeof(operation_limit_args_t));
	if(!args){
		free(operation);
		return NULL;
	}

	args->suboperation = suboperation;
	args->count = 0;
	args->limit = limit;

	operation->args = args;

	operation->types = malloc(ncols * sizeof(type_t));

	if(!operation->types){
    	free(operation->args);
    	free(operation);
    	return NULL;
    }

    memcpy(operation->types, suboperation->types, ncols * sizeof(type_t));

    return operation;
}


