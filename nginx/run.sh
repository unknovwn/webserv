#!/bin/zsh

docker build . -t nginx
docker run --name nginx -p 80:80 --rm -d nginx
