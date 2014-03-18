/*
 * opts.h
 *
 *  Created on: Feb 5, 2014
 *      Author: volodymyr varchuk
 */

#ifndef OPTS_H_
#define OPTS_H_

#include "strfunc.h"

typedef enum {add, delete, find, struct_create, none} Operation_t;

typedef struct {
	Operation_t tMode;
	char *pObject_path;
	int iList;
	int iMatch;
	char **pKey;
	char **pVal;
	int iCount;
} Options_t;

int getopts (int, char **, Options_t*, KeyValueList_t *, List_t *);

#endif /* OPTS_H_ */
