/* Includes */
#include <time.h>
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

#define Megabytes_in_Bytes	1048576
#define Kibibyte_in_Bytes	1024

int dlnow_Mb = 0;
int dltotal_Mb = 0;

// measure download speed
bool open_room = false;
bool once = false;
int dlspeed = 0;
int dl_curr = 0;
int	curr_sec = 0; // current second from system
int ticket = 0; // current (second + 1) from system

char global_f_tmp[512]; /* we need this global FILE variable for passing args */

/* Functions */
int older_progress(__attribute__((unused)) void *p, double dltotal, double dlnow, __attribute__((unused)) double ultotal, __attribute__((unused)) double ulnow) {
	return xferinfo((curl_off_t)dltotal, (curl_off_t)dlnow);
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
			
			if (strlen(global_f_tmp) != 0) curl_easy_setopt(curl, CURLOPT_USERPWD, global_f_tmp);
			
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

size_t dnld_header_parse(void *hdr, size_t size, size_t nmemb)
{
	const size_t	cb = size * nmemb;
	const char		*hdr_str = hdr;
	const char		*compareContent = "Content-disposition:";

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

int xferinfo(curl_off_t dltotal, curl_off_t dlnow) {

	dlnow_Mb = dlnow / Megabytes_in_Bytes;
	dltotal_Mb = dltotal / Megabytes_in_Bytes;
	
	// we need to create a separated room inside this "while" loop
	// so we can process this room once every so often
	curr_sec = time(0);
	
	if (open_room == false) {
		ticket = time(0) + 1;
		dl_curr = dlnow;
		open_room = true; // closing room
	}
	
	if (curr_sec >= ticket) {
		dlspeed = (dlnow - dl_curr) / Kibibyte_in_Bytes;
		open_room = false; // opening room
	}
	
	if (dltotal_Mb == 1) {
		printf("# DOWNLOAD: %" CURL_FORMAT_CURL_OFF_T " Bytes of %" CURL_FORMAT_CURL_OFF_T " Bytes | %3d Kb/s\r", dlnow, dltotal, dlspeed);
	} else if (dltotal_Mb > 1) {
		printf("# DOWNLOAD: %d Mb of %d Mb | %3d Kb/s\r", dlnow_Mb, dltotal_Mb, dlspeed);
	}
	
	if (dlnow == dltotal && dltotal > 0 && once == false) {
		printf("\n                                                                                "); // lol, is required
		once = true;
	}
	
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
			printf("\ntmpfile.txt : %s%s%s\n", CONSOLE_GREEN, buffer, CONSOLE_RESET);
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

static char	*selectLink(void)
{
	int		n = 0;			// just using n to count on how many links have founded; used for debugging
	int		nb_links = 0;	// Used to know how links in file
	char	**links = NULL;	// array f will contain the argument <download-link>
	char	**desc = NULL;	// array i will contain the argument <desc-of-download>
	char	*url = NULL;	// Used to save the url for download

	// get all links and desc from input.txt
	if ((nb_links = getLinksInFile("input.txt", &links, &desc)) != -1) {
		while(appletMainLoop()) {
			hidScanInput();
			u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

			printf("nb_links = %d\n", nb_links);

			printf("\x1b[1;1H%d links counter", n);
			printf("\x1b[5;1HStart = %s%s%s\n\nURL = %s%s%s", CONSOLE_BLUE, desc[n], CONSOLE_RESET, CONSOLE_GREEN, links[n], CONSOLE_RESET);

			printf("\x1b[10;1HPress D-PAD [->] to look forward\n");
			printf("Press D-PAD [<-] to look backward\n");
			printf("\nPress [A] to start download\n");
			printf("Press [B] to go back\n");
			
			if (kDown & KEY_DLEFT) {
				consoleClear();
				n--;
				if (n < 0) n = nb_links -1; // back to the last entry
			}
			
			if (kDown & KEY_DRIGHT) {
				consoleClear();
				n++;
				if (n >= nb_links) n = 0; // go to the first entry
			}
			
			if (kDown & KEY_B) {
				freeArray(links);
				freeArray(desc);
				return ((void *)-1);
			}
			
			if (kDown & KEY_A) {
				url = strdup(links[n]);
				break;
			}
			consoleUpdate(NULL);
		}

		freeArray(links);
		freeArray(desc);

		printf("\n# Done!");
		consoleUpdate(NULL);
	} else { // error opening dest
		printf("\n# %s%s%s", CONSOLE_RED, "There is no input.txt!", CONSOLE_RESET);
		consoleUpdate(NULL);
	}

	return (url);
}

static char	*getLink(const char *filename)
{
	char	*link = NULL;
	int		fd = 0;

	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		return (NULL);
	}

	get_next_line(fd, &link);

	close(fd);

	return (link);
}

static char	*getUrl(int a)
{
	char	*url = NULL;

	if (a == TMPFILE_TXT) {
		url = getLink("tmpfile.txt");
		printf("\n# %s%s%s", CONSOLE_YELLOW, "Founded argument/link to use", CONSOLE_RESET);
		if (url == NULL) {
			printf("\n# %s%s%s", CONSOLE_RED, "Error passing argument", CONSOLE_RESET);
			return (NULL);
		}
	} else {
		consoleClear();
		consoleUpdate(NULL);
		url = selectLink();
	}

	return (url);
}

static void	addExtension(char *filename)
{
	Result		rc;
	SwkbdConfig	skp;
	char		ext[21] = {0};

	printf("\n\n# %s%s%s", CONSOLE_YELLOW, "No extension founded for your filename from your link. Want to add it now?\n", CONSOLE_RESET);
	printf("\n  Press [A] to continue");
	printf("\n  Press [B] to skip\n");

	rc = swkbdCreate(&skp, 0);

	while(appletMainLoop()) {
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (kDown & KEY_A) {
			rc = swkbdCreate(&skp, 0);

			if (R_SUCCEEDED(rc)) {
				swkbdConfigMakePresetDefault(&skp);
				swkbdConfigSetGuideText(&skp, "Remember to add `.` like `[example].nsp`");

				// -1 to do not erase '\0'
				rc = swkbdShow(&skp, ext, sizeof(ext) -1);
				filename = realloc(filename, strlen(filename) + strlen(ext) + 1);

				strcat(filename, ext);
				swkbdClose(&skp);
				break;
			}
		}

		if (kDown & KEY_B) return;

		consoleUpdate(NULL);
	}
}

static bool	warnFileExist(const char *filename)
{
	printf("\n# %s%s%s%s%s", CONSOLE_YELLOW, "File (", filename, ") exist already, overwrite?\n", CONSOLE_RESET);
	printf("\nPress [A] to overwrite\nPress [B] to go back"); // little warning

	while (appletMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (kDown & KEY_A) {
			break;
		}

		if (kDown & KEY_B) {
			return (false);
		}

		consoleUpdate(NULL);
	}

	return (true);
}

static char	*getFileName(char *url)
{
	char	*tmp = NULL;

	tmp = strrchr(url, '/');

	if (tmp == NULL) {
		consoleClear();
		printf("%s%s%s%s\n", CONSOLE_RED, "URL bad formatted : ", url, CONSOLE_RESET);
		consoleUpdate(NULL);
		sleep(2);
		free(url);
		url = NULL;

		return (NULL);
	}

	return (strdup(tmp+1));
}

bool FILE_TRANSFER_HTTP(int a) {
	consoleClear();

	char	*url = NULL;
	char	*filename = NULL;

	// get url from file or keyboard
	url = getUrl(a);
	if (url == (void *) -1) {
		return (false);
	} else if (url != NULL) {
		// get filename
		filename = getFileName(url);
		if (filename == NULL) {
			return (false);
		}

		// add extension if missing
		if (strchr(filename, '.') == NULL) {
			addExtension(filename);
		}

		// Check if file exist before download
		if (isFileExist(filename) == false) {
			printf("\n# %s%s%s%s%s", CONSOLE_GREEN, "No (", filename, ") to overwrite", CONSOLE_RESET);
		} else { // the file exist
			if (warnFileExist(filename) == false) {
				free(url);
				free(filename);
				return (false);
			}
			remove(filename);
		}

		printf("\n# %s%s%s", CONSOLE_GREEN, "Starting downloading...\n", CONSOLE_RESET);
		printf("%s, %s\n", url, filename);

		downloadFile(url, filename);

		// release memory
		free(filename);
		free(url);
	}

	/*printf ("\nRemote name: %s\n", dnld_params.dnld_remote_fname);*/

	return (functionExit());
}

bool	inputUserOrPassword(bool userPass)
{
	bool		err = false;
	char		tmpout[256] = {0};
	SwkbdConfig	skp; // Software Keyboard Pointer
	Result		rc = swkbdCreate(&skp, 0);

	if (R_SUCCEEDED(rc)) {
		swkbdConfigMakePresetDefault(&skp);

		if (userPass == USER)
			{ swkbdConfigSetGuideText(&skp, "Insert Username"); }
		else if (userPass == PASSWORD)
			{ swkbdConfigSetGuideText(&skp, "Insert Password (if neccessary)"); }

		rc = swkbdShow(&skp, tmpout, sizeof(tmpout));

		// Username
		if (userPass == USER && tmpout[0] == 0) {
			err = true;
		} else if (userPass == USER && tmpout[0] != 0){
			sprintf(global_f_tmp, "%s", tmpout);
		}

		// Password
		else if (userPass == PASSWORD && tmpout[0] != 0) {
			strcat(global_f_tmp, ":");
			strcat(global_f_tmp, tmpout);
		}
	} else {
		err = true;
	}

	return err;
}
