#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

/*
 * example 4 with a queries build on-the-fly, the good way
 */

long mk_new_usrid(SQLDBC dbc) {
  long id;
  char consulta[1000] = "select max(user_id) from fiduser";

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


int exists_usr(SQLDBC dbc) {
}



int main(int argc, char *argv[]) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    SQLINTEGER x;
    /*	char fullname[512];
	char scrname[512]; */
	unsigned int i, j;
	char consulta[1000] = "";

	/*	if(!(argc == 2)){
		printf("Please use the format:\n./usuario +<scrname>\"<fullname>\"\nor\n./usuario -<scrname>\n");
		return EXIT_SUCCESS;
	}
	*/

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
	printf("Error connecting.\n");
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

	if(argv[1][0] == '+'){ /*Starts with + (add user)*/
	  /*
		for (i = 0; i < strlen(argv[1]) && argv[1][i + 1] != '\0' && argv[1][i + 1] != '"'; i++){
			scrname[i] = argv[1][i + 1];			
		}
		for(j = i + 2; j < strlen(argv[1]) && argv[1][j] != '\n' && argv[1][j] != '"'; j++){
			fullname[j - (i + 2)] = argv[1][j];
		} 
	  */		
		strcpy(consulta, "SELECT scrname FROM fiduser WHERE scrname = '");
		strcat(consulta, argv[2]);
		strcat(consulta, "';");

		SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
		
		SQLExecute(stmt);
		if(SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
		  printf("El usuario %s existe\n", argv[2]);
		}

	}
		/*
		

		SQLBindCol(stmt, 1, SQL_INTEGER, &x, sizeof(x), NULL);
		
		if(x >= 1){
			printf("The user does already exists.\n");
			goto EXIT;
		}

		strcpy(consulta, "INSERT INTO fiduser (scrname, fullname, ) VALUES(");
		strcat(consulta, scrname);
		strcat(consulta, ", "),
		strcat(consulta, fullname);
		strcat(consulta, ");");
		
		SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
		
		SQLExecute(stmt);

		printf("El usuario se ha añadido a la base de datos.\n");

		goto EXIT;		
	}

	for (i = 0; i < strlen(argv[1]) && argv[1][i + 1] != '\0' && argv[1][i + 1] != '"'; i++){
		scrname[i] = argv[1][i + 1];			
	}
	
	strcpy(consulta, "SELECT COUNT(*) FROM fiduser WHERE scrname = '");
	strcat(consulta, scrname);
	strcat(consulta, "';");

	SQLPrepare(stmt, (SQLCHAR*) consulta, SQL_NTS);
	
	SQLExecute(stmt);
	SQLBindCol(stmt, 1, SQL_INTEGER, &x, sizeof(x), NULL);
	
	if(x == 0){
		printf("The user does not exist.\n");
		goto EXIT;
	}

	strcpy(consulta, "UPDATE fiduser SET borrado = True WHERE scrname = '");
	strcat(consulta, scrname);
	strcat(consulta, "';");

	SQLExecute(stmt);

	printf("El usuario se ha borrado de la base de datos.\n");

	EXIT:
		*/	
    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
	printf("Error disconnecting.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
