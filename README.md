# OpenSBV
Open source single board vision library

### Dependencies

Required:
* [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) - library to work with images
* [libserial](https://github.com/wjwwood/serial) - serial read write library
* [OpenAL](https://github.com/kcat/openal-soft) - audio working library
* (Optional)[OpenCV](https://github.com/opencv/opencv) - required for tests

### Install

Get the code:

    git clone https://github.com/harutk7/opensbv

Cmake

    cd opensbv && mkdir build && cd build && cmake ../

Make

    make
    
Install

    sudo make install && sudo ldconfig