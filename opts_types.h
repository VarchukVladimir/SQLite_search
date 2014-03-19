/*
 * opts_types.h
 *
 *  Created on: Mar 18, 2014
 *      Author: Volodymyr Varchuk
 */

#ifndef OPTS_TYPES_H_
#define OPTS_TYPES_H_


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


#endif /* OPTS_TYPES_H_ */
