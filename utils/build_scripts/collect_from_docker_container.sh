set -ex && mkdir -p build/release/bin
set -ex && docker create --name quenero-daemon-container quenero-daemon-image
set -ex && docker cp quenero-daemon-container:/usr/local/bin/ build/release/
set -ex && docker rm quenero-daemon-container
