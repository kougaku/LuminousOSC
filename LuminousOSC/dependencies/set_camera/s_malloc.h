#pragma once
#ifndef _S_MALLOC_H_
#define _S_MALLOC_H_

#define xalloc(t,n)      (t *)s_malloc((n),(unsigned)sizeof(t))
void *s_malloc(unsigned int n,size_t t);

#if 0
// 旧バージョンの対応用
char *xalloc_char(char data[]);
char *xalloc_char_se(char start[], char end[]);
#endif

#endif /* _S_MALLOC_H_ */
