FROM archlinux:base-devel-20231001.0.182270

RUN echo -e "\n[multilib]\nInclude = /etc/pacman.d/mirrorlist" | sudo tee -a /etc/pacman.conf
RUN pacman -Syu --noconfirm
RUN pacman -Sy --noconfirm \
    git \
    ninja \
    cmake \
    python \
    python-pip \
    wget \
    libunwind \
    wine \
    gcc \
    clang

RUN python -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"
RUN pip install conan==2.0.13
RUN conan profile detect

RUN useradd -m mingw && echo "mingw:password" | chpasswd
RUN echo "mingw ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/myuser
USER mingw
WORKDIR /home/mingw
RUN git clone https://aur.archlinux.org/paru.git
RUN cd paru && makepkg -si --noconfirm
RUN paru -S --noconfirm mingw-w64-cmake mingw-w64-zstd mingw-w64-zlib

USER root
RUN ln -s /usr/x86_64-w64-mingw32/lib/librpcrt4.a /usr/x86_64-w64-mingw32/lib/libRpcRT4.a
RUN userdel -r mingw

RUN git config --system --add safe.directory '*'
