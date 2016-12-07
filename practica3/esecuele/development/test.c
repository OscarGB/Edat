#include <stdio.h>
#include "./database/database.h"
#include "./database/record.h"

int main(){
	table_t* table = NULL;
	database_t* database = NULL;
	type_t* type = NULL;
	type_t* t_type = NULL;
	int ncols = 3, i;
	record_t* record;
	void** values;
	void* aux = NULL;


	if(database_create("prueba") == -1) return -1;

	/*Creamos una base de datos y la abrimos*/
	database = database_open("prueba");
	if(!database){
		printf("Error al abrir la base de datos\n");
		return -1;
	}

	/*Creamos un array de types*/
	type = (type_t*) malloc (3*sizeof(type_t));
	if(!type){
		printf("Error1");
		database_close(database);
		return -1;
	}

	type[0] = INT;
	type[1] = DBL;
	type[2] = LNG;

	/*Creamos una tabla y la abrimos*/
	table_create("./prueba/nombre", ncols, type);

	table = table_open("./prueba/nombre");
	if(!table){
		printf("Error2");
		database_close(database);
		return -1;
	}

	/*Probamos table_ncols y table_types*/
	if(table_ncols(table) != ncols){
		printf("Error en table_ncols\n");
		table_close(table);
		database_close(database);
		return -1;
	}

	t_type = table_types(table);
	for(i = 0; i < ncols; i++){
		if(t_type[i] != type[i]){
			printf("Error en table_types\n");
			table_close(table);
			database_close(database);
			return -1;
		}
	}

	values = (void**) malloc (3*sizeof(void*));
	if(!values){
		table_close(table);
		database_close(database);
		return -1;
	}

	for(i = 0; i < ncols; i++){
		values[i] = (void*) malloc (value_length(type[i], 0));
	}

	*((int*)values[0]) = 3; /*INT*/
	*((double*)values[1]) = 2.1; /*DBL*/
	*((long*)values[2]) = 8.3; /*LNG*/

	table_insert_record(table, values);

	record = table_read_record(table, table_first_pos(table));

	aux = record_get(record, 0);

	if(*((int*)aux) != *((int*)values[0])){
		printf("Error en la insercion/obtencion de record\n");
		record_free(record);
		table_close(table);
		database_close(database);
		return -1;
	}

	aux = record_get(record, 1);

	if(*((double*)aux) != *((double*)values[1])){
		printf("Error en la insercion/obtencion de record\n");
		record_free(record);
		table_close(table);
		database_close(database);
		return -1;
	}

	aux = record_get(record, 2);

	if(*((long*)aux) != *((long*)values[2])){
		printf("Error en la insercion/obtencion de record\n");
		record_free(record);
		table_close(table);
		database_close(database);
		return -1;
	}
	
	for(i = 0; i < ncols; i++){
		free(values[i]);
	}
	free(values);
	record_free(record);
	table_close(table);
	database_close(database);

	printf("Todo correcto\n");

	return 0;
}