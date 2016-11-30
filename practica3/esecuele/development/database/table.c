#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"
#include "../type/type.h"

struct table_ {

  int ncols; /*Numero de columnas*/
  type_t* types; /*Array de tipos*/
  FILE* file; /*Fichero de la tabla*/
  long first_pos; /*Primera posicion*/
  long last_pos; /*Ultima posicion*/

};

void table_create(char* path, int ncols, type_t* types) {

  FILE *f = NULL;

  if(!path || ncols < 1 || !types){
    return;
  }

  f = fopen(path, "w+");
  if(!f){
    return;
  }

  fwrite(&ncols, sizeof(int), 1, f);
  fwrite(types, sizeof(type_t), ncols, f);

  fclose(f);

}

table_t* table_open(char* path) {

  table_t* table = NULL;

  if(!path){
    return NULL;
  }

  table = (table_t*)malloc(sizeof(table_t));
  if(!table) return NULL;

  table->file = fopen(path, "r+");
  if(!f){
    free(table);
    return NULL;
  }


  fread(&(table->ncols), sizeof(int), 1, f);

  table->types = (type_t*)malloc(sizeof(type_t)*table->ncols);
  if(!table->types){
    fclose(f);
    free(table);
    return NULL;
  }

  fread(table->types, sizeof(type_t), table->ncols, f);

  table->first_pos = ftell(f);
  fseek(f, 0, SEEK_END);
  table->last_pos = ftell(f);

  return table;

}

void table_close(table_t* table) {

  if(!table){
    return;
  }

  fclose(table->file);
  free(table->types);
  free(table);

  return;

}

int table_ncols(table_t* table) {

  if(!table){
    return 0;
  }

  return table->ncols;

}

type_t* table_types(table_t* table) {

  if(!table){
    return NULL;
  }

  return table->types;

}

long table_first_pos(table_t* table) {

  if(!table){
    return NULL;
  }

  return table->first_pos;

}

long table_last_pos(table_t* table) {

  if(!table){
    return NULL;
  }

  return table->last_pos;

}

record_t* table_read_record(table_t* table, long pos) {

  if(!table || pos >= table->last_pos || pos < 0){
    return NULL;
  }

  record_t* record = NULL;
  void** values;
  int size, i;

  values = (void **)malloc(sizeof(void*)*table->ncols);
  if(!values){
    return NULL;
  }

  fseek(f, pos, SEEK_SET);

  for(i = 0; i < table->ncols; i++){
    fread(&size, sizeof(type_t), 1, f);
    values[i] = (void *)malloc(size);
    fread(values[i], size, 1, f);
  }

  record = record_create(values, table->ncols, ftell(table->file));
  if(!record){
    for(i = 0; i < table->ncols; i++){
      free(values[i]);
    }
    free(values);
    return NULL;
  }

  return record;

}

void table_insert_record(table_t* table, void** values) {
  
  int size;

  if(!table || !values){
    return;
  }

  fseek(table->file, table->last_pos, SEEK_SET);

  for(i = 0; i < table->ncols; i++){
    size = value_length(table->types[i], values[i]);
    fwrite(&size, sizeof(int), 1, f);
    fwrite(values[i], size, 1, f);
  }

  table->last_pos = ftell(f);
  return;

}
