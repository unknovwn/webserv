#!/bin/zsh

docker build . -t nginx
docker run --name nginx -p 7100-7120:7100-7120 --rm -d nginx
