#include "http.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void version_to_str(struct Version ver, char** out_ver) {
    assert(out_ver && *out_ver);
    sprintf(*out_ver, "HTTP/%d.%d", ver.high, ver.low);
}

/*
  A B C D E F G H I J  K  L  M  N  O  P  Q
  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
  R   S  T  U  V  W  X  Y  Z  a  b  c  d  e  f  g  h
  17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33
  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y
  34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50
  z  0  1  2  3  4  5  6  7  8  9  +  / (pad) =
  51 52 53 54 55 56 57 58 59 60 61 62 63
*/

static char base64_table[255];
void base64_tableinit()
{
    int i, j;
    bzero(base64_table, 255);
    for (j = 0, i = 'A'; i <= 'Z'; i++) {/*填base64编码表*/
        base64_table[i] = j++;
    }
    for (i = 'a'; i <= 'z'; i++) {
        base64_table[i] = j++;
    }
    for (i = '0'; i<= '9'; i++) {
        base64_table[i] = j++;
    }
    base64_table['+'] = j++;
    base64_table['/'] = j++;
    base64_table['='] = j;
}

char *base64_decode(const char *cptr, char **rptr)
{
    char *res;
    int clen, len;
    static int init=0;
    if (cptr == NULL) {
        return NULL;
    }
    len = strlen(cptr);

    /*编了码的字符绝对是4的倍数*/
    if (len % 4) {
        return NULL;
    }
    if (!init) {
        init = 1;
        base64_tableinit();
    }
    clen = len/4;
    if ((res = malloc(len - clen)) == NULL) {
        return NULL;
    }
    for (*rptr = res; clen--; ) {
        //cptr后六位移动到最高位
        *res=base64_table[*cptr++] << 2&0xfc;
        //跟着下个字符低两位给res低两位
        *res++ |= base64_table[*cptr] >> 4;
        //填充res高四位其它清0
        *res = base64_table[*cptr++] << 4&0xf0;
        //字符前六位移到低六位取低四位
        *res++|= base64_table[*cptr] >> 2&0x0f;
        *res = base64_table[*cptr++] << 6;
        if(*cptr != '=') {
            *res++ |= base64_table[*cptr++];
        }
    }
    return *rptr;
}

char* base64_encode(const char *cptr, char **rptr) {
    char *res;
    int i, clen, len;
    len = strlen(cptr);
    clen = len/3;
    if (cptr == NULL || (res = malloc(clen+3*2 + len)) == NULL) {
        return NULL;
    }
    for (*rptr = res; clen--;) {
        *res++ = *cptr >> 2&0x3f;   //取ptr高6位放入res低6位
        //移动ptr最低2位到高6位然后清0其它位
        *res =*cptr++ << 4&0x30;
        *res++ |= *cptr >> 4;        //取ptr高4位给res低4位
        *res = (*cptr++&0x0f) << 2;  //取ptr低4位移动到高6位
        *res++ |= *cptr >> 6;        //取ptr高2位给res低2位*/
        *res++ = *cptr++ & 0x3f;
    }
    if(i = len%3) { //处理多余字符只有两种情况多一个或者两个字符
        if(i == 1) { //根据base64编码补=号
            *res++ = *cptr >> 2&0x3f;
            *res++ = *cptr<<4&0x30;
            *res++ = '=';
            *res++ = '=';
        }
        else {
            *res++ = *cptr >> 2&0x3f;
            *res = *cptr++ << 4&0x30;
            *res++ |= *cptr >> 4;
            *res++ = (*cptr&0x0f) << 2;
            *res++ = '=';
        }
    }
    *res = '='; //保证最后结位为=结束原因是因为base64里有为0的编码
    for (res = *rptr; *res != '='; res++) {
        *res="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="[*res];
    }

    rptr[0][strlen(*rptr)-1]='\0';    //去掉最后一个=号
    return *rptr;
}
