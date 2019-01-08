# nXDownload (working title)
Download any file directly inside sdmc:/ on the Nintendo Switch

See markdown [here.](https://guides.github.com/features/mastering-markdown/)

This homebrew was intended to download specially NSPs & XCI without problems. But can download every file (really)!

## This tool is pretty basic:
#### 1 - Create a file .txt (from any device)
#### 2 - Add few lines with links with formatted arguments
####     - i.e.`<title-of-game/desciption> = <download/link/url_without_HTTPS>`
#### 3 - Power on your Switch, and select what file to download!

## NOTE:
This project is based on libcurl.

- use command line `pacman -S switch-curl` inside Msys2 to be able to compile this homebrew.

## Features & ideas
### As this tool is pretty new, and lot of my freetime goes to debugging, probably will take much time before finishing it.
- add possibility to read a custom URL from sdmc:/switch/nXDownload/url.txt = Done!
- add pssibility to read multiple URL from "url.txt"
- add support for plain URL's (meaning writing URL directly to the Switch!) = Done!
- add support for saving plain URL's (meaning written URL will be saved in a different file) = Done!
- add support for SDL2 projects!
- fix major bugs!
- Any idea you want i should add? Create a [pull request.](https://github.com/Dontwait00/nXDownload/pulls)
