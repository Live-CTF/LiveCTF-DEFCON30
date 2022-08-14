# nsjail is giving `execve('/home/livectf/challenge') failed: No such file or directory` despite everything appearing ok...
#FROM livectf_base:nsjail as base
FROM --platform=amd64 livectf_base:socat as base

FROM base as build

# Space-separated list of required packages
ARG REQUIRED_PACKAGES="build-essential gcc-aarch64-linux-gnu libc6-dev-arm64-cross"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ${REQUIRED_PACKAGES} \
    && rm -rf /var/lib/apt/lists/*

COPY build.sh /build.sh
COPY src/ /src/
RUN mkdir /build /handout
RUN cd / && /build.sh
RUN tar -czf /handout.tar.gz /handout

FROM base as run

RUN apt-get update && apt-get install -y qemu-user libc6-arm64-cross

COPY --from=build /build/challenge /home/livectf/
COPY --from=build /build/pacman /home/livectf/
COPY --from=build /handout.tar.gz /handout.tar.gz
COPY --chown=root:flag config.toml /home/livectf/.config.toml
RUN chmod 440 /home/livectf/.config.toml
