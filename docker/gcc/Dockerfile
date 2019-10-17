FROM ubuntu:latest
RUN apt-get update && apt-get install -y \
    gcc \
    g++ \
    wget \
    make \
    git
RUN wget -qO- "https://cmake.org/files/v3.15/cmake-3.15.4-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local
RUN wget -qO- "http://www.fftw.org/fftw-3.3.8.tar.gz" | tar -xz -C /tmp && \
    cd /tmp/fftw*/ && \
    ./configure && \
    make && \
    make install