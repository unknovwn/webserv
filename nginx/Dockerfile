FROM debian:buster

EXPOSE 7100-7120

RUN apt update
RUN apt install -y nginx vim curl

COPY start.sh .

CMD sh start.sh
