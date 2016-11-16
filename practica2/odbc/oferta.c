#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

/*Gives back the next available id*/
int mk_new_offerid(SQLHDBC dbc) {
  	int id;
  	char consulta[1000] = "select max(offerta_id) from oferta";
	SQLHSTMT stmt;
	SQLRETURN ret;

  	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
  	SQLBindCol(stmt, 1, SQL_INTEGER, &id, sizeof(id), NULL);  
  	SQLExecute(stmt);
  	if(SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return id+1;
  	} 
  	else {
    	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    	return 1;
  	}
}

void add_offer(SQLHDBC dbc, int id, char* from, char* to, char* discount){
	char consulta[1000];
	SQLHSTMT stmt;
	
	sprintf(consulta, "Insert into XXXXXXXX values (%d, '%s', '%s', %s)", id, from, to, discount); 

  	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
 	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
	
	SQLExecute(stmt);

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return;
}

void add_offer_isbn(SQLHDBC dbc, int id, char* isbn){
	char consulta[1000];
	SQLHSTMT stmt;
	
	sprintf(consulta, "Insert into XXXXXXX values (%d, '%s')", id, isbn); 

  	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
 	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
	
	SQLExecute(stmt);

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return;
}

int main(int argc, char* argv[]){
	SQLHENV env;
  	SQLHDBC dbc;
  	SQLRETURN ret;/* ODBC API return status */
	int i, id;

	if(argc < 5){
		printf("Please use the format:\n./oferta <discount> <from (YYYY-MM-DD)> <to (YYYY-MM-DD)> <ISBNS>");

	}

	/* CONNECT */
	ret = odbc_connect(&env, &dbc);
	if (!SQL_SUCCEEDED(ret)) {
		printf("Error connecting.\n");
	    return EXIT_FAILURE;
	}
	
	id = mk_new_offerid(dbc);

	add_offer(dbc, id, argv[2], argv[3], argv[1]);

	for(i = 4; i < argc; i++){
		add_offer_isbn(dbc, id, argv[i]);
	}
	
	/* DISCONNECT */
	ret = odbc_disconnect(env, dbc);
	if (!SQL_SUCCEEDED(ret)) {
		printf("Error disconnecting.\n");
	  	return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
