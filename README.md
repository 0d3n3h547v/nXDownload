# nXDownload
Download any file directly inside `sdmc:/` on your Nintendo Switch
![First impact](https://i.imgur.com/M0Q7Jk9.jpg)

This homebrew was intended to download specially NSPs & XCI without problems. But it can download any file (really)!
*sort of tool downloader*

# Main Features!
> What you see is heavily based on `printf()` for making this *at least* readable & enjoyable in matrix world! :D
> 
> So, whatever you don't like, please report it!

## Write URLs from the Switch!
![Show writeURL](https://i.imgur.com/cAylb93.jpg)
## Select one of the links you injected!
![Download txt](https://i.imgur.com/jKqxnNm.jpg)

## Some more images!
![downloading](https://i.imgur.com/sugFPW6.jpg)
![installing](https://raw.githubusercontent.com/XorTroll/Goldleaf/master/Screenshots/Goldleaf.install3.jpg)
![done](https://i.imgur.com/yipKvHb.jpg)

## This tool is pretty basic:
### 1. Create a file .txt (from any device)
### 2. Add few lines with links with formatted arguments
####     - i.e.`<title-of-game/desciption> = <download/link/url_without_HTTPS>`
####     - Files are downloaded by default to `sdmc:/switch/nXDownload/`
####     - Remember that this tool requires *Direct Download Links* till i find someway to parse the download link from the webpage!
####     - File-sharing websites compatible with my tool are Mediafire, Uptobox, 1fichier, and any website that uses HTTP protocol!
### 3. Power on your Switch, and select what file to download!

## NOTE:
- When dowloading BIG files, is better to put your Switch in **Dock mode** so you will not risk to:
  - Burn your pixels!
  - No need to be afraid if the battery is low.

- This project is based on *libcurl*.
- This project changes the **heap size**, to get the software Keyboard feature (can have memory issues). In future, this could change.
- It doesn't support **HTTPS** protocol (meaning that once **[mbedtls library](https://tls.mbed.org/)** is officially ported to `libnx` we then can use **SSL** encyption)
- This project is under GPL-3 License.
- use command line `pacman -S switch-curl` inside Msys2 to be able to compile this homebrew.
- **I do not take any responsibility if it is used inappropriately or is damaging your console.**

### :star: Special thanks to (this is not in order):
- [4TU](https://fortheusers.org/) which are hosting many homebrew's/utilities and much more! A great thanks to them! 
- Every **Nintendo Switch warez scene community** which are hosting & releasing free NSPs and XCIs!
- The members of our server [NX++ Homebrew Discord](https://discord.gg/NErqFV4)! 
- Many thanks also to all the main characters who have contributed to the **development of homebrew** & **who pwn'ed the Nintendo Switch**!
- Major thanks at all the people behind the scene that have helped me to follow constantly and still helping me to learn C in better!

## Features & achievements
- [x] add possibility to read a custom URL from `sdmc:/switch/nXDownload/input.txt`
- [x] add possibility to read multiple URL from `input.txt`
- [x] add support for plain URL's (meaning writing URL directly to the Switch!)
- [x] add support for saving plain URL's (meaning written URL will be saved in a different file *temporaly* untill new written URL is encountered)
- [ ] add support for SDL2 projects!
- [ ] fix major bugs! (kinda)

Any idea you want i should add? Create a [pull request](https://github.com/Dontwait00/nXDownload/pulls)!

Or you have founded a major/minor bug? Come [here](https://github.com/Dontwait00/nXDownload/issues)!

### As this tool is pretty new, and lot of my freetime goes to debugging, probably will take much time before finishing it.
Thanks, and as always, have a nice day! :wave:
