FROM ubuntu:16.04
MAINTAINER Mirko Krause <mirko.krause@student.hpi.de>

RUN mkdir /project

COPY . /project

WORKDIR /project

ENV OPOSSUM_HEADLESS_SETUP true
RUN apt-get update \
	&& apt-get install -y \
	sudo \
	software-properties-common \
	&& add-apt-repository -y ppa:git-core/ppa \
	&& apt-get update \
	&& apt-get install -y git \
	&& chmod +x install.sh #\
	&& ./install.sh \
	&& rm -rf /var/lib/apt/lists/*

VOLUME /project

CMD echo give me a command