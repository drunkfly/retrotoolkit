#ifndef BAS2TAP_H
#define BAS2TAP_H

#ifndef BAS2TAP_EXTERN
#define BAS2TAP_EXTERN extern
#endif

#define MAXLINELENGTH 10240

#ifdef __cplusplus
extern "C" {
#endif

BAS2TAP_EXTERN void (*bas2tap_error)(int line, int stmt, const char* fmt, ...);
BAS2TAP_EXTERN int (*bas2tap_fgets)(char** basicIndex, int* basicLineNo);
BAS2TAP_EXTERN void (*bas2tap_output)(const void* dst, size_t length);

extern char bas2tap_ConvertedSpectrumLine[MAXLINELENGTH + 1];
int bas2tap_PrepareLine(char* LineIn, int FileLineNo, char** FirstToken);

void bas2tap_reset(void);
int bas2tap_main(void);

#ifdef __cplusplus
}
#endif

#endif
