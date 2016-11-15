#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

/*Gives back the next available id*/
int mk_new_ventaid(SQLHDBC dbc) {
  	int id;
  	char consulta[1000] = "select max(order) from ventas";
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

int exists_usr(SQLHDBC dbc, char* name) {
	char consulta[1000];
	SQLHSTMT stmt;
	SQLRETURN ret;

	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
	sprintf(consulta, "SELECT scrname FROM fiduser WHERE scrname = '%s';", name);

	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
	
	SQLExecute(stmt);
	if(SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
	  	printf("El usuario %s existe\n", name);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return 0;
	}
	else{
		printf("El usuario %s no existe\n", name);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return 1;
	}
}

int get_user_id(SQLHDBC dbc, char* scrname){
	char consulta[1000];
	SQLHSTMT stmt;
	SQLRETURN ret;
	int uid;

	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
	sprintf(consulta, "SELECT user_id FROM fiduser WHERE scrname = '%s';", scrname);

	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
	
	SQLExecute(stmt);

	SQLBindCol(stmt, 1, SQL_INTEGER, &uid, sizeof(uid), NULL);

	if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return uid;
    }
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return -1;
}

int add_venta(SQLHDBC dbc, int id, char* scrname, char* date){
	char consulta[1000];
	SQLHSTMT stmt;
	int uid;

	uid = get_user_id(dbc, scrname);

	if(uid == -1){
		printf("Unable to find users Id\n");
		return -1;
	}

	sprintf(consulta, "Insert into ventas values (%d, %d, %s)",id, uid, date); 

  	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
	
	SQLExecute(stmt);

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return 0;
}

void add_incluye(SQLHDBC dbc, int id, char* isbn){
	char consulta[1000];
	SQLHSTMT stmt;

	sprintf(consulta, "Insert into incluye values (%s, %d)",isbn, id); 

  	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
	
	SQLExecute(stmt);

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return;
}

int main(int argc, char* argv[]){
	SQLHENV env;
  	SQLHDBC dbc;
  	SQLRETURN ret; /* ODBC API return status */
  	int id;
  	int i;
 	
 	if(argc < 4){
 		printf("Please use the format:\n./compra <Scrname> <Date(YYYY-MM-DD)> <ISBNS>\n");
		return EXIT_FAILURE;
 	}

 	 /* CONNECT */
	ret = odbc_connect(&env, &dbc);
	if (!SQL_SUCCEEDED(ret)) {
		printf("Error connecting.\n");
	    return EXIT_FAILURE;
	}

 	if(exists_usr(dbc, argv[2]) != 0){
 		ret = odbc_disconnect(env, dbc);
		if (!SQL_SUCCEEDED(ret)) {
			printf("Error disconnecting.\n");
  			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
 	}

 	id = mk_new_ventaid(dbc);

 	if(add_venta(dbc, id, argv[2], argv[3]) == -1){
 		/* DISCONNECT */
	    ret = odbc_disconnect(env, dbc);
	    if (!SQL_SUCCEEDED(ret)) {
	        return EXIT_FAILURE;
	    }	

	    return EXIT_FAILURE;
 	}

 	for(i = 4; i < argc; i++){
 		add_incluye(dbc, id, argv[i]);
 	}


 	/* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}