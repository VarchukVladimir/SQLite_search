/*
 * opts.c
 *
 *  Created on: Feb 5, 2014
 *      Author: Volodymyr Varchuk
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "opts.h"
#include "strfunc.h"


void init_options (Options_t *opts)
{
	opts->tMode = none;
	opts->iList = 0;
	opts->iMatch = 0;
	opts->pObject_path = NULL;
	opts->pKey = NULL;
	opts->pVal = NULL;
	opts->iCount = 0;
}

int checkMode (Options_t *opts, Operation_t new_op)
{
	if (opts->tMode == none)
	{
		opts->tMode = new_op;
		return 0;
	}
	else
	{
		return -1;
	}
}

int getopts (int argc, char **argv, Options_t *opts, KeyValueList_t *kvlist, List_t *list)
{
	int i = 0;
	int iFilterCount = 0;
	init_options  (opts);

	for ( i = 1; i < argc; i++)
	{
		if ((MY_STRNCMP(argv[i], "--add") == 0) || (MY_STRNCMP(argv[i], "-a") == 0))
		{
			if (checkMode (opts, add) != 0)
				return -1;

		}
		else if ((MY_STRNCMP(argv[i], "--delete") == 0) || (MY_STRNCMP(argv[i], "-d") == 0))
		{
			if (checkMode (opts, delete)  != 0 )
				return -1;
			if ((i+1 < argc) && (argv [i+1][0] != '-') )
			{
				char *buff = NULL;
				buff = (char *) malloc (sizeof (char ) * ( strlen (argv[i + 1]) + 1));
				strcpy (buff, argv[i+1]);
				i++;
			}
		}
		else if ((MY_STRNCMP(argv[i], "--find") ==0 ) || (MY_STRNCMP(argv[i], "-f") == 0))
		{
			if (checkMode (opts, find) != 0)
			{
				return -1;
			}
			iFilterCount = 0;
			while (((i + iFilterCount + 1) < argc) && (argv [i + iFilterCount + 1][0] != '-') )
			{
				iFilterCount++;
			}
			if (iFilterCount == 0)
				return -1;
			opts->pKey = (char **) malloc (sizeof (char *) * iFilterCount);
			opts->pVal = (char **) malloc (sizeof (char *) * iFilterCount);
			opts->iCount = iFilterCount;
			iFilterCount = 0;
			while (((i + iFilterCount + 1) < argc) && (argv [i + iFilterCount + 1][0] != '-') )
			{
				char *tempkey = NULL;
				char *tempval = NULL;

				tempkey = getKeyArgs(argv[i+iFilterCount + 1]);
				tempval = getValArgs(argv[i+iFilterCount + 1]);
				if (tempkey && tempval)
				{
					opts->pKey[iFilterCount] = tempkey;
					opts->pVal[iFilterCount] = tempval;
				}
				else
					return -1;
				iFilterCount++;
			}
			i = i + iFilterCount;
		}
		else if ((MY_STRNCMP(argv[i], "--struct") == 0) || (MY_STRNCMP(argv[i], "-s") == 0))
		{
			if (checkMode (opts, struct_create)  != 0 )
				return -1;
		}
		else if ((MY_STRNCMP(argv[i], "--list") == 0) || (MY_STRNCMP(argv[i], "-l") == 0))
		{
			opts->iList = 1;
		}
		else if ((MY_STRNCMP(argv[i], "--match") == 0) || (MY_STRNCMP(argv[i], "-m") == 0))
		{
			opts->iMatch = 1;
		}
		else
			return -1;
	}
	if (opts->iList == 1)
	{
		get_list(stdin, NULL, list);
	}
	else
	{
		getEnvKeyList (list);
	}
	return 0;
}
