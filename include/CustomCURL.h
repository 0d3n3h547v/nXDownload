#ifndef _CUSTOMCURL_H_
#define _CUSTOMCURL_H_

/* includes */
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // used for chdir()

#define TIMETYPE double
#define TIMEOPT CURLINFO_TOTAL_TIME
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3
#define STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES         6000

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
	printf("TOTAL TIME: %f \r", curtime);
  }
  
  printf("DOWNLOAD: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T "\r", dlnow, dltotal);
  
  consoleUpdate(NULL);
  return 0;
}

static int older_progress(void *p, double dltotal, double dlnow) {
  return xferinfo(p, (curl_off_t)dltotal, (curl_off_t)dlnow);
}

// this "write_callback" probably is used for writing into FILE *destinaton (FILE *stream)
size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

/* initializing main sources */
void curlInit()
{
	socketInitializeDefault();
	curl = curl_easy_init();
}

/* terminate main sources */
void curlExit()
{
	appletEndBlockingHomeButton();
	socketExit();
}

/* used to update nXDownload */
void nXDownloadUpdate()
{
	char url[] = "http://projects00.000webhostapp.com/nXDownload.nro";
	
	chdir("sdmc:/switch/nXDownload/");
	FILE *dest;
	dest = fopen("nXDownload_newer.nro", "w");
	
	if (curl)
	{
		appletBeginBlockingHomeButton(0);
		printf("\n\n* URL = %s\n\n* Download...\n", url);
		curl_easy_setopt(curl, CURLOPT_URL, url); // getting URL from char *url
		printf("\n* DEBUG MODE\n");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // useful for debugging
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // following HTTP redirects
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L); // a max of setopt timeout for 20s
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // skipping cert. verification, if needed
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // skipping hostname verification, if needed
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, dest); // writes pointer into FILE *destination
        res = curl_easy_perform(curl); // perform tasks curl_easy_setopt asked before
		
		if (res != CURLE_OK) {
			printf("\n# Failed: %s", curl_easy_strerror(res));
			fclose(dest);
		}
		
	    appletEndBlockingHomeButton();
		fclose(dest); // closing FILE *stream
	}
	
	remove("sdmc:/switch/nXDownload/nXDownload.nro"); 
	rename("sdmc:/switch/nXDownload/nXDownload_newer.nro", "sdmc:/switch/nXDownload/nXDownload.nro"); // Wow. this is like reinxtool source! xD
	return;
}

/* \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
            main function of the tool;
       is like touching the eyeball of the head

Thats why i need to parse the entire body of the header
         into a separated/external file

TODO:
- divide the source to actually understand something
- proper errors handling
- it just needs a new cleanup
- needs even to test errors

i think this tool, is pretty stable, untill gets some random crash from unexpected error
*/
void fileDownload(char *url, char path[], int a)
{
	consoleClear();
	FILE *dest;
	chdir(path);
	
	// using tmp1 for passing url to another char array
	char tmp1[256];
	
	// buf here has the main function to extract the filename and use it as argument for fopen()
	char buf[256];
	struct myprogress prog;
	
	if (a == 2) { // useful to pass data (url) between functions; but you need to write the tmpfile.txt with inside the url, before executing this function
	
		// needs to be cleaned up here after testing
		dest = fopen("tmpfile.txt", "r");
		while(!feof(dest)) fgets(tmp1, sizeof(tmp1), dest);
		url = strdup(tmp1);
		printf("\n# %s%s%s", CONSOLE_YELLOW, "Founded argument/link to use", CONSOLE_RESET);
		fclose(dest);
		
		// idk even what error is going to say to me :/
		if (url == NULL) {
			printf("\n# %s%s%s", CONSOLE_RED, "Error passing argument", CONSOLE_RESET);
			return;
		}
		
		// if int a equivale with 2, and everything was okay, the code below is skipped
		goto SKIP; 
	}
	
	consoleClear();
	
	// array i will contain the argument <desc-of-download>
	char i[512][512];
	
	// array f will contain the argument <download-link>
	char f[512][512];
	
	// just using n to count on how many links have founded; used for debugging
	int n;
	
	consoleUpdate(NULL);
	
	// i think this is the only way to be sure to open a file in GOOD condition, without racing
	if ((dest = fopen("input.txt","r")) != NULL) {
		for (n = 0; n < 512; n++) {
			
			if (fscanf(dest, "%s = %s", i[n], f[n]) != 2) break;
			
			printf("\x1b[1;1H%d links counter", n);
			
			if (n == 511) printf("\n# %s%s%s", CONSOLE_RED, "Too many links that i can't handle! My MAX is 511 links!", CONSOLE_RESET);
			
			consoleUpdate(NULL);
		}
		
		// containing the number of how many links founded; used for resetting n
		int counter = n - 1;
		fclose(dest);
		
		if (i[n] == NULL || f[n] == NULL) { // error: no arguments founded as the file was open
			printf("\n# %s%s%s",
			CONSOLE_RED, 
			"There was an err while reading arguments:\nCheck that 'input.txt' at least is made like this:\n===============================\n<title> = <download/link/url>", 
			CONSOLE_RESET);
			
			return;
		}
		
		while(appletMainLoop()) {
			hidScanInput();
			u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			
			printf("\x1b[1;1H%d links counter", n);
			printf("\x1b[5;1HStart = %s%s%s\n\nURL = %s%s%s", CONSOLE_BLUE, i[n], CONSOLE_RESET, CONSOLE_GREEN, f[n], CONSOLE_RESET);
			printf("\x1b[10;1HPress D-PAD [->] to look forward\n");
			printf("Press D-PAD [<-] to look backward\n");
			printf("\nPress [A] to start download\n");
			printf("Press [B] to go back\n");
			
			if (kDown & KEY_DLEFT) {
				consoleClear();
				n--;
				if (n < 0) n = counter;
			}
			
			if (kDown & KEY_DRIGHT) {
				consoleClear();
				n++;
				if (n > counter) n = 0;
			}
			
			if (kDown & KEY_B) {
				curlExit();
				return;
			}
			
			if (kDown & KEY_A) {
				sprintf(tmp1, "%s", f[n]);
				break; 
			}
			
			consoleUpdate(NULL);
		}
			
		printf("\n# Done!");
		consoleUpdate(NULL);
		
		// passing url to char* for libcurl; Now we need tmp1 to parse the code to get the filename
		url = strdup(tmp1);
		fclose(dest);
		
	} else { // error opening dest
		printf("\n# %s%s%s", CONSOLE_RED, "There is no input.txt!", CONSOLE_RESET);
		consoleUpdate(NULL);
		return;
	}
	
	SKIP:
	
	if (a == 0 || a == 1 || a == 2) { // this parses the url to mantain the last token, which (usually) contains the filename
		
		char *token;
		token = strtok(tmp1, "/");
	
		while(token != NULL) {
			token = strtok(NULL, "/");
			if (token != NULL) strcpy(buf, token);
		}
		
	}
	
	dest = fopen(buf, "r");
	
	if (dest == NULL) {
		
		printf("\n# %s%s%s%s%s", CONSOLE_GREEN, "No (", buf, ") to overwrite", CONSOLE_RESET);
		dest = fopen(buf, "wb");
	
	} else { // the file exist
		printf("\n# %s%s%s%s%s", CONSOLE_YELLOW, "File (", buf, ") exist already, overwrite?\n", CONSOLE_RESET);
		printf("\nPress [A] to continue\nPress [B] to go back"); // little warning
		
		while (appletMainLoop()) {
			hidScanInput();
			u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			
			if (kDown & KEY_A) {
				break;
			}
			
			if (kDown & KEY_B) {
				fclose(dest); // closing early
				return;
			}
			
			consoleUpdate(NULL);
		}
		
		fclose(dest);
		dest = fopen(buf, "wb");
		
	}
	
	printf("\n# %s%s%s", CONSOLE_GREEN, "Starting downloading...\n", CONSOLE_RESET);
	
    if (curl)
	{
		appletBeginBlockingHomeButton(0);
		prog.lastruntime = 0;
		prog.curl = curl;
		printf("\n\n# URL = %s\n* File = %s%s\n\n* Download...\n", url, path, buf);
		curl_easy_setopt(curl, CURLOPT_URL, url); // getting URL from char *url
		printf("\n* DEBUG MODE\n");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // useful for debugging
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // following redirects
		curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTP); // following only HTTP redirects
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // skipping cert. verification, if needed
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // skipping hostname verification, if needed
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, dest); // writes dat into FILE *destination
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, older_progress);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        res = curl_easy_perform(curl); // perform tasks curl_easy_setopt asked before
		
		if (res != CURLE_OK) {
				printf("\n# Failed: %s", curl_easy_strerror(res));
				fclose(dest);
		    }
		
		free(url);
		appletEndBlockingHomeButton(); // idk why doesnt work this syscall
		fclose(dest);
	
	}
	
	return; // terminate function; Congrats, you did it!
}

#endif