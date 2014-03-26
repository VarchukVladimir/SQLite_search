/*
 * env.c
 *
 *  Created on: Feb 28, 2014
 *      Author: Volodymyr Varchuk
 */


#include <unistd.h>
#include <stdio.h>
#include "strfunc.h"
#include "env.h"

extern char **environ;

int printEnviron ()
{
	int i = 0;
	for (i = 0; environ[i]; i++ )
		printf ("%s\n", environ[i]);
	return i;
}

int getEnvList ( KeyValueList_t *KVList )
{
	int item_count = 0;
	int i = 0;
	for (i = 0; environ[i]; i++)
	{
		char *k = getKeyArgs_e(environ[i]);
		char *v = getValArgs_e(environ[i]);

//		printf ("key = %s, val = %s \n", k,v);

		addKeyVal( k, v, KVList );
	}
	item_count = i;
	return item_count;
}


int getEnvKeyList ( List_t *List )
{
	int item_count = 0;
	int i = 0;

	for (i = 0; environ[i]; i++)
	{
		//printf ("%s\n", environ[i]);
		addToList( getKeyArgs_e(environ[i]), List);
	}

	item_count = i;
	return item_count;
}
