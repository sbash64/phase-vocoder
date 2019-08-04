FROM ubuntu:latest
RUN apt-get -y update && apt-get install -y
RUN apt-get -y install clang wget
RUN wget -qO- "https://cmake.org/files/v3.15/cmake-3.15.1-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local
ENV CC=clang \
    CXX=clang++ \
    CMAKE_C_COMPILER=clang \
    CMAKE_CXX_COMPILER=clang++
RUN update-alternatives --config c++