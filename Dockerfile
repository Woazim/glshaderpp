FROM conanio/gcc11:latest

# These commands copy sources in the image
# and set that as the working location
COPY --chown=conan:1001 . /home/conan/glshaderPP
WORKDIR /home/conan/glshaderPP

# If there is any conan cache in .conan, copy it in its right place
RUN if [ -d .conan ]; then cp -r .conan /home/conan/; fi
# This command initialise conan
RUN conan config init && conan profile update settings.compiler.libcxx=libstdc++11 default

# This command upgrade CMake
RUN pip install cmake -U

#Installing all dependencies
RUN sudo apt-get update && sudo apt-get install -y libglu1-mesa-dev  libx11-dev libx11-xcb-dev libfontenc-dev libice-dev \
                                                   libsm-dev libxau-dev libxaw7-dev libxcomposite-dev libxcursor-dev libxdamage-dev \
                                                   libxdmcp-dev libxext-dev libxfixes-dev libxft-dev libxi-dev libxinerama-dev \
                                                   libxkbfile-dev libxmu-dev libxmuu-dev libxpm-dev libxrandr-dev libxrender-dev \
                                                   libxres-dev libxss-dev libxt-dev libxtst-dev libxv-dev libxvmc-dev libxxf86vm-dev \
                                                   xtrans-dev libxcb-render0-dev libxcb-render-util0-dev libxcb-xkb-dev libxcb-icccm4-dev \
                                                   libxcb-image0-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-shape0-dev libxcb-sync-dev \
                                                   libxcb-xfixes0-dev libxcb-xinerama0-dev xkb-data libxcb-dri3-dev uuid-dev libxcb-util-dev \
                                                   xvfb

# This command configure sources with CMake, build it, install it, then remove intermediate files to shrink image
RUN mkdir build && cd build && cmake .. -D CMAKE_BUILD_TYPE=Release -D BUILD_TESTING=On \
    && cmake --build . \
    && cmake --install . --prefix=/home/conan/glshaderPP/install

WORKDIR /home/conan/glshaderPP

LABEL Name=glshaderPP Version=0.0.1
