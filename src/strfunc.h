/*
 * keyvalstr.h
 *
 *  Created on: Feb 6, 2014
 *      Author: Volodymyr Varchuk
 */

#ifndef KEYVALSTR_H_
#define KEYVALSTR_H_

#include "config_const_types.h"

char *getValArgs (char *);
char *getKeyArgs (char *);

char *getValArgs_e (char *);
char *getKeyArgs_e		 (char *);


char ** checkmemsize2 (int, int *, char**);
char * checkmemsize (int, int *, char*);
char * my_strtolower (char *);


int addToList (const char *, List_t *);
int initList (List_t *, int );

int initKVList (KeyValueList_t *, int);
int addKeyVal (char *, char *, KeyValueList_t *);

int addIntKeyVal (char *, int, IntKeyValueList_t *);
int initIntKVList (IntKeyValueList_t *, int);
void printIntKVList (IntKeyValueList_t *);

unsigned long num_CRC32 (const char *);
char * str_CRC32 (const char *);


char *getInsert_Values_Str ( KeyValueList_t *, List_t *, int );
char *getInsert_Values_Str_ID ( KeyValueList_t *, List_t *, int, int );
char *getWHERE_condition ( Options_t *);
int check_str_in_list (List_t *, char *);



#define MY_MAX(A, B) strlen (A) >= strlen (B) ? strlen (A) : strlen (B)
#define MY_STRNCMP(A, B) strncmp (my_strtolower(A) ,my_strtolower(B), MY_MAX(my_strtolower(A), my_strtolower(B)))

#endif /* KEYVALSTR_H_ */
