#ifndef __UTILS_H_
#define __UTILS_H_


#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

extern unsigned int utils_atoh ( char *ap );

extern char * utils_strtok_r (char *s, const char *delim, char **save_ptr);
extern char * utils_strtok(char *s, const char *delim);
extern char * utils_strsep(char **stringp, const char *delim);

extern int utils_strtolower(char *s,int len);
extern int utils_strtoupper(char *s,int len);

extern char *strnstr(const char *str1,const char *str2,int len);
extern int  utils_bitcmp(const char *str1,const char *str2,int len);
extern char *utils_find_split_next(char *str,char split);
extern int utils_nsplit(char *str,char split,int n,char **pout);

extern int utils_ishex( int a);
extern int utils_hextoi(int hex);
extern int utils_ishexstr(const char *str);

extern int utils_isnum(const char *str);

extern int utils_atohb(char *str, char *buffer, int len);

extern char *utils_strcatul(char *str,uint32_t ul);
extern char *utils_strcatint(char *str,uint16_t ul);
extern char *utils_strcathex(char *str,uint32_t ul);
#ifdef __cplusplus	
}
#endif

#endif /*__UTILS_H_*/
