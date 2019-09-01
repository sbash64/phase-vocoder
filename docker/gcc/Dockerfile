FROM ubuntu:latest
RUN apt-get update && apt-get install -y \
    gcc \
    g++ \
    wget \
    make \
    cmake
RUN wget -qO- "http://www.fftw.org/fftw-3.3.8.tar.gz" | tar -xz -C /tmp && \
    cd /tmp/fftw*/ && \
    ./configure && \
    make && \
    make install