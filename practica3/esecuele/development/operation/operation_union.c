
#include "operation.h"

/* UNION OPERATION: concatenation of the results of two operations with the same columns */

typedef struct {
    operation_t* suboperation1;
    operation_t* suboperation2;
    int flag;
} operation_union_args_t;

/*Reset de argumentos*/
void
operation_union_reset(void* vargs) {
    operation_union_args_t* args = NULL;

    if(!vargs){
    	return;
    }

    args = vargs;
    operation_reset(args->suboperation1);
    operation_reset(args->suboperation2);
    args->flag = 1;
    return;
}

/*Retorno de los next de las suboperaciones*/
int operation_union_next(void* vargs) {
	int result = 0;
	operation_union_args_t* args = NULL;

	if(!vargs){
		return -1;
	}

	args = vargs;
	if(args->flag == 1) {
		result = operation_next(args->suboperation1);
		if(result == 0){
			args->flag = 2;
		}
	}
	if(args->flag == 2){
		result = operation_next(args->suboperation2);
	}

    return result; 
}

/*Retorno de los valores de las suboperaciones*/
void* operation_union_get(int col, void* vargs) {
    operation_union_args_t* args = NULL;

    if(!vargs){
    	return NULL;
    }

    args = vargs;

	if(args->flag == 1){
		return operation_get(col, args->suboperation1);
	}
	else if(args->flag == 2){
		return operation_get(col, args->suboperation2);
	}
	else{
		return NULL;
	}
}

/*Cierra y libera memoria de esta operación*/
void operation_union_close(void* vargs) {
    operation_union_args_t* args = NULL;

	if(!vargs){
		return;
	}

	args = vargs;

    operation_close(args->suboperation1);
    operation_close(args->suboperation2);
    free(args);
    return;
}

operation_t*
operation_union_create(operation_t* operation1, operation_t* operation2) {
    operation_t* operation = NULL;
    operation_union_args_t* args = NULL;
	int i, n_cols;

	if(!operation1 || !operation2){
		return NULL;
	}

	n_cols = operation_ncols(operation1);

	/*Compruebo que tengan el mismo número de columnas*/
	if(n_cols != operation_ncols(operation2)){
		return NULL;
	}

	/*Compruebo que las columnas sean iguales*/
	for(i = 0; i < n_cols; i++) {
		if(operation1->types[i] != operation2->types[i]){
			return NULL;
		}
	}
    
	operation_reset(operation1);
	operation_reset(operation2);
	
	/*Reservo memoria para la operación*/
    operation = malloc(sizeof(operation_t));
    if(!operation){
    	return NULL;
    }
	
    /*Asigno los punteros necesarios*/
    operation->reset = operation_union_reset;
    operation->next = operation_union_next;
    operation->get = operation_union_get;
    operation->close = operation_union_close;
    operation->ncols = n_cols;

    /*Reservo memoria para los args*/
	args = malloc(sizeof(operation_union_args_t));
    if(args == NULL){
    	free(operation);
    	return NULL;
    }
	
    args->suboperation1 = operation1;
    args->suboperation2 = operation2;
	args->flag = 1;
    
    /*Los asigno a la operación*/
    operation->args = args;

    /*Reservo memoria para los tipos, en este caso tantos como columnas haya*/
    operation->types = malloc(n_cols * sizeof(type_t));
	
    if(!operation->types){
    	free(operation->args);
    	free(operation);
    	return NULL;
    }
	
	/*Copio los valores de los tipos de una de las suboperaciones*/
    memcpy(operation->types, operation1->types, operation1->ncols * sizeof(type_t));
    
    return operation;
}

