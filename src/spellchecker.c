#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/err.h"
#include "../header/file.h"
#include "../header/args.h"
#include "../header/test.h"
#include "../header/str.h"
#include "hashtable.h"

typedef struct Dict{
    char* ctx;
    size_t ctxs;
    char** words;
    int nwords;
}Dict;

typedef Dict Fixes;

Dict dict;
Fixes pre,suf;

void load_dict();
void destroy_dict();
void load_fixes();
void destroy_fixes();
int trim_word_from_begin(char* check);
int trim_word_from_last(char* check);
void check_spell(char** checks,int nchecks);

int
main(int argc,char* argv[]){
    if(argc<2){
        fprintf(stderr,"usage:%s <infiles>\n",argv[0]);
        exit(1);
    }
    timecost(load_dict(),1,1);
    timecost(load_fixes(),1,1);
    char* delim=" \n\t\f\r\b|/\\\"'~!@#$%^&*()_+=<>{}[];,.:?-0123456789";
    for(int i=1;i<argc;i++){
        FILE* fin=File_open(argv[i],"r");
        size_t fsize=File_size(fin);
        char* ctx=File_getctx(fin,fsize);
        int nchecks;
        char** checks=Args_get(ctx,delim,&nchecks);
        printf("\nchecking ||%s||\n",argv[i]);
        timecost(check_spell(checks,nchecks),1,1);
        printf("%s checked.\n\n",argv[i]);
        Args_free(&checks,nchecks);
        File_freectx(ctx,fsize);
        File_close(fin);
    }
    destroy_fixes();
    destroy_dict();
    return 0;
}

void
load_dict(){
    printf("loading dict...\n");
    Hashtable_init();
    FILE* fin=File_open("../res/american-english","r");
    size_t fsize=File_size(fin);
    char* ctx=File_getctx(fin,fsize);
    dict.ctx=ctx;
    dict.ctxs=fsize;
    char** words=Args_get(ctx,"\n",&(dict.nwords));
    for(int i=1;i<dict.nwords;i++){
        Hashtable_put(words[i]);
    }
    dict.words=words;
    File_close(fin);
    printf("dict loaded.\n");
}

void
load_fixes(){
    printf("\nloading prefixes/suffixes...\n");
    FILE* fin1=File_open("../res/prefixes.txt","r");
    FILE* fin2=File_open("../res/suffixes.txt","r");
    pre.ctxs=File_size(fin1);
    pre.ctx=File_getctx(fin1,pre.ctxs);
    pre.words=Args_get(pre.ctx,"\n",&pre.nwords);
    suf.ctxs=File_size(fin2);
    suf.ctx=File_getctx(fin2,suf.ctxs);
    suf.words=Args_get(suf.ctx,"\n",&suf.nwords);
    File_close(fin1);
    File_close(fin2);
    printf("prefixes/suffixes loaded.\n");
}

void 
destroy_fixes(){
    printf("\ndestroy prefixes/suffixes...\n");
    Args_free(&pre.words,pre.nwords);
    File_freectx(pre.ctx,pre.ctxs);
    Args_free(&suf.words,suf.nwords);
    File_freectx(suf.ctx,suf.ctxs);
    printf("destroyed.\n");
}

void
destroy_dict(){
    printf("\ndestroy dict...\n");
    Args_free(&dict.words,dict.nwords);
    File_freectx(dict.ctx,dict.ctxs);
    Hashtable_destroy();
    printf("destroyed.\n");
}

/*trim word according to fixes from beginning to end and 
 * check whether it's in dictionary.
 * return val:
 * 0-not found.
 * 1-found.
*/
int
trim_word_from_begin(char* check){
    for(int i=0;i<pre.nwords;i++){
        for(int j=0;j<pre.nwords;j++){
            ltrim(check,pre.words[j]);
            if(Hashtable_get(check)!=0){
                return 1;
            }
        }
    }
    for(int i=0;i<suf.nwords;i++){
        for(int j=0;j<suf.nwords;j++){
            rtrim(check,suf.words[j]);
            if(Hashtable_get(check)!=0){
                return 1;
            }
        }
    }
    return 0;
}

/*trim word according to fixes from end to beginning and 
 * check whether it's in dictionary.
 * return val:
 * 0-not found.
 * 1-found.
*/
int
trim_word_from_last(char* check){
    for(int i=0;i<pre.nwords;i++){
        for(int j=pre.nwords-1;j>=0;j--){
            ltrim(check,pre.words[j]);
            if(Hashtable_get(check)!=0){
                return 1;
            }
        }
    }
    for(int i=0;i<suf.nwords;i++){
        for(int j=suf.nwords-1;j>=0;j--){
            rtrim(check,suf.words[j]);
            if(Hashtable_get(check)!=0){
                return 1;
            }
        }
    }
    return 0;
}

void
check_spell(char** checks,int nchecks){
    char cp1[1024];
    char cp2[1024];
    int nwrong=0;
    printf("=======================================================================\n");
    for(int i=0;i<nchecks;i++){
        if(Hashtable_get(checks[i])==0){
            sprintf(cp1,"%s",checks[i]);
            sprintf(cp2,"%s",checks[i]);
            if(trim_word_from_begin(cp1)==0&&trim_word_from_last(cp2)==0){
                printf("%s\t",checks[i]);
                nwrong++;
            }
        }
    }
    printf("\n\n%d word%c checked.\n",nchecks,nwrong!=1?'s':' ');
    if(nwrong){
        printf("%d word%c above seem to be wrong.\n",nwrong,nwrong!=1?'s':' ');
    }else{
        printf("All words seem to be right.\n");
    }
    printf("=======================================================================\n");
}
