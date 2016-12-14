#include "operation.h"

/* OFFSET OPERATION: operation whose result all but the first N results of another source operation */

typedef struct {
	operation_t* suboperation;
	int offset;
	int count;
} operation_offset_args_t;

void
operation_offset_reset(void* vargs) {
	operation_offset_args_t* args = NULL;
	int i;

	if(!vargs){
		return;
	}

	args = vargs;

	operation_reset(args->suboperation);
	args->count = 0;
	return;
}

int operation_offset_next(void* vargs) {
	operation_offset_args_t* args = NULL;

	if(!vargs){
		return 0;
	}

	args = vargs;

	while(args->count < args->offset){
		operation_next(args->suboperation);
		args->count++;
	}

	return (operation_next(args->suboperation));
}

void* operation_offset_get(int col, void* vargs) {
    operation_offset_args_t* args = NULL;

	if(!vargs){
		return NULL;
	}

	args = vargs;

	return (operation_get(col, args->suboperation));
}

void operation_offset_close(void* vargs) {
    operation_offset_args_t* args = NULL;

	if(!vargs){
		return;
	}

	args = vargs;

	operation_close(args->suboperation);
	free(args);
	return;
}

operation_t*
operation_offset_create(operation_t* suboperation, int offset) {
    operation_t* operation = NULL;
	operation_offset_args_t* args = NULL;
	int ncols;

	if(!suboperation || offset < 0){
		return NULL;
	}

	operation_reset(suboperation);

	ncols = operation_ncols(suboperation);

	operation = malloc(sizeof(operation_t));
	if(!operation){
		return NULL;
	}

	operation->reset = operation_offset_reset;
	operation->next = operation_offset_next;
	operation->get = operation_offset_get;
	operation->close = operation_offset_close;
	operation->ncols = ncols;

	args = malloc(sizeof(operation_offset_args_t));
	if(!args){
		free(operation);
		return NULL;
	}

	args->suboperation = suboperation;
	args->offset = offset;
	args->count = 0;

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

