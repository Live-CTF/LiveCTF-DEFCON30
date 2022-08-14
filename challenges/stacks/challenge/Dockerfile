FROM --platform=amd64 livectf_base:nsjail as base
# Alternatively: FROM livectf_base:socat as base

# Space-separated list of required packages
ARG REQUIRED_PACKAGES="build-essential socat libc6:i386 gcc-multilib rake"

ENV DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get -y upgrade && \
    apt-get install -y --no-install-recommends \
    ${REQUIRED_PACKAGES} \
    && rm -rf /var/lib/apt/lists/*

FROM --platform=amd64 base as build

WORKDIR /

COPY stacks ./stacks
COPY libctf ./libctf
RUN mkdir handout
WORKDIR /stacks
RUN /usr/bin/rake release
RUN mkdir /build/
RUN cp *.o /build/
RUN cp stacks /build/
RUN cp stacks /handout/challenge
RUN cp /usr/lib/i386-linux-gnu/libc.so.6 /handout
RUN cp /usr/lib/i386-linux-gnu/ld-linux.so.2 /handout
WORKDIR /
RUN tar -czf handout.tar.gz handout


FROM --platform=amd64 base as run
COPY --from=build /build/stacks /home/livectf/challenge
COPY --from=build /handout.tar.gz /handout.tar.gz
COPY --chown=root:flag config.toml /home/livectf/.config.toml
RUN chmod 440 /home/livectf/.config.toml
