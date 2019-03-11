#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

/* Includes */
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <switch.h>

/* Defines */
#define TIMETYPE double
#define TIMEOPT CURLINFO_TOTAL_TIME
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3
#define STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES         6000

#define USER		0
#define PASSWORD	1

/* Structures */
struct myprogress {
  TIMETYPE lastruntime;
  CURL *curl;
};

struct a {
    char        dnld_remote_fname[512];
    char        dnld_url[512]; 
    FILE        *dnld_stream;
    FILE        *dbg_stream;
    uint64_t    dnld_file_sz;
} dnld_params;


/* Prototypes */

// init curl sockets
void curlInit(void);
void curlExit(void);

int nXDownloadUpdate(void);
void FILE_TRANSFER_FTP(char *url, char path[]);
bool FILE_TRANSFER_HTTP(int a);
bool FILE_TRANSFER_HTTP_TEMPORALY(void);
int xferinfo(curl_off_t dltotal, curl_off_t dlnow);
int older_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow);
size_t dnld_header_parse(void *hdr, size_t size, size_t nmemb);
/* this func's is used for auth in case ftp/http requires */
bool inputUserOrPassword(bool userPass);

#endif
