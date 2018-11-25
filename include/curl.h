#ifndef _CURL_H_
#define _CURL_H_

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

/*
inside function fileDownload() int a options:

- 1 = generates a filename of numbers from 0 up to 999998;
- 2 = prints useful text for testing curl;
- 3 = 1 & 2;
- 0 = default;

*/

void fileDownload(char *url, char *path, char *filename, char *extension, int a) {
	chdir(path); // change dir
	FILE *dest;
	if (a == 1 || a == 3) // checks if int a is 0, or 3, or 4... If int a has got 1 value, then generates a random filename. If a has got 2 value, prints useful text for testing.
	{
		srand(time(NULL)); // sradicates the rand function
		int random = rand() % 999999; // generating the numbers
		sprintf(filename, "%d", random);
		strcat(filename, extension); // collocates the extension after the numbers
		dest = fopen(filename, "r"); // reading the file
	} else { // directly reading the file
	strcat(filename, extension); // collocates the extension after the filename
	dest = fopen(filename, "r"); // reading the file
	}
	
	if (fopen(filename, "r") != NULL) // check the file
	{
	    dest = freopen(filename, "wb", NULL); // if the file doesnt exist, reopens fopen with write previligies
	} else // the file exist
	{
		fprintf(stdout, "\n#fileDownload(): file exist already, overwrite? {A} yes, {B} return"); // little warning
		while (appletMainLoop()) {
			hidScanInput();
			u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			
			if (kDown & KEY_A) {
				break;
			}
			if (kDown & KEY_B) {
				fclose(dest); // as you decided to return, we need to close FILE *stream
				return;
			}
			
			gfxFlushBuffers();
            gfxSwapBuffers();
		}
	}
	
	consoleClear();
	CURL *curl; 
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT); // initializing/calling functions
	curl = curl_easy_init(); // initializing/calling functions
	
	fprintf(stdout, "starting libcurl...\n");
    
    if (curl)
	{
		appletBeginBlockingHomeButton(0);
		printf("\n\nURL = %s\nFile = %s%s\n\nDownload...\n", url, path, filename);
		curl_easy_setopt(curl, CURLOPT_URL, url); // getting URL from char *url
		
	    if (a == 2 || a == 3) {
			printf("\n* DEBUG MODE *\n");
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // useful for debugging
		}
		
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // following HTTP redirects
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L); // a max of setopt timeout for 20s
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // skipping cert. verification, if needed
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // skipping hostname verification, if needed
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback); // writes data into pointer
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, dest); // writes pointer into FILE *destination
        res = curl_easy_perform(curl); // perform tasks curl_easy_setopt asked before
		
		if (res != CURLE_OK) {
			printf("\nfailed: %s\n", curl_easy_strerror(res));
			fclose(dest);
			remove(filename);
		}
		
	    appletEndBlockingHomeButton();	
	    curl_easy_cleanup(curl); // always cleanup
        fclose(dest); // closing FILE *fp
	
	}
	
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return;
}

#endif