FROM debian:buster

EXPOSE 80

RUN apt update
RUN apt install -y nginx vim curl

COPY start.sh .

CMD sh start.sh
