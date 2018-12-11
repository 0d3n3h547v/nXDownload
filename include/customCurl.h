#ifndef _CUSTOMCURL_H_
#define _CUSTOMCURL_H_

/* includes */
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // used for chdir()

// this "write_callback" probably is used for writing into FILE *destinaton (FILE *stream)
size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

void fileDownload(char *url, char path[], char filename[], char extension[], int a)
{
	FILE *dest;
	char buf[256];
	
	if (url == NULL) {
		chdir("sdmc:/switch/nXDownload/");
		dest = fopen("url.txt", "r");
	
		if (dest == NULL) {
			perror("\n# Error dest: ");
			goto SKIP;
		}
	
		// fix this shit!!!!!!
		if (dest)
		{
			printf("\n# file opened...");
			char tmp1[150];
			
			while(!feof(dest)) fgets(tmp1, 150, dest);
			printf("\n# Done!");
			url = tmp1;
			printf("\n# Done twice!");
			fclose(dest);
		}
	}
	
	SKIP:
	chdir(path); // change dir
	
	if (a == 0) {
		snprintf(buf, sizeof(buf), "%s.%s", filename, extension);
	}
	
	if (a == 1) {
		srand(time(NULL)); // sradicates the rand function
		int random = rand() % 99999; // generating the numbers
		snprintf(buf, sizeof(buf), "%d.%s", random, extension);
	}
	
	if (buf == NULL) { 
		perror("\n# Failed");
		goto EXIT;
	}
	
	dest = fopen(buf, "r");
	
	if (dest == NULL) {
		fclose(dest);
		perror("\n# Failed buf");
		dest = fopen(buf, "wb");
	} else { // the file exist
		printf("\n# File %s exist already, overwrite? [A] Continue, [B] Exit", buf); // little warning
		
		while (appletMainLoop()) {
			hidScanInput();
			u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			
			if (kDown & KEY_A) {
				break;
			}
			if (kDown & KEY_PLUS) goto EXIT;
			
			gfxFlushBuffers();
            gfxSwapBuffers();
		}
		
		fclose(dest);
		dest = fopen(buf, "wb");
		
	}
	
	CURL *curl; 
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT); // initializing/calling functions
	curl = curl_easy_init(); // initializing/calling functions
	
	printf("\n# Starting curl...\n");
    
    if (curl)
	{
		appletBeginBlockingHomeButton(0);
		printf("\n\n* URL = %s\n* File = %s%s\n\n* Download...\n", url, path, buf);
		curl_easy_setopt(curl, CURLOPT_URL, url); // getting URL from char *url
		printf("\n* DEBUG MODE\n");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // useful for debugging
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // following HTTP redirects
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L); // a max of setopt timeout for 20s
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // skipping cert. verification, if needed
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // skipping hostname verification, if needed
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback); // writes data into pointer
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, dest); // writes pointer into FILE *destination
        res = curl_easy_perform(curl); // perform tasks curl_easy_setopt asked before
		
		if (res != CURLE_OK) {
			printf("\n# Failed: %s", curl_easy_strerror(res));
			fclose(dest);
		}
		
	    appletEndBlockingHomeButton();
		fclose(dest); // closing FILE *stream
		
		printf("\n\n# Exit by pressing [+] or [HOME]");
		while (appletMainLoop()) {
			hidScanInput();
			u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			if (kDown & KEY_PLUS) goto EXIT;
			gfxFlushBuffers();
			gfxSwapBuffers();
		}
		
		curl_global_cleanup();
	    curl_easy_cleanup(curl); // always cleanup
	
	}
	
	EXIT:
		gfxExit();
		socketExit();
}

#endif