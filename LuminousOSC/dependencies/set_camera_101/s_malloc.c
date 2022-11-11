/************************************************************************
 s_malloc サイバラ版 malloc関係処理サブルーチン Ver.1.06c
 ************************************************************************
 経歴
 Ver.1.00(95/08/18)	s_string系より分離
 Ver.1.01(95/10/25)	xmallocのエラー表示を変更
 Ver.1.02(96/06/14)	memsetを追加(0で初期化する)
 Ver.1.03(98/07/21)	malloc->callocに変更
 Ver.1.04(99/07/07)	xalloc_char(NULL)対応
 Ver.1.05(08/09/12)	xalloc_char_se()追加,string.h非依存化
 Ver.1.06(11/01/18)	xalloc_char系統を分離,_WINDOWS対応
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"s_malloc.h"
#ifdef _WINDOWS
#include	<windows.h>
#endif

/* 安全対策したmalloc */
void *s_malloc(unsigned int n, size_t t)
{
  void *point;
  point = calloc (n, t);
  if (point == NULL)
    {
#ifdef _WINDOWS
      MessageBox(NULL,"s_malloc",
		 "s_malloc : Insufficient Memory. Stop.",MB_OK);
#else
#if 01
      fputs("s_malloc : Insufficient Memory. Stop.\n", stderr);
#else
      // printfの方がプログラムサイズがでかくなるが・・・意味ないかも
      printf("s_malloc : Insufficient Memory. (allocate %d x %d bytes)\n",
	     t, n);
#endif
#endif
      exit (2);
    }
#if 0
  printf("Allocated Memory. (allocate %d x %d bytes)\n", t, n);
#endif
  memset(point, 0, n * t);	/* 0で初期化 */
  return(point);
}
