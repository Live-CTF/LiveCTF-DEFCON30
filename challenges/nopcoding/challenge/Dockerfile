FROM --platform=amd64 livectf_base:nsjail as base

# Space-separated list of required packages
ARG REQUIRED_PACKAGES="build-essential gcc-multilib libc6:i386 wget tar"

ENV DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get -y upgrade && \
    apt-get install -y --no-install-recommends \
    ${REQUIRED_PACKAGES} && \
    rm -rf /var/lib/apt/lists/*


FROM base as build
COPY build.sh /build.sh
COPY src/ /src/
RUN mkdir /build /handout
RUN cd / && /build.sh
RUN tar -czf /handout.tar.gz /handout


FROM base as run
COPY --from=build /build/challenge /home/livectf/
COPY --from=build /build/libcapstone.so.4 /lib/
ENV LD_LIBRARY_PATH=/home/livectf
COPY --from=build /handout.tar.gz /handout.tar.gz
COPY --chown=root:flag config.toml /home/livectf/.config.toml
RUN chmod 440 /home/livectf/.config.toml
