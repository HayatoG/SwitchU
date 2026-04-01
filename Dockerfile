FROM archlinux:latest

# System dependencies
RUN pacman -Syu --noconfirm \
    && pacman -S --noconfirm \
        base-devel \
        git \
        curl \
        cmake \
        python \
    && pacman -Scc --noconfirm

# Import devkitPro signing key
RUN pacman-key --recv BC26F752D25B92CE272E0F44F7FD5492264BB9D0 \
        --keyserver keyserver.ubuntu.com \
    && pacman-key --lsign BC26F752D25B92CE272E0F44F7FD5492264BB9D0

# Install devkitPro keyring
RUN pacman -U --noconfirm \
        https://pkg.devkitpro.org/devkitpro-keyring.pkg.tar.zst

# Add devkitPro repositories
RUN printf '\n[dkp-libs]\nServer = https://pkg.devkitpro.org/packages\n[dkp-linux]\nServer = https://pkg.devkitpro.org/packages/linux/$arch/\n' \
        >> /etc/pacman.conf

# Install Switch toolchain and libraries
RUN pacman -Sy --noconfirm \
        switch-dev \
        switch-libwebp \
        switch-sdl2 \
        switch-sdl2_mixer \
        switch-sdl2_ttf \
        switch-zlib \
    && pacman -Scc --noconfirm

# Install xmake
RUN curl -fsSL https://raw.githubusercontent.com/xmake-io/xmake/master/scripts/get.sh | bash

# devkitPro environment
ENV DEVKITPRO=/opt/devkitpro
ENV DEVKITARM=/opt/devkitpro/devkitARM
ENV PATH="${DEVKITPRO}/devkitA64/bin:${DEVKITPRO}/tools/bin:/root/.local/bin:${PATH}"

WORKDIR /project

# Default: configure + build both NSPs
CMD xmake f -p cross --toolchain=devkita64 -y \
    && xmake -j$(nproc)
