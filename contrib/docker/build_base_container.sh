RELEASE=20.04
curl -so quenero-deb-key.gpg https://deb.quenero.io/pub.gpg
docker build -t quenero-ubuntu:${RELEASE} -f Dockerfile.quenero-ubuntu .
rm quenero-deb-key.gpg
