FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y \
        dnsutils \
        iptables \
        simpleproxy \
        && \
    rm -rf /var/lib/apt/lists/*

VOLUME /code

WORKDIR /code

ENTRYPOINT scripts/run_proxy.sh
