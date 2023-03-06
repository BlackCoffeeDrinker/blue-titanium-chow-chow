FROM ubuntu:20.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    bison \
    flex \
    curl \
    gcc \
    g++ \
    make \
    texinfo \
    zlib1g-dev \
    g++ \
    unzip \
    curl \
    git \
    cmake \
    ninja-build

WORKDIR /code
RUN git clone https://github.com/andrewwutw/build-djgpp.git && \
    cd build-djgpp && \
    ./build-djgpp.sh 10.2.0 && \
    cd .. && \
    rm -rf build-djgpp

ENV PATH /usr/local/djgpp/i586-pc-msdosdjgpp/bin/:$PATH
ENV GCC_EXEC_PREFIX /usr/local/djgpp/lib/gcc/
