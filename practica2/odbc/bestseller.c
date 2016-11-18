#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

int get_best_seller(SQLHDBC dbc, int n){
	char consulta[1000];
	SQLHSTMT stmt;
	SQLRETURN ret;
	char title[1000];
	int sells;
	int i;

	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
	sprintf(consulta, "SELECT T1.ttl, T1.sells FROM	(SELECT min(e.titulo) as ttl, count(i.isbn) as sells FROM incluye as i, edicion as e 		WHERE i.isbn = e.isbn GROUP BY i.isbn) as T1 ORDER BY sells desc");
	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
	
	SQLExecute(stmt);

	SQLBindCol(stmt, 1, SQL_CHAR, title, sizeof(title), NULL);
	SQLBindCol(stmt, 2, SQL_INTEGER, &sells, sizeof(sells), NULL);

	for(i = 0; i < n; i ++){
		if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
			printf("Best seller numero %d: %s (vendido %d veces)\n", i+1, title, sells);
	    	}
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return 0;
}

int main(int argc, char* argv[]){
	SQLHENV env;
  	SQLHDBC dbc;
  	SQLRETURN ret; /* ODBC API return status */	
	int n;

	if(argc < 2){
		printf("Please use the format:\n./bestseller <Num>\n");
		return EXIT_FAILURE;
	}

	n = atoi(argv[1]);	

	/* CONNECT */
	ret = odbc_connect(&env, &dbc);
	if (!SQL_SUCCEEDED(ret)) {
		printf("Error connecting.\n");
	    return EXIT_FAILURE;
	}

	if(get_best_seller(dbc, n) != 0){
		ret = odbc_disconnect(env, dbc);
		if (!SQL_SUCCEEDED(ret)) {
			printf("Error disconnecting.\n");
  			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	/* DISCONNECT */
	ret = odbc_disconnect(env, dbc);
	if (!SQL_SUCCEEDED(ret)) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
	
}

