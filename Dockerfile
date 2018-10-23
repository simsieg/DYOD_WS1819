FROM ubuntu:18.04
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
	&& chmod +x install.sh \
	&& ./install.sh \
	&& rm -rf /var/lib/apt/lists/*

VOLUME /project

RUN apt-get update \
	&& apt-get install -y \
	openssh-server \
	gdbserver \
	&& rm -rf /var/lib/apt/lists/*

# following taken from: http://ebagdasa.me/2017-01-20/docker-clion-debugging/
RUN mkdir /var/run/sshd \
	&& echo 'root:root' | chpasswd \
	&& sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config \
	&& sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd
# last line is an SSH login fix. Otherwise user is kicked off after login

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

EXPOSE 22 7777

RUN chmod +x run-container.sh

# https://stackoverflow.com/questions/32727594/how-to-pass-arguments-to-shell-script-through-docker-run
ENTRYPOINT ["/project/run-container.sh"]
CMD []