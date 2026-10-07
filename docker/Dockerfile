# Alpine is musl libc.
FROM alpine:3.22

# musl-dev supplies libc headers gfortran/gcc need to link.
# --no-cache keeps the image small.
RUN apk add --no-cache \
        cmake \
        ninja-is-really-ninja \
        g++ \
        gfortran \
        musl-dev

WORKDIR /src

# Default to an interactive shell so `docker run -it` is usable.
CMD ["/bin/sh"]
