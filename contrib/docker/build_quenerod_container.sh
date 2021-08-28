RELEASE=8.1.6
docker build -t quenerod:${RELEASE} -f Dockerfile.quenerod --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) .
