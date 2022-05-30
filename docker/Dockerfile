#---------------------------------------------------------------------
# BUILDER IMAGE
#---------------------------------------------------------------------
ARG BASE_IMAGE=ubuntu:focal

FROM $BASE_IMAGE as builder

RUN apt update && \
     DEBIAN_FRONTEND=noninteractive apt install -y \
     build-essential \
     cmake libfftw3-dev \
     libmbedtls-dev \
     libboost-program-options-dev \
     libconfig++-dev \
     libsctp-dev \
     libzmq3-dev \
     iputils-ping \
     iproute2 \
     iptables \
     git wget && \
     rm -rf /var/lib/apt/lists/*

COPY . /srsRAN

RUN cd srsRAN && \ 
    mkdir build &&  cd build && \
    cmake  ../ && \
    make  && \
    make install

#---------------------------------------------------------------------
# TARGET IMAGE
#---------------------------------------------------------------------
ARG BASE_IMAGE=ubuntu:focal
FROM $BASE_IMAGE as srsran

RUN apt update && apt install iproute2 libzmq3-dev -y

COPY --from=builder /usr/local/share/srsran/sib.conf.example /root/.config/srsran/sib.conf
COPY --from=builder /usr/local/share/srsran/rb.conf.example /root/.config/srsran/rb.conf
COPY --from=builder /srsRAN/docker/*.conf /root/.config/srsran/

COPY --from=builder /usr/local/lib/libsrsran_* /usr/local/lib/

COPY --from=builder /usr/lib/x86_64-linux-gnu/libboost_program_options.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libmbedcrypto.so.3 \ 
                    /usr/lib/x86_64-linux-gnu/libconfig++.so.9 \
                    /usr/lib/x86_64-linux-gnu/libsctp.so.1 \
                    /usr/local/lib/libsrsran_rf_zmq.so.0 \
                    /usr/lib/x86_64-linux-gnu/libfftw3f.so.3 /usr/lib/x86_64-linux-gnu/
RUN ldconfig

WORKDIR /srsran/bin
COPY --from=builder /usr/local/bin/srsenb \
                    /usr/local/bin/srsue \
                    /srsRAN/docker/entrypoint.sh /srsran/bin/

ENTRYPOINT ["/srsran/bin/entrypoint.sh"]
