#Devrait créer une image pour les tests (explorer la commande FROM xxx as yyy)
# voir par exemple https://github.com/conan-io/conan-docker-tools/blob/master/modern/gcc/Dockerfile

FROM conanio/gcc10-ubuntu16.04:latest as builder

LABEL Name=glshaderPP-build Version=1.0.0

# This command initialise conan
RUN conan config init && conan profile update settings.compiler.libcxx=libstdc++11 default

# This command upgrade CMake
RUN pip install cmake -U

#Installing all dependencies
RUN    sudo apt-get update \
    && sudo apt-get install -y libglu1-mesa-dev  libx11-dev libx11-xcb-dev libfontenc-dev libice-dev \
                               libsm-dev libxau-dev libxaw7-dev libxcomposite-dev libxcursor-dev libxdamage-dev \
                               libxdmcp-dev libxext-dev libxfixes-dev libxft-dev libxi-dev libxinerama-dev \
                               libxkbfile-dev libxmu-dev libxmuu-dev libxpm-dev libxrandr-dev libxrender-dev \
                               libxres-dev libxss-dev libxt-dev libxtst-dev libxv-dev libxvmc-dev libxxf86vm-dev \
                               xtrans-dev libxcb-render0-dev libxcb-render-util0-dev libxcb-xkb-dev libxcb-icccm4-dev \
                               libxcb-image0-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-shape0-dev libxcb-sync-dev \
                               libxcb-xfixes0-dev libxcb-xinerama0-dev xkb-data libxcb-dri3-dev uuid-dev libxcb-util-dev \
                               graphviz doxygen \
                               \
                               xvfb libxcomposite1 libxcursor1 libxdamage1 libxft2 libxi6 libxinerama1 \
                               libxrandr2 libxres1 libxss1 libxtst6 libxv1 libxvmc1 libxcb-xkb1 libxcb-icccm4 \
                               libxcb-image0 libxcb-keysyms1 libxcb-render0 libxcb-render-util0 libxcb-shape0 \
                               libxcb-xinerama0 libglu1-mesa \
                               \
                               gcovr \
    && sudo apt-get clean

# These commands copy sources in the image
# and set that as the working location
COPY --chown=conan:1001 . /home/conan/glshaderPP
WORKDIR /home/conan/glshaderPP
# If there is any conan cache in .conan, copy it in its right place
RUN if [ -d .conan ]; then cp -r .conan /home/conan/; fi

# This command configure sources with CMake, build it, install it
RUN mkdir build && cd build && cmake .. -D CMAKE_BUILD_TYPE=Release -D BUILD_TESTING=On -D BUILD_DOCUMENTATION=On \
    && cmake --build . \
    && cmake --install . --prefix=/home/conan/glshaderPP/install \
    && cd ..

# This command configure sources with CMake for code coverage analysis then build it.
RUN mkdir build-gcov && cd build-gcov && cmake .. -D CMAKE_BUILD_TYPE=Debug -D BUILD_TESTING=On \
    && cmake --build . \
    && cd ..

#FROM ubuntu:latest as tester

#COPY --from=builder /home/conan/glshaderPP /home/conan/glshaderPP
#COPY --from=builder /home/conan/.conan /root/.conan
#WORKDIR /home/conan/glshaderPP

#RUN apt-get update && apt-get install -y wget xvfb libxcomposite1 libxcursor1 libxdamage1 libxft2 libxi6 libxinerama1 \
#                                         libxrandr2 libxres1 libxss1 libxtst6 libxv1 libxvmc1 libxcb-xkb1 libxcb-icccm4 \
#                                         libxcb-image0 libxcb-keysyms1 libxcb-render0 libxcb-render-util0 libxcb-shape0 \
#                                         libxcb-xinerama0 libglu1-mesa gcovr && \
#    wget -O cmake.sh https://github.com/Kitware/CMake/releases/download/v3.23.0/cmake-3.23.0-linux-x86_64.sh && \
#    chmod +x cmake.sh && ./cmake.sh --skip-license && \
#    apt-get clean && rm cmake.sh

ENV DISPLAY :0
