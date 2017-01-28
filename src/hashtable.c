#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "hashtable.h"
#include "../header/mem.h"
#include "../header/str.h"

#define MAX_HELEM_N 1024*1024
#define foreach(op) do{\
    for(int i=0;i<MAX_HELEM_N;i++){\
        for(WordList w=table[i],next;w!=NULL;w=next){\
            next=w->next;\
            op;\
        }\
    }\
}while(0)

static int factor=31;

typedef struct WordList{
    const char* word;
    struct WordList* next;
}*WordList;

static WordList table[MAX_HELEM_N];

static int
hash(const char* word){
    int v=0;
    char l;
    for(const char* p=word;*p!='\0';p++){
        if(isalpha(*p)){
            l=tolower(*p);
        }else{
            l=*p;
        }
        v=v*factor+l;
    }
    return v&(MAX_HELEM_N-1);
}

void
Hashtable_init(){
    for(int i=0;i<MAX_HELEM_N;i++){
        table[i]=NULL;
    }
}

void
Hashtable_destroy(){
    foreach(safer_free(w));
}

void
Hashtable_put(const char* word){
    int v=hash(word);
    WordList first=table[v];
    for(WordList w=first;w!=NULL;w=w->next){
        if(strcmp(w->word,word)==0){
            return;
        }
    }
    WordList new;
    safer_malloc(new,sizeof(*new));
    new->word=word;
    new->next=table[v];
    table[v]=new;
}

int
Hashtable_get(const char* word){
    int v=hash(word);
    WordList first=table[v];
    for(WordList w=first;w!=NULL;w=w->next){
        if(strcasecmp(w->word,word)==0){
            return 1;
        }
    }
    return 0;
}

char*
Hashtable_print(){
    char buf[1024*1024];
    foreach(do{
            sprintf(buf,"%s\n",w->word);
            append(buf);
            }while(0));
    return append(NULL);
}
