/*
 * env.h
 *
 *  Created on: Feb 28, 2014
 *      Author: Volodymyr Varchuk
 */

#ifndef ENV_H_
#define ENV_H_

#define PATH_INFO_NAME "PATH_INFO"

int printEnviron ();
int getEnvList ( KeyValueList_t *);
int getEnvKeyList ( List_t *);

#endif /* ENV_H_ */
