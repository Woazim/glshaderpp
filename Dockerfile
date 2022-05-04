FROM ubuntu:jammy as commonbase

LABEL Name=glshaderPP-build Version=1.0.0

RUN apt-get update && \
    apt-get install -y gcc-10 libstdc++-10-dev wget && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 10 && \
    update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-10 10 && \
    update-alternatives --install /usr/bin/gcov-tool gcov-tool /usr/bin/gcov-tool-10 10 && \
    update-alternatives --install /usr/bin/gcov-dump gcov-dump /usr/bin/gcov-dump-10 10 && \
    wget -O cmake.sh https://github.com/Kitware/CMake/releases/download/v3.23.0/cmake-3.23.0-linux-x86_64.sh && \
    chmod +x cmake.sh && ./cmake.sh --prefix=/usr/local --skip-license --exclude-subdir && \
    rm cmake.sh

# These commands copy sources in the image
# and set that as the working location
COPY . /root/glshaderPP
WORKDIR /root/glshaderPP
# If there is any conan cache in .conan, copy it in its right place
RUN if [ -d .conan ]; then mv .conan /root/; fi

FROM commonbase as builder

RUN apt-get install -y g++-10 make \
                       libopengl-dev libglu1-mesa-dev libx11-dev libx11-xcb-dev libfontenc-dev libice-dev \
                       libsm-dev libxau-dev libxaw7-dev libxcomposite-dev libxcursor-dev libxdamage-dev \
                       libxdmcp-dev libxext-dev libxfixes-dev libxft-dev libxi-dev libxinerama-dev \
                       libxkbfile-dev libxmu-dev libxmuu-dev libxpm-dev libxrandr-dev libxrender-dev \
                       libxres-dev libxss-dev libxt-dev libxtst-dev libxv-dev libxvmc-dev libxxf86vm-dev \
                       xtrans-dev libxcb-render0-dev libxcb-render-util0-dev libxcb-xkb-dev libxcb-icccm4-dev \
                       libxcb-image0-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-shape0-dev libxcb-sync-dev \
                       libxcb-xfixes0-dev libxcb-xinerama0-dev xkb-data libxcb-dri3-dev uuid-dev libxcb-util-dev \
                       graphviz doxygen && \
    wget https://github.com/conan-io/conan/releases/latest/download/conan-ubuntu-64.deb && \
    dpkg -i conan-ubuntu-64.deb && \
    update-alternatives --install /usr/bin/cpp cpp /usr/bin/cpp-10 10 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 10 && \
    apt-get clean && rm conan-ubuntu-64.deb

# This command initialise conan
RUN conan config init && conan profile update settings.compiler.libcxx=libstdc++11 default

# This command configure sources with CMake, build it, install it
RUN mkdir build && cd build && cmake .. -D CMAKE_BUILD_TYPE=Release -D BUILD_TESTING=On -D BUILD_DOCUMENTATION=On \
    && cmake --build . \
    && cmake --install . --prefix=/home/conan/glshaderPP/install \
    && cd ..

# This command configure sources with CMake for code coverage analysis then build it.
RUN mkdir build-gcov && cd build-gcov && cmake .. -D CMAKE_BUILD_TYPE=Debug -D BUILD_TESTING=On \
    && cmake --build . \
    && cd ..

FROM commonbase as tester

COPY --from=builder /root/glshaderPP/build /root/glshaderPP/build
COPY --from=builder /root/glshaderPP/build-gcov /root/glshaderPP/build-gcov
COPY --from=builder /root/.conan /root/.conan

RUN apt-get update && apt-get install -y xvfb gcovr && \
    apt-get clean

ENV DISPLAY :0
