/*
  定数の定義ファイル Ver.1.04
*/

/* TRUE,FALSEを定義 */
#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

/* 数学上の定数を定義 */
#ifndef M_PI
#define M_PI            3.14159265358979323846  /* pi */
#endif
#ifndef M_E
#define M_E             2.7182818284590452354   /* e */
#endif

/* ファイル名の最大長さ MAXPATHLENを定義 */
#ifndef         MAXPATHLEN
/* _WINDOWS の時は MAX_PATH が有効 */
#ifdef  MAX_PATH
#define MAXPATHLEN      MAX_PATH
#else
#define MAXPATHLEN      256
#endif
#endif

/* Windows特有の設定 */
#ifdef _WIN32
/* strcpy()等のWarning抑止 */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif
#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif
#endif

/* 機種別の設定 */
// MS-DOSの時： __MSDOS__
// Windowsの時： _WIN32,_CONSOLE
// Windowsの時： _WIN32,_WINDOWS
// FreeBSDの時： __unix__,__FreeBSD__
// linuxの時：   __unix__,__linux__
