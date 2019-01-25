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

/* Structures */
CURL *curl; 
CURLcode res;

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
void curlInit(void);
void curlExit(void);
int nXDownloadUpdate(void);
void FILE_TRANSFER_FTP(char *url, char path[]);
bool FILE_TRANSFER_HTTP(char *url, char path[], int a);
void FILE_TRANSFER_HTTP_TEMPORALY(void);
int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow);
int older_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow);
size_t dnld_header_parse(void *hdr, size_t size, size_t nmemb, void *userdata);

#endif