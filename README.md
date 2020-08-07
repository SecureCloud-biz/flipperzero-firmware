# Flipper Zero firmware repository

Open source multi-tool device for researching and pentesting radio protocols, access control systems, hardware, and more.

Project website: [flipperzero.one](https://flipperzero.one)

## You can find all available information at [Project wiki](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki)

## How to contribute

(add code style, contribution guide here)

## Building

### Build in docker container (main way)

1. Install [docker compose](https://docs.docker.com/compose/install/)
2. After startup you should run `docker-compose up -d` to run the container.
3. Then you can run `docker-compose exec dev make` to build application.

If Dockerfile is changed you should run `docker-compose down` and `docker-compose build` for rebuild the image.

### Build in IDE

* Arduino IDE (in progress)
* PlatformIO (in progress)

## Firmware description

* HAL
* OSAL
* Components description
* Basic Flipperzero components