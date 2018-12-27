# nXDownload
Download any file directly inside sdmc:/ on the Nintendo Switch

Coming big features, like downloading NSP & XCI games from a simple file containing multiple download links. And, if you're downloading a RAR/ZIP compression, it will automatically unrar/unzip it! (A new way to offer piracy to your Switch!).

## This tool is pretty basic:
#### 1 - Create a file .txt
#### 2 - Add few lines with links
#### 3 - Power on you're Switch, and start download you're file!

## NOTE:
This project uses my custom implementation of libcurl. You can use my own "customCurl.h" header, without problems! Be sure at least to credit!

- use command line "pacman -S switch-curl" inside Msys to be able to compile this program.

## Features & ideas
### As this tool is pretty new, and lot of my freetime goes to debugging, probably will take much time before finishing it.
- add possibility to read a custom URL from sdmc:/switch/nXDownload/url.txt = Done!
- add pssibility to read multiple URL from "url.txt"
- add support for plain URL's (meaning writing URL directly to the Switch!)
- add support for saving plain URL's (meaning written URL will be saved in a different file)
- add support for SDL2 projects!
- fix major bugs!
- Any idea you want i should add? Create a pull request: https://github.com/Dontwait00/nXDownload/pulls
