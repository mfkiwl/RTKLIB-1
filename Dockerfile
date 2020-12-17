# Docker image to build CLI apps only in Alpine Linux
FROM alpine AS builder

#CLI applications only
RUN apk update && apk add cmake make g++ gfortran git

#To build QT apps:
#RUN apk add cqt5-qtbase-dev qt5-qtserialport-dev

COPY . /rtklib_src
WORKDIR /rtklib_src/build
RUN echo "Bulding with $(nproc) threads" \
  && cmake -DCMAKE_BUILD_TYPE=Release .. \
# Build QT apps:
#-DBUILD_QT_APPS \
  && make -j$(nproc) \
  && make install \
  && make package

# Copy from builder, binary only, and install
FROM alpine AS runtime
RUN apk update && apk add tar libgcc
COPY --from=builder /rtklib_src/build/*.tar.bz2 /tmp
RUN mkdir -p /tmp/rtklib \
  && tar xvjf /tmp/*.tar.bz2 -C /tmp/rtklib --strip-components=1 \
  && cp -R /tmp/rtklib/bin/* /usr/bin \
  && cp -R /tmp/rtklib/lib/* /usr/lib \
  && cp -R /tmp/rtklib/include/* /usr/include \
  && ldconfig /etc/ld.so.conf.d
RUN echo "Test convbin execution:" && convbin -h
