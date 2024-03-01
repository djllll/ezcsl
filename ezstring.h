#ifndef _EZSTRING_H_
#define _EZSTRING_H_

#define EZSTR_OK 0
#define EZSTR_ERR 1

#define ezstr_ret_t char
#define ezstr_size_t int

extern ezstr_ret_t estrcat_s(char *_Dst, ezstr_size_t _DstSize, const char *_Src);
extern ezstr_ret_t estrcatc_s(char *_Dst, ezstr_size_t _DstSize, char _Src);
extern ezstr_ret_t estrcpy_s(char *_Dst, ezstr_size_t _DstSize, const char *_Src);
extern ezstr_ret_t estrlen_s(const char *_Str,ezstr_size_t _Size);
extern ezstr_size_t estrlen(const char *_Str);
extern ezstr_ret_t estrcmp(const char* _Str1,const char* _Str2);
extern ezstr_ret_t estrncmp(const char *_Str1, const char *_Str2, ezstr_size_t _Size);
extern char* estrtokc(char *_Str, char _Deli);

#endif