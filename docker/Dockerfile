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
     git wget \
     libboost-all-dev libusb-1.0-0-dev doxygen python3-docutils python3-mako python3-numpy python3-requests python3-ruamel.yaml python3-setuptools && \
     rm -rf /var/lib/apt/lists/*

#COPY . /srsRAN
RUN git clone https://github.com/srsran/srsRAN.git -b release_22_04_1
COPY docker/* /srsRAN/docker/

RUN cd srsRAN && \ 
    mkdir build &&  cd build && \
    cmake  ../ && \
    make -j 4  && \
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

COPY --from=builder /usr/local/lib/libsrsran_* \
                    /usr/local/lib/

COPY --from=builder /usr/lib/x86_64-linux-gnu/libboost_program_options.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libboost_chrono.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libboost_date_time.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libmbedcrypto.so.3 \ 
                    /usr/lib/x86_64-linux-gnu/libconfig++.so.9 \
                    /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libboost_regex.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libboost_serialization.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libboost_thread.so.1.71.0 \
                    /usr/lib/x86_64-linux-gnu/libusb-1.0.so.0 \
                    /usr/lib/x86_64-linux-gnu/libsctp.so.1 \
                    /usr/lib/x86_64-linux-gnu/libudev.so.1 \
                    /usr/lib/x86_64-linux-gnu/libicuuc.so.66 \
                    /usr/lib/x86_64-linux-gnu/libicui18n.so.66 \
                    /usr/lib/x86_64-linux-gnu/libicudata.so.66 \ 
                    /usr/local/lib/libsrsran_rf_zmq.so.0 \
                    /usr/lib/x86_64-linux-gnu/libfftw3f.so.3 /usr/lib/x86_64-linux-gnu/
RUN ldconfig

WORKDIR /srsran/bin
COPY --from=builder /usr/local/bin/srsenb \
                    /usr/local/bin/srsue \
                    /srsRAN/docker/entrypoint.sh /srsran/bin/

ENTRYPOINT ["/srsran/bin/entrypoint.sh"]
#CMD ["sleep", "infinity"]

