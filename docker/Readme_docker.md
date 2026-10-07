# Docker image for testing

We provide a Dockerfile using Alpine Linux for testing the ffilesystem library with musl libc.

On macOS, colima can be used to run the Docker image.

```sh
brew install colima docker docker-buildx
```

Enable Docker buildx

```sh
mkdir -p ~/.docker/cli-plugins
ln -sfn "$(brew --prefix)/opt/docker-buildx/bin/docker-buildx" ~/.docker/cli-plugins/docker-buildx
hash -r
```

Start the image with colima:

```sh
colima start --vm-type vz --mount-type virtiofs --cpu 4 --memory 8

docker context use colima
```

Build image

```sh
docker buildx build -t alpine-fortran --load .
```

Build project

```sh
docker run --rm -v "$PWD":/src -w /src alpine-fortran \
  sh -c 'cmake -S . -B build-docker -G Ninja && cmake --build build-docker'
```

When done, stop the colima instance:

```sh
colima stop
```
