## ----------------------------------------------------------------------------
## file    : Makefile
## brief   : Makefile for OpenCV project.
## module  : image-processing
##
## author  : Teppei Kobayashi <teppei.ts@gmail.com>
## date    : 2016/11/25
## ----------------------------------------------------------------------------

matrix-video: matrix-video.cpp
	clang++ -std=c++11 -Wall -Wextra matrix-video.cpp -o matrix-video `pkg-config --cflags opencv3` `pkg-config --libs opencv3` -I${HOME}/lib/mylibs/include -L${HOME}/lib/mylibs/lib -lprogress

matrix-image: matrix-image.cpp
	clang++ -std=c++11 -Wall -Wextra matrix-image.cpp -o matrix-image `pkg-config --cflags opencv3` `pkg-config --libs opencv3`

clean:
	rm -f matrix-image matrix-video
