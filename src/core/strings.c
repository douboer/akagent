#include "strings.h"

int string_strcmp(const char *s1,const char *s2)
{
    if(!s1){
        return (s1==s2) ? 0 : 1;
    }
    if(!s2){
        return 1;
    }

    for(; (*s1) == (*s2); ++s1, ++s2){
        if(*s1 == 0){
            return 0;
        }
    }

    return (*(const unsigned char *)s1) - (*(const unsigned char *)s2);
}

int string_find_delim(char *buffer ,char delim ,int size)
{
    char *ptr = buffer;

    while(*ptr != delim && (ptr - buffer < size)){
        ptr++;
    }

    return ptr - buffer;
}

void string_trim_blank(char *pStr)
{
    while(*pStr != '\0'){
        if((*pStr == ' ') &&
                (*(pStr+1) != '\\')){
            *pStr = '-';
        }else if((*pStr == '\n') ||
                (*pStr == '\\')){
            *pStr = '\0';
        }
        pStr++;
    }
}

void string_trim_zero(char *pStr){
    pStr[strlen(pStr)-1] = '\0';
}

void string_escape_slash(char *org ,int size)
{
    int i;
    
    for(i = 0 ;i < size ;i++){
        if((org[i] == '\\') && (
            (org[i+1] != 'u') || (org[i+1] != 'x'))){
            org[i] = ' '; 
        }
    } 
}

int string_string_to_hex(char *str, unsigned char *out)
{
    char *p = str;
    char high = 0, low = 0;
    int tmplen = strlen(p), cnt = 0;
    tmplen = strlen(p);
    if(tmplen % 2 != 0) return -1;
    while(cnt < tmplen / 2)
    {
        high = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
        low = (*(++ p) > '9' && ((*p <= 'F') || (*p <= 'f'))) ? *(p) - 48 - 7 : *(p) - 48;
        out[cnt] = ((high & 0x0f) << 4 | (low & 0x0f));
        p++;
        cnt ++;
    }
    return tmplen / 2;
}

char *string_string_to_lower(char *str)     
{
    char *ret = str;              

    while(*str != '\0'){   
        if((*str >= 'A') && (*str <= 'Z'))        {
            *str = *str +32;          
        }
        str++;
    }

    return ret;      
}

void string_itoa(int n,char *s)
{
    int i,sign;
    if((sign=n)<0)//记录符号
        n=-n;//使n成为正数
    i=0;
    do{
        s[i++]=n%10+'0';//取下一个数字
    }
    while ((n/=10)>0);//删除该数字
    if(sign<0)
        s[i++]='-';
    s[i]='\0';
}

char *string_trim_hspace(char *buf)
{
    char *ptr = buf;

    while(isspace(*ptr)){ptr++;}

    return ptr;
}

void string_trim_tab(char *buf)
{
    int i;

    for(i = 0 ;i < strlen(buf) ;i++){
        if(buf[i] == 9){
            buf[i] = ' ';
        }
    } 
}
