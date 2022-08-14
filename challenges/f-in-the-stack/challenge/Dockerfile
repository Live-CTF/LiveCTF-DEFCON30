FROM livectf_base:socat as base

FROM base as build
USER root

# Space-separated list of required packages
ARG REQUIRED_PACKAGES="build-essential"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ${REQUIRED_PACKAGES} \
    && rm -rf /var/lib/apt/lists/*

COPY build.sh /build.sh
COPY src/ /src/
RUN mkdir /build /handout
COPY Dockerfile.run /
RUN cd / && /build.sh
RUN tar -czf /handout.tar.gz /handout

FROM base as run
COPY --from=build build/challenge/ /home/livectf/
COPY --from=build handout.tar.gz /handout.tar.gz
COPY --chown=root:flag config.toml /home/livectf/.config.toml
RUN chmod 440 /home/livectf/.config.toml
RUN echo "F" > /etc/os-release

USER livectf

CMD ["./run.sh"]