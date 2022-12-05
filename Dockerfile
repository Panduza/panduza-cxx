FROM pza-cpp-img:latest

ARG USER_ID
ARG GROUP_ID

RUN groupadd -g $GROUP_ID dummy
RUN useradd -m -u $USER_ID -g $GROUP_ID -s /bin/bash dummy
RUN echo 'dummy:dummy' | chpasswd

USER dummy

WORKDIR /work