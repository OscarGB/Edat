#include "operation.h"

/* COUNT OPERATION: operation whose result is the number of results of the source operation */

typedef struct {
    int count;
    operation_t* suboperation;
} operation_count_args_t;

/*Reset de argumentos*/
void
operation_count_reset(void* vargs) {
	operation_count_args_t* args = NULL;

	if(!vargs) return;

    args = vargs;
    args->count = -1;
    operation_reset(args->suboperation);
    return;
}

/*Bucle para contar las columnas de la suboperación*/
int operation_count_next(void* vargs) {
	operation_count_args_t* args = NULL;

	if(!vargs) return 0;

	args = vargs;
	/*Si es mayor es que ya ha sido usado*/
	if(args->count >= 0){
		return 0;
	}

	else{
		args->count = 0;
		while(operation_next(args->suboperation)){
			args->count += 1;
		}

		return 1;
	}

}

/*Devuelve el número de resultados contados*/
void* operation_count_get(int col, void* vargs) {
    operation_count_args_t* args = NULL;

	if(!vargs) return NULL;

	args = vargs;
	return &(args->count);
}

/*Cierra y libera memoria de esta operación*/
void operation_count_close(void* vargs) {
	operation_count_args_t* args = NULL;

	if(!vargs) return;

	args = vargs;

	operation_close(args->suboperation);
	free(args);
	return;
}

/*Crea la operación, reservando memoria y asignando punteros*/
operation_t*
operation_count_create(operation_t* suboperation) {
    operation_t* operation = NULL;
    operation_count_args_t* args = NULL;

    /*Reservo memoria para la operación en sí*/
    operation = malloc(sizeof(operation_t));
    if(!operation){
    	return NULL;
    }

    /*Aisgno los punteros necesarios*/
    operation->reset = operation_count_reset;
    operation->next = operation_count_next;
    operation->get = operation_count_get;
    operation->close = operation_count_close;
    operation->ncols =  1;

    /*Reservo memoria para los args*/
    args = malloc(sizeof(operation_count_args_t));
    if(!args){
    	free(operation);
    	return NULL;
    }

    args->suboperation = suboperation;
    args->count = -1;

    /*Los asigno a la operación*/
    operation->args = args;

    /*Reservo memoria para los tipos, en este caso un solo tipo (INT)*/
    operation->types = malloc(sizeof(type_t));
    if(!operation->types){
    	free(operation->args);
    	free(operation);
    	return NULL;
    }

    operation->types[0] = INT;

    return operation;

}

