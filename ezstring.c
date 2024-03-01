
#include "ezstring.h"
#include <stddef.h>

#define overflow(s, lmt) \
    if (++s >= lmt)      \
        return EZSTR_ERR;

ezstr_ret_t estrcat_s(char *_Dst, ezstr_size_t _DstSize, const char *_Src);
ezstr_ret_t estrcatc_s(char *_Dst, ezstr_size_t _DstSize, char _Src);
ezstr_ret_t estrcpy_s(char *_Dst, ezstr_size_t _DstSize, const char *_Src);
ezstr_ret_t estrlen_s(const char *_Str,ezstr_size_t _Size);
ezstr_size_t estrlen(const char *_Str);
ezstr_ret_t estrcmp(const char* _Str1,const char* _Str2);
ezstr_ret_t estrncmp(const char *_Str1, const char *_Str2, ezstr_size_t _Size);
char* estrtokc(char *_Str, char _Deli);


ezstr_ret_t estrcat_s(char *_Dst, ezstr_size_t _DstSize, const char *_Src)
{
    ezstr_size_t s = 0;
    char *tmp = _Dst;
    while (*tmp != 0) {
        overflow(s, _DstSize);
        *tmp++;
    }
    while (*_Src != 0) {
        overflow(s, _DstSize);
        *tmp++ = *_Src++;
    }
    *tmp = 0;
    return EZSTR_OK;
}
ezstr_ret_t estrcatc_s(char *_Dst, ezstr_size_t _DstSize, char _Src)
{
    ezstr_size_t s = 0;
    char *tmp = _Dst;
    while (*tmp != 0) {
        overflow(s, _DstSize);
        *tmp++;
    }
    if(s+1>=_DstSize){
        return EZSTR_ERR;
    }
    *tmp = _Src;
    *(tmp+1) = 0; 
    return EZSTR_OK;
}

ezstr_ret_t estrcpy_s(char *_Dst, ezstr_size_t _DstSize, const char *_Src)
{
    ezstr_size_t s = 0;
    while (*_Src != 0) {
        overflow(s, _DstSize);
        *_Dst++ = *_Src++;
    }
    return EZSTR_OK;
}

ezstr_ret_t estrlen_s(const char *_Str, ezstr_size_t _Size)
{
    ezstr_size_t s = 0;
    while (*_Str != 0) {
        if (++s >= _Size)
            return _Size;
        _Str++;
    }
    return s;
}


ezstr_size_t estrlen(const char *_Str)
{
    ezstr_size_t s = 0;
    while (*_Str != 0) {
        s++;
        _Str++;
    }
    return s;
}

ezstr_ret_t estrcmp(const char* _Str1,const char* _Str2){
    while (*_Str1 == *_Str2)
	{
		if (*_Str1 == '\0')
		{
			return EZSTR_OK;
		}
		_Str1++;
		_Str2++;
	}
    return EZSTR_ERR;
}

ezstr_ret_t estrncmp(const char *_Str1, const char *_Str2, ezstr_size_t _Size)
{
    ezstr_size_t s = 0;
    while (*_Str1 == *_Str2) {
        if (++s >= _Size) {
            return EZSTR_OK;
        }
        _Str1++;
        _Str2++;
    }
    return EZSTR_ERR;
}


char *estrtokc(char *_Str, char _Deli)
{
    static char *tmp = NULL;
    if (_Str == NULL) {
        _Str = tmp;
    }
    char *search = _Str;
    while (*search != _Deli) {
        if (*search == 0) {
            return NULL;
        }
        search++;
    }
    *search = 0;
    tmp = search + 1;
    return _Str;
}