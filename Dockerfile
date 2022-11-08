# syntax=docker/dockerfile:1
FROM ubuntu:22.04
RUN apt-get update && apt-get upgrade --yes && apt-get install --yes python3 python3-pip libclang-dev clang
RUN useradd -ms /bin/bash builder

USER builder
RUN pip3 install "MarkupSafe==2.1.1" "clang==14.0" "mako==1.2.3"

WORKDIR /scripts/bindings
CMD python3 cpp_generator.py cpp_template.mako /src/nats.h /dest/nats.hpp
