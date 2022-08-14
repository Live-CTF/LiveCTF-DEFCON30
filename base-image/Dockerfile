FROM --platform=linux/amd64 ubuntu:22.04 as base

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y ca-certificates

RUN useradd livectf
RUN useradd -M flag

WORKDIR /

FROM --platform=linux/amd64 rust:slim-buster as build_submitter
RUN apt-get update && apt-get install make
COPY submitter /submitter
RUN make -C submitter


FROM base as livectf_submitter
COPY --from=build_submitter --chown=flag:flag /submitter/target/release/submitter /home/livectf/submitter
RUN chmod 2511 /home/livectf/submitter

COPY --from=build_submitter --chown=flag:flag /submitter/.config.toml /home/livectf/.config.toml
RUN chmod 440 /home/livectf/.config.toml


FROM base as build_nsjail

RUN apt-get update && apt-get install -y --no-install-recommends \
    autoconf \
    bison \
    flex \
    g++ \
    gcc \
    git \
    libnl-route-3-dev \
    libprotobuf-dev \
    libtool \
    make \
    pkg-config \
    curl \
    protobuf-compiler \
    libssl-dev && \
    rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/google/nsjail.git /nsjail && make -C /nsjail


FROM livectf_submitter as run_nsjail

RUN apt-get update && apt-get install -y --no-install-recommends libprotobuf23 libnl-3-200 libnl-route-3-200

COPY --from=build_nsjail /nsjail/nsjail /usr/bin/nsjail
RUN chmod 755 /usr/bin/nsjail

COPY --chown=root:root nsjail/nsjail.conf /home/livectf/nsjail.conf
RUN chmod 400 /home/livectf/nsjail.conf

COPY challenge/run_nsjail.sh /home/livectf/run.sh
RUN chmod 755 /home/livectf/run.sh

WORKDIR /home/livectf/

CMD ["./run.sh"]


FROM livectf_submitter as run_socat
RUN apt-get update && apt-get install -y --no-install-recommends \
    socat && \
    rm -rf /var/lib/apt/lists/*

COPY challenge/run_socat.sh /home/livectf/run.sh
RUN chmod 755 /home/livectf/run.sh

WORKDIR /home/livectf/

CMD ["./run.sh"]
