#include "common.h"
#include "string.h"
static inline char* get_time() {
    time_t timep;
    time(&timep);
    char *ret = asctime(gmtime(&timep));
    ret[strlen(ret) - 1] = '\0';
    return ret;
}

inline void init_log() {
    f_log = fopen("sandbox.log", "a+");
    if (f_log == NULL) {
        printf("Log File Create Error.\n");
        exit(-1);
    }

    fprintf(f_log, "[%s]Start\n", get_time());
}

inline void free_log() {
    fprintf(f_log ,"[%s]End\n", get_time());
    fclose(f_log);      
}

void logger(const char * msg) {
    fprintf(f_log, "%s\n", msg);
}