##
## Makefile
## GNU Makefile
## Version : 1.0
## Project : opencv/machine-learning
##
## Created by Teppei Kobayashi
## Last Modified 2016/10/20
##


make: sampling-image.cpp feature-vector.cpp learning-data.cpp export-result.cpp
	g++ sampling-image.cpp -Wall -o 1-sampling-image `pkg-config --cflags opencv2` `pkg-config --libs opencv2`
	g++ feature-vector.cpp -Wall -o 2-feature-vector `pkg-config --cflags opencv2` `pkg-config --libs opencv2`
	g++ learning-data.cpp -Wall -o 3-learning-data `pkg-config --cflags opencv2` `pkg-config --libs opencv2`
	g++ export-result.cpp -Wall -o 4-export-result `pkg-config --cflags opencv2` `pkg-config --libs opencv2`

sampling-image: sampling-image.cpp
	g++ sampling-image.cpp -Wall -o sampling-image `pkg-config --cflags opencv2` `pkg-config --libs opencv2`

feature-vector: feature-vector.cpp
	g++ feature-vector.cpp -Wall -o feature-vector `pkg-config --cflags opencv2` `pkg-config --libs opencv2`

learning-data: learning-data.cpp
	g++ learning-data.cpp -Wall -o learning-data `pkg-config --cflags opencv2` `pkg-config --libs opencv2`

export-result: export-result.cpp
	g++ export-result.cpp -Wall -o export-result `pkg-config --cflags opencv2` `pkg-config --libs opencv2`

clean:
	rm -f sampling-image feature-vector learning-data export-result
	rm -f 1-sampling-image 2-feature-vector 3-learning-data 4-export-result

cleanall:
	rm -f examples/*.csv sample/*.png resources/*.mp4
	rm -f sampling-image feature-vector learning-data export-result
	rm -f 1-sampling-image 2-feature-vector 3-learning-data 4-export-result
