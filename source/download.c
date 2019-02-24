/* Includes */
#include <stdio.h>
#include <errno.h>
#include <switch.h>
#include <stdlib.h> // used for alloc/malloc
#include <string.h> // used for mini tools
#include <unistd.h> // used for chdir()
#include <stdbool.h>// bool = 1 == true; 0 == false;
#include <curl/curl.h>

#include "includes/download.h"
#include "includes/menuCUI.h"
#include "includes/helper.h"

#define Megabytes_in_Bytes 1048576

int dlnow_Mb = 0;
int dltotal_Mb = 0;

/* Functions */
int older_progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow) {
	return xferinfo(p, (curl_off_t)dltotal, (curl_off_t)dlnow);
}

bool	downloadFile(const char *url, const char *filename)
{
	FILE				*dest = NULL;
	CURL				*curl = NULL;
	CURLcode			res = -1;
	struct myprogress	prog;

	consoleClear();

	curl = curl_easy_init();
	
	if (curl) {

		prog.lastruntime = 0;
		prog.curl = curl;

		dest = fopen(filename, "wb");
		if (dest == NULL) {
			perror("fopen");
		} else {
			curl_easy_setopt(curl, CURLOPT_URL, url);						// getting URL from char *url
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);					// useful for debugging
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); 			// skipping cert. verification, if needed
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); 			// skipping hostname verification, if needed
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, dest);				// writes pointer into FILE *destination
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, older_progress);
			curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &prog);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			res = curl_easy_perform(curl);									// perform tasks curl_easy_setopt asked before

			fclose(dest);
		}
	}

	curl_easy_cleanup(curl);												// Cleanup chunk data, resets functions.

	if (res != CURLE_OK) {
		printf("\n# Failed: %s%s%s\n", CONSOLE_RED, curl_easy_strerror(res), CONSOLE_RESET);
		return false;
	}

	return true;
}

size_t dnld_header_parse(void *hdr, size_t size, size_t nmemb, void *userdata) {
	
    const size_t cb = size * nmemb;
    const char *hdr_str = hdr;
    const char *compareContent = "Content-disposition:";

    /* Example: 
     * ...
     * Content-Type: text/html
     * Content-Disposition: filename=name1367; charset=funny; option=strange
     */
    if (strstr(hdr_str, compareContent)) {
        printf ("has c-d: %s\n", hdr_str);
    }
	
    return cb;
}

int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow) {
	struct myprogress *myp = (struct myprogress *)p;
	CURL *curl = myp->curl;
	TIMETYPE curtime = 0;
 
	curl_easy_getinfo(curl, TIMEOPT, &curtime);
	
	if((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL) {
		myp->lastruntime = curtime;
		//if (print == 0) printf("TOTAL TIME: %f                                  \n", curtime);
	}
	
	dlnow_Mb = dlnow / Megabytes_in_Bytes;
	dltotal_Mb = dltotal / Megabytes_in_Bytes;
	
	printf("# DOWNLOAD: %" CURL_FORMAT_CURL_OFF_T " Bytes of %" CURL_FORMAT_CURL_OFF_T " Bytes (%d Mb of %d Mb) \r", dlnow, dltotal, dlnow_Mb, dltotal_Mb);
	
	consoleUpdate(NULL);
	return 0;
}

void curlInit(void) {
	socketInitializeDefault();
}

void curlExit(void) {
	appletEndBlockingHomeButton();
	socketExit();
}

int nXDownloadUpdate(void) {
	consoleClear();

	if (downloadFile("http://projects00.000webhostapp.com/nXDownload.nro", "nXDownload_new.nro") == true) {
		// Remove old nro
		if (isFileExist("./nXDownload.nro") == true) {
			if (remove("./nXDownload.nro") == -1)
				printf("error code (%d)\n", errno);
		}
		//Rename new nro
		if (rename("nXDownload_new.nro", "nXDownload.nro") == -1) {
			printf("error code (%d)\n", errno);
			perror("rename()");
		}
	} else {
		printf("downloadFile() Failure !\n");
	}

	return (functionExit());
}

// Return true mean pop the keyboard to write a new link
// Return false mean use old link
static bool	useOldLink(void)
{
	bool		ret = false;
	FILE		*fp = NULL;
	char		*buffer = NULL;
	size_t		nbytes = 0;
	struct stat	st;

	// if tmpfile is empty, return true and pop the keyboard.
	stat("sdmc:/switch/nXDownload/tmpfile.txt", &st);
	if (st.st_size == 0)
		return (true);

	// if calloc fail, pop the keyboard
	buffer = (char *)calloc(sizeof(char), st.st_size);
	if (buffer == NULL) {
		return (true);
	}

	printf("\n# %s%s%s\n", CONSOLE_YELLOW, "'tmpfile.txt' exist already. Want to use old link or overwrite?", CONSOLE_RESET);

	fp = fopen("sdmc:/switch/nXDownload/tmpfile.txt", "r");
	if (fp != NULL) {
		nbytes = fread(buffer, sizeof(char), st.st_size, fp);

		if (nbytes > 0) {
			printf("\ntmpfile.txt :\n");
			printf("\n%s\n", buffer);
		}
		fclose(fp);
	}

	// free memory
	free(buffer);
	buffer = NULL;

	printf("\nPress [A] to continue");
	printf("\nPress [X] to use old link");

	while(appletMainLoop()) {
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (kDown & KEY_A) {
			ret = true;
			break;
		}
		if (kDown & KEY_X) {

			ret = false;
			break;
		}

		consoleUpdate(NULL);
	}

	return (ret);
}

// this function pop the keyboard and write the content in tmpfile.txt
static bool	inputNewLink(void)
{
	SwkbdConfig	skp; // Software Keyboard Pointer
	Result		rc;
	FILE		*fp = NULL;
	char		tmpoutstr[256] = {0};
	bool		err = false;

	rc = swkbdCreate(&skp, 0);
	if (R_SUCCEEDED(rc)) {
		swkbdConfigMakePresetDefault(&skp);
		swkbdConfigSetGuideText(&skp, "insert a http:// direct download link");

		rc = swkbdShow(&skp, tmpoutstr, sizeof(tmpoutstr));
		if (*tmpoutstr == 0) {
			err = true;
		}
		else {
			if ((fp = fopen("sdmc:/switch/nXDownload/tmpfile.txt", "wb")) != NULL) {
				fprintf(fp, "%s", tmpoutstr);
				fclose(fp);
			} else {
				err = true;
			}
			swkbdClose(&skp);
		}
	} else {
		err = true;
	}

	return (err);
}

bool FILE_TRANSFER_HTTP_TEMPORALY(void) {
	consoleClear();

	if (isFileExist("sdmc:/switch/nXDownload/tmpfile.txt") == true) {
		if (useOldLink() == false)
			return (false); // Return false mean no error
	}

	return (inputNewLink());
}

// This function open a file and return the content in a char *
static char	**getLinksInFile(const char *filename, int a)
{
	FILE	*fp = NULL;
	char	**array = NULL;
	char	*line = NULL;
	size_t	nb_nb_lines = 0;
	size_t	n = 0;
	char	desc[512] = {0}, link[512] = {0};

	fp = fopen(filename, "r");
	if (fp == NULL) {
		return (NULL);
	}

	// if error while reading file return NULL
	nb_lines = countLinesInFile(fp);
	if (nb_lines == -1) {
		return NULL;
	}

	// Alloc array whith number of line in file
	// x2 for description and link
	// [description]
	// [link]
	// +1 for NULL
	array = (char **)calloc((nb_lines * 2) + 1, sizeof(char *));
	if (array == NULL)
		return (NULL);


	// store description and link
	for (int i = 0, j = 1; getline(&line, &n, fp); i+=2; j+=2) {
		sscanf(line, "%s = %s", desc, link);
		array[i] = strdup(desc);
		array[j] = strdup(link);
	}

	free(line);
	line = NULL;

	return (array);
}

bool FILE_TRANSFER_HTTP(char *url, int a) {
	consoleClear();
	FILE *dest;
	char hn[50];
	
	// using tmp1 for passing url to another char array
	char tmp1[256];
	
	// buf here has the main function to extract the filename and use it as argument for fopen()
	char buf[256];
	
	// TODO : Write funtion to parse tmpfile.txt
	if (a == WITH_TMP_FILE) { // useful to pass data (url) between functions; but you need to write the tmpfile.txt with inside the url, before executing this function
		// needs to be cleaned up here after testing
		dest = fopen("tmpfile.txt", "r");
		while(!feof(dest)) fgets(tmp1, sizeof(tmp1), dest);
		url = strdup(tmp1);
		printf("\n# %s%s%s", CONSOLE_YELLOW, "Founded argument/link to use", CONSOLE_RESET);
		fclose(dest);
		
		if (url == NULL) {
			printf("\n# %s%s%s", CONSOLE_RED, "Error passing argument", CONSOLE_RESET);
			goto FINISH;
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
	
	/*// i think this is the only way to be sure to open a file in GOOD condition, without racing*/
	/*if ((dest = fopen("input.txt","r")) != NULL) {*/
		/*for (n = 0; n < 512; n++) {*/
			
			/*if (fscanf(dest, "%s = %s", i[n], f[n]) != 2) break;*/
			
			/*printf("\x1b[1;1H%d links counter", n);*/
			
			/*if (n == 511) printf("\n# %s%s%s", CONSOLE_RED, "Too many links that i can't handle! My MAX is 511 links!", CONSOLE_RESET);*/
			
			/*consoleUpdate(NULL);*/
		/*}*/
		
		/*// containing the number of how many links founded; used for resetting n*/
		/*int counter = n - 1;*/
		/*fclose(dest);*/
		
		/*if (i[n] == NULL || f[n] == NULL) { // error: no arguments founded as the file was open*/
			/*printf("\n# %s%s%s",*/
			/*CONSOLE_RED, */
			/*"There was an err while reading arguments:\nCheck that 'input.txt' at least is made like this:\n===============================\n<title> = <download/link/url>", */
			/*CONSOLE_RESET);*/
			
			/*goto FINISH;*/
		/*}*/
		
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
				return false;
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
		goto FINISH;
	}
	
	SKIP:
	
	if (a == 0 || a == 1 || a == 2) // this parses the url to mantain the last token, which (usually) contains the filename
	{
		
		char *token;
		token = strtok(tmp1, "/");
		strcpy(hn, token);
	
		while(token != NULL) {						// getting filename from link
			token = strtok(NULL, "/");				// get next token
			if (token != NULL) strcpy(buf, token);	// copy token to buf to save possible filename from url
		}
		
		int n;
		for (n = 0; buf[n] != '\0'; n++)  {
			if (buf[n] == '.') break;
		}
		
		void *haddr;
		if(R_FAILED(svcSetHeapSize(&haddr, 0x10000000))) goto FINISH;
		
		if (buf[n] == '\0') {
			printf("\n\n# %s%s%s", CONSOLE_YELLOW, "No extension founded for your filename from your link. Want to add it now?\n", CONSOLE_RESET);
			printf("\n  Press [A] to continue");
			printf("\n  Press [B] to skip\n");
		
			Result rc;
	
			SwkbdConfig skp;
			rc = swkbdCreate(&skp, 0);
	
			while(appletMainLoop()) {
				hidScanInput();
				u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		
				if (kDown & KEY_A) {
					rc = swkbdCreate(&skp, 0);
				
					if (R_SUCCEEDED(rc)) {
				
						char tmp[21] = {0};
						swkbdConfigMakePresetDefault(&skp);
						swkbdConfigSetGuideText(&skp, "Remember to add `.` like `[example].nsp`");
	
						rc = swkbdShow(&skp, tmp, sizeof(tmp));
						strcat(buf, tmp);
						swkbdClose(&skp);
						break;
					}
				}
		
				if (kDown & KEY_B) break;
		
				consoleUpdate(NULL);
			}
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
				return false;
			}
			
			consoleUpdate(NULL);
		}
		
		fclose(dest);
		dest = fopen(buf, "wb");
		
	}
	
	// Temporary fclose until refactor
	fclose(dest);

	printf("\n# %s%s%s", CONSOLE_GREEN, "Starting downloading...\n", CONSOLE_RESET);
	
	printf("%s, %s\n", url, buf);
	downloadFile(url, buf);

	free(url);
	
	FINISH:
	printf ("\nRemote name: %s\n", dnld_params.dnld_remote_fname);
	return (functionExit());
}
