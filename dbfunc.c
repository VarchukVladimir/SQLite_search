/*
 * dbfunc.c
 *
 *  Created on: Feb 8, 2014
 *      Author: Volodymyr Varchuk
 */

#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "strfunc.h"
#include "dbfunc.h"
#include "env.h"

#define MAX_BUFF 1024



int do_Query_SQL (sqlite3 *db, char *SQL)
{
	return sqlite3_exec(db, SQL, 0, 0, 0);
}


int do_Query_SQL_row_count (sqlite3 *db, char *SQL)
{
	int reccount = 0;
	int result = 0;
	//sqlite3_exec(db, SQL, 0, 0, 0);
    sqlite3_stmt *pStmt;

    do {
    	result = sqlite3_prepare(db, SQL, -1, &pStmt, 0);
        if( result != SQLITE_OK ){
        	return result;
        }
        while ( (result = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
        	reccount++;
        }
        result = sqlite3_finalize(pStmt);
    }while (result==SQLITE_SCHEMA );

	return reccount;
}

int checkRec_Path_Name (sqlite3 *db, char *tableName)
{
	char *pathInfo = NULL;
	char *SQL_ = "SELECT ID FROM %s WHERE %s"; // 1. PATH_INFO field, 2. table name, 3. filter field PATH_INFO, 4. filter val
	char *SQL_Where = NULL;
	char *SQL = NULL;
	pathInfo = getenv (PATH_INFO_NAME);
	if (pathInfo == NULL)
		return -1;
	SQL = (char *) malloc ( sizeof (char) * ( strlen (SQL_) + strlen ( PATH_INFO_NAME ) + 20 + strlen (tableName) )  );
	sprintf (SQL, SQL_, DB_TABLE_NAME, PATH_INFO_NAME, pathInfo );
	if (do_Query_SQL_row_count (db, SQL) > 0)
	{
		free (SQL);
		return 1;
	}
	free (SQL);
	return 0;
}

int checkTable (sqlite3 *db, char *tableName)
{
	List_t listTables;
	int i = 0;
	initList( &listTables, 10 );
	get_tables_names (db, &listTables);

	for (i = 0; i < listTables.count; i++)
		if ( MY_STRNCMP(tableName, listTables.list[i]) == 0 )
		{
			return 0;
		}

	return 1;
}

int Query_SQL (sqlite3 *db, char * SQL)
{
    sqlite3_stmt *pStmt;
    int rowcount = 0;
    int result;
    do {
    	result = sqlite3_prepare(db, SQL, -1, &pStmt, 0);
        if( result!=SQLITE_OK ){
        	CHECK_DB_ERROR;
        	return result;
        }

        while ( (result = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
            int  i = 0;
            rowcount++;
            for ( i = 0; i < sqlite3_column_count(pStmt); i++)
            {
            	int colbytes = sqlite3_column_bytes (pStmt, i);
            	char buff [colbytes];

            	if (sqlite3_column_type(pStmt, i) == 3)
            		printf ( "%s\t|", sqlite3_column_text(pStmt, i) );
            	else
            		printf ( "%d\t|", sqlite3_column_int(pStmt, i) );
            }
            printf ("\n");

        }
        result = sqlite3_finalize(pStmt);
    }while (result==SQLITE_SCHEMA );

    printf ("query result have %d rows\n", rowcount);
	return result;
}


int insertSQL (sqlite3 *db, const char * SQL)
{
	int result = 0;
	sqlite3_stmt *pStmt;
	do {
		result = sqlite3_prepare(db, SQL, -1, &pStmt, 0);

		if( result!=SQLITE_OK ){
                return result;
        }
        result = sqlite3_step(pStmt);
        result = sqlite3_finalize(pStmt);
	} while (result == SQLITE_SCHEMA);


	return result;
}

int insertValues (sqlite3 *db, IntKeyValueList_t *intList, KeyValueList_t *strList, KeyValueList_t *fieldList, const char *tableName)
{
	int i = 0, j =0;
	char *SQL = NULL;
	char *SQL_ = "INSERT INTO %s(%s) VALUES(%s)"; // first string - table name, second string - inserting field name, third string "? ?" template for inserting values
	char *SQL_fields = NULL;
	char *SQL_values_template = NULL;
	int SQL_fields_size = 0;
	int SQL_values_template_size = 0;
	sqlite3_stmt *pStmt;
	int result = 0;

	if (tableName == NULL )
		return -1;


	for (i = 0; i < fieldList->count; i++)
	{
		SQL_fields_size += strlen (fieldList->pKey[i]) + 2;
	}
	SQL_fields_size++;
	SQL_values_template_size = fieldList->count * 3 + 1;

	SQL_values_template = (char *) malloc ( sizeof (char) * SQL_values_template_size);
	SQL_fields = (char *) malloc ( sizeof (char) * SQL_fields_size);

	SQL = (char *) malloc ( sizeof (char) * (strlen (SQL_) + strlen (tableName) + SQL_values_template_size + SQL_fields_size) );

	if (SQL == NULL || SQL_values_template == NULL || SQL_fields == NULL)
		return -1;

	SQL_values_template[0] = '\0';
	SQL_fields[0] = '\0';

	for (i = 0; i < fieldList->count; i++)
	{
		sprintf (SQL_values_template + strlen (SQL_values_template), "?, " );
		sprintf (SQL_fields + strlen (SQL_fields), "%s, ", fieldList->pKey[i] );
	}

	SQL_values_template[strlen (SQL_values_template) - 2] = '\0';
	SQL_fields[strlen (SQL_fields) - 2] = '\0';

	sprintf (SQL, SQL_, tableName, SQL_fields, SQL_values_template);

	printf ("%s\n", SQL);


	do {
		result = sqlite3_prepare(db, SQL, -1, &pStmt, 0);

		if( result!=SQLITE_OK ){
                return result;
        }
		for (i = 0; i < fieldList->count; i++)
		{
			// adding strng values
			for ( j = 0; j < strList->count; j++)
			{
				if ( MY_STRNCMP( strList->pKey[j], fieldList->pKey[i]) == 0)
				{
					sqlite3_bind_text (pStmt, i + 1, strList->pVal[j], -1, SQLITE_STATIC) ;// add value

				}
			}
			// adding int values
			for ( j = 0; j < intList->count; j++)
				if ( MY_STRNCMP( intList->pKey[j], fieldList->pKey[i]) == 0)
				{
					if ( MY_STRNCMP( fieldList->pVal[j], "") == 0)
					{
						char buff_int [20];
						sprintf(buff_int, "%d", intList->pVal[j]);
						sqlite3_bind_text (pStmt, i + 1, buff_int, -1, SQLITE_STATIC) ;//	 add value
					}
					else
						sqlite3_bind_int(pStmt, i + 1, intList->pVal[j]) ;// add value
				}
		}
        result = sqlite3_step(pStmt);
//		printf ( "%s\n", sqlite3_);
        result = sqlite3_finalize(pStmt);
	} while (result == SQLITE_SCHEMA);
	return 0;
}

int get_tables_names (sqlite3 *db, List_t *list)
{
	int table_count = 0;
	int result = 0;
	char * SQL = "SELECT name FROM sqlite_master WHERE type='table'"	;
    sqlite3_stmt *pStmt;
    do {
    	result = sqlite3_prepare(db, SQL, -1, &pStmt, 0);
        if( result!=SQLITE_OK ){
                return result;
        }

        while ((result = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
        	addToList( sqlite3_column_text (pStmt, 0), list);
        }
        result = sqlite3_finalize(pStmt);
    } while ( result == SQLITE_SCHEMA);

	return table_count;
}

int get_table_struct (sqlite3 *db, KeyValueList_t *list, const char * tableName )
{
	char *SQL_ = "SELECT * FROM %s LIMIT 1";
	char *SQL = NULL;
    sqlite3_stmt *pStmt;
    int result = 0;

    SQL = (char *) malloc (sizeof (char) * (strlen (tableName) + strlen (SQL_) + 2));
    sprintf (SQL, SQL_, tableName);
    do {
    	result = sqlite3_prepare(db, SQL, -1, &pStmt, 0);


    	if( result!=SQLITE_OK ){
                return result;
        }

        result = sqlite3_step(pStmt);
        int i = 0;
        int column_count = sqlite3_column_count(pStmt);
        for (i = 0; i < column_count; i++)
        {
        	char *datatype;
        	const char *collation;
        	int not_null;
        	int primary_key;
        	int autoincrement;

        	sqlite3_table_column_metadata( db, NULL, tableName, sqlite3_column_name(pStmt, i), (const char **) &datatype, &collation, &not_null, &primary_key, &autoincrement);
        	if (datatype != NULL)
        		addKeyVal ( (char *) sqlite3_column_name(pStmt, i), datatype, list);
        	else
        		addKeyVal ( (char *) sqlite3_column_name(pStmt, i), "", list);

        	//printf ("%d. name=%s, type%s.\n", i, (char *) sqlite3_column_name(pStmt, i),	 datatype);
        }
        result = sqlite3_finalize(pStmt);
    } while ( result == SQLITE_SCHEMA);
	return list->count;
}

int delete_table (sqlite3 *db, const char *table_name)
{
	char *SQL_ = "drop  table if exists ";
	char *SQL = NULL;
    int result = 0;

	SQL = (char *) malloc (sizeof (char) * ( strlen (table_name) + strlen (SQL_) + 2));
	sprintf (SQL, "%s%s", SQL_, table_name);

	result = sqlite3_exec(db, SQL, 0, 0, 0);

	return result;
}

/*
static int writeBlob(
                sqlite3 *db, 		 Database to insert data into
                const char *zKey, 	 Null-terminated key string
                void *zBlob, 		 Pointer to blob of data
                int nBlob 			 Length of data pointed to by zBlob
                ){

        const char *zSql = "INSERT INTO blobs(key, value) VALUES(?, ?)";
        sqlite3_stmt *pStmt;
        int rc;

        do {
                 Compile the INSERT statement into a virtual machine.
                rc = sqlite3_prepare(db, zSql, -1, &pStmt, 0);
                if( rc!=SQLITE_OK ){
                        return rc;
                }

                 Bind the key and value data for the new table entry to SQL variables
                 ** (the ? characters in the sql statement) in the compiled INSERT
                 ** statement.
                 **
                 ** NOTE: variables are numbered from left to right from 1 upwards.
                 ** Passing 0 as the second parameter of an sqlite3_bind_XXX() function
                 ** is an error.

                sqlite3_bind_text(pStmt, 1, zKey, -1, SQLITE_STATIC);
                sqlite3_bind_blob(pStmt, 2, zBlob, nBlob, SQLITE_STATIC);

                 Call sqlite3_step() to run the virtual machine. Since the SQL being
                 ** executed is not a SELECT statement, we assume no data will be returned.

                rc = sqlite3_step(pStmt);
                assert( rc!=SQLITE_ROW );

                 Finalize the virtual machine. This releases all memory and other
                 ** resources allocated by the sqlite3_prepare() call above.

                rc = sqlite3_finalize(pStmt);

                printf ("test\n");

                 If sqlite3_finalize() returned SQLITE_SCHEMA, then try to execute
                 ** the statement again.

        } while( rc == SQLITE_SCHEMA );

        return rc;
}
*/


int create_Table (sqlite3 *db, List_t *list)
{ //
	const char *SqlCreatTemplate = "CREATE VIRTUAL TABLE metatags_tab USING fts4 (ID INTEGER PRIMARY KEY NOT NULL UNIQUE %s )";
//	const char *SqlCreatTemplate = "CREATE TABLE metatags_tab (ID INTEGER PRIMARY KEY NOT NULL UNIQUE%s )";
	const char *SqlContentLength = ", CONTENT_LENGTH INTEGER";
	const char *SqlTEXT_FILED = "TEXT ";
	char *SQL = NULL;
	char *buff = NULL;
    int max_buff_len = MAX_BUFF;
    int buff_len = 0;
    int i = 0;
    int result;
	buff = malloc(MAX_BUFF);
	for (i = 0; i < list->count; i++)
	{
		if ((MY_STRNCMP (list->list[i], CONTETN_LENGTH_NAME)) == 0)
		{
			if (checkmemsize(strlen(buff) + strlen(SqlContentLength) + 1, &max_buff_len, buff) == NULL)
				return -1;
			sprintf(buff + strlen(buff), "%s", SqlContentLength);
			buff_len = strlen(buff);
		}
		else
		{
			if ((checkmemsize( buff_len + strlen(list->list[i]) + strlen(SqlTEXT_FILED) + 4, &max_buff_len, buff)) == NULL)
				return -1;
			sprintf(buff + buff_len, ", %s %s", list->list[i], SqlTEXT_FILED);
			buff_len = strlen(buff);
		}
	}
	SQL = (char *) malloc( sizeof(char) * (strlen(buff) + strlen(SqlCreatTemplate) + 1));
	sprintf(SQL, SqlCreatTemplate, buff);
	free(buff);
	// try to create new table
	result = sqlite3_exec(db, SQL, 0, 0, 0);
	printf ("%s\n", SQL);
	// if table "metatags_tab" exists - try to drop table and create it again
	if (result == 1 )
		if ( (result = delete_table (db, "metatags_tab")) != SQLITE_OK)
			return -1;
		else
			result = sqlite3_exec(db, SQL, 0, 0, 0);
	return result;
}

int db_open (const char * zDatabase, sqlite3 *db)
{
	printf ("%s\n", zDatabase);
	sqlite3_open(zDatabase, &db);
	if( SQLITE_OK != sqlite3_errcode(db) )
	{
			databaseError(db);
			return 1;
	}
	return 0;
}

void databaseError(sqlite3* db){
        int errcode = sqlite3_errcode(db);
        const char *errmsg = sqlite3_errmsg(db);
        fprintf(stdout, "Database error %d: %s\n", errcode, errmsg);
}

