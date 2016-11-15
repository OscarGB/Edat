#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

/*Gives back the next available id*/
int mk_new_usrid(SQLHDBC dbc) {
  	int id;
  	char consulta[1000] = "select max(user_id) from fiduser";
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

void add_ccard(SQLHDBC dbc, char* ccardno, char* exp){
	char consulta[1000];
	SQLHSTMT stmt;
	
	sprintf(consulta, "Insert into ccard values (%s, %s)", exp, ccardno); 

  	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
	
	SQLExecute(stmt);

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return;
}

void add_usr(SQLHDBC dbc, char* scrname, char* fullname, char* joindate, char* ccardno){
	char consulta[1000];
	SQLHSTMT stmt;
	
	sprintf(consulta, "Insert into fiduser values (%d, '%s', '%s', %s, FALSE, '%s')", mk_new_usrid(dbc), scrname, fullname, joindate, ccardno); 

  	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
 	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
	
	SQLExecute(stmt);

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return;
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

int exists_ccard(SQLHDBC dbc, char* ccardno) {
	char consulta[1000];
	SQLHSTMT stmt;
	SQLRETURN ret;

	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
	sprintf(consulta, "SELECT ccardno FROM ccard WHERE ccardno = '%s';", ccardno);

	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
	
	SQLExecute(stmt);
	if(SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
	  printf("La tarjeta %s existe\n", ccardno);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return 0;
	}
	else{
		printf("La tarjeta %s no existe\n", ccardno);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return 1;
	}
}

void delete_user(SQLHDBC dbc, char* scrname){
	char consulta[1000];
	SQLHSTMT stmt;

	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
	sprintf(consulta, "UPDATE fiduser SET borrado=True WHERE scrname = '%s';", scrname);
	SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
 	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
	
	SQLExecute(stmt);

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	return;
}

int main(int argc, char *argv[]) {
  SQLHENV env;
  SQLHDBC dbc;
  SQLRETURN ret; /* ODBC API return status */
 
 	if(argc < 2){
  		printf("Please use the format:\n./usuario + <scrname> \"<fullname>\" <joinig date(YYYY-MM-DD)> <ccard number> <ccard expiration(YYYY-MM-DD)>\nor\n./usuario - <scrname>\n");
		return EXIT_FAILURE;
  	}
	if(strcmp(argv[1], "+") == 0){
	  	if(argc != 7){
	  		printf("Please use the format:\n./usuario + <scrname> \"<fullname>\" <joinig date(YYYY-MM-DD)> <ccard number> <ccard expiration(YYYY-MM-DD)>\nor\n./usuario - <scrname>\n");
			return EXIT_FAILURE;
  		}
  	}

	if(strcmp(argv[1], "-") == 0){
  		if(argc != 3){
	  		printf("Please use the format:\n./usuario + <scrname> \"<fullname>\" <joinig date(YYYY-MM-DD)> <ccard number> <ccard expiration(YYYY-MM-DD)>\nor\n./usuario - <scrname>\n");
			return EXIT_FAILURE;
  		}
  	}	

  /* CONNECT */
	ret = odbc_connect(&env, &dbc);
	if (!SQL_SUCCEEDED(ret)) {
		printf("Error connecting.\n");
	    return EXIT_FAILURE;
	}

	if(strcmp(argv[1], "+") == 0){
		if(exists_usr(dbc, argv[2]) == 0){
			ret = odbc_disconnect(env, dbc);
    		if (!SQL_SUCCEEDED(ret)) {
				printf("Error disconnecting.\n");
      			return EXIT_FAILURE;
    		}
			return EXIT_SUCCESS;
		}
		
		if(exists_ccard(dbc, argv[5]) == 0){
			ret = odbc_disconnect(env, dbc);
    		if (!SQL_SUCCEEDED(ret)) {
				printf("Error disconnecting.\n");
      			return EXIT_FAILURE;
    		}
    		return EXIT_SUCCESS;
		}

		add_ccard(dbc, argv[5], argv[6]);
		add_usr(dbc, argv[2], argv[3], argv[4], argv[5]);
		
	}
	else if(strcmp(argv[1], "-") == 0){
		if(exists_usr(dbc, argv[2]) != 0){
			ret = odbc_disconnect(env, dbc);
    		if (!SQL_SUCCEEDED(ret)) {
				printf("Error disconnecting.\n");
      			return EXIT_FAILURE;
    		}
			return EXIT_SUCCESS;
		}
		delete_user(dbc, argv[2]);

	}

	/* DISCONNECT */
	ret = odbc_disconnect(env, dbc);
	if (!SQL_SUCCEEDED(ret)) {
		printf("Error disconnecting.\n");
	  	return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

		
