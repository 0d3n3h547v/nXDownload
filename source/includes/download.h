#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

/* Includes */
#include <curl/curl.h>
#include <stdio.h>
#include <switch.h>

/* Defines */
#define TIMETYPE double
#define TIMEOPT CURLINFO_TOTAL_TIME
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3
#define STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES         6000

/* Structs */
CURL *curl; 
CURLcode res;

struct myprogress {
  TIMETYPE lastruntime;
  CURL *curl;
};

static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow) {
  struct myprogress *myp = (struct myprogress *)p;
  CURL *curl = myp->curl;
  TIMETYPE curtime = 0;
 
  curl_easy_getinfo(curl, TIMEOPT, &curtime);
 
  if((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL) {
    myp->lastruntime = curtime;
	printf("TOTAL TIME: %f \n", curtime);
  }
  
  printf("# DOWNLOAD: %" CURL_FORMAT_CURL_OFF_T " Bytes of %" CURL_FORMAT_CURL_OFF_T " Bytes\r", dlnow, dltotal);
  
  consoleUpdate(NULL);
  return 0;
}

static int older_progress(void *p, double dltotal, double dlnow) {
	return xferinfo(p, (curl_off_t)dltotal, (curl_off_t)dlnow);
}

/* Prototypes */
void curlInit(void);
void curlExit(void);
void nXDownloadUpdate(void);
void FILE_TRANSFER_FTP(char *url, char path[]);
void FILE_TRANSFER_HTTP(char *url, char path[], int a);
size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream);

#endif