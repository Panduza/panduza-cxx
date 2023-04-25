FROM ubuntu:22.04

# Install Packages
RUN apt-get update && DEBIAN_FRONTEND=noninteractive TZ=Europe/Paris \
    apt-get -y install \
    python3 \
    python3-pip \
    libssl-dev \
    wget \
    git

WORKDIR /setup

RUN wget https://github.com/Kitware/CMake/releases/download/v3.25.0/cmake-3.25.0.tar.gz
RUN tar -xvf cmake-3.25.0.tar.gz
RUN cd cmake-3.25.0 && \
    ./bootstrap && make && make install

RUN pip3 install conan==1.58

WORKDIR /project


CMD /project/scripts/build.sh
