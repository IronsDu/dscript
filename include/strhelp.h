#ifndef _STRHELP_H
#define _STRHELP_H

const char* skipSpace(const char* input);
const char* skipChar(const char* input, char skipchar);
const char* endsearch(const char* input, char endchar);
const char* skipChar(const char* input);
const char* skipNum(const char* input);

void auxCopyStr(char* dest, const char* head, const char* end);

int getNumFromStr(const char* head, const char* end);

#endif
