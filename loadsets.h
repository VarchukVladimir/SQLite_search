/*
 * loadsets.h
 *
 *  Created on: Feb 6, 2014
 *      Author: Volodymyr Varchuk
 */

#ifndef LOADSETS_H_
#define LOADSETS_H_

#include "strfunc.h"

int get_list (FILE *f, char *, List_t *);
int get_key_val_list (FILE *f, char *, KeyValueList_t *);

#endif /* LOADSETS_H_ */
