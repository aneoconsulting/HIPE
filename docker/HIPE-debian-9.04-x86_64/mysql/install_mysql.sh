#!/bin/bash

groupadd -r mysql && useradd -r -g mysql mysql


apt-get update && apt-get install -y --no-install-recommends gnupg dirmngr && rm -rf /var/lib/apt/lists/*


export GOSU_VERSION=1.7

#set -x \
#	&& apt-get update && apt-get install -y --no-install-recommends ca-certificates wget && rm -rf /var/lib/apt/lists/* \
#	&& wget -O /usr/local/bin/gosu "https://github.com/tianon/gosu/releases/download/$GOSU_VERSION/gosu-$(dpkg --print-architecture)" \
#	&& wget -O /usr/local/bin/gosu.asc "https://github.com/tianon/gosu/releases/download/$GOSU_VERSION/gosu-$(dpkg --print-architecture).asc" \
#	&& export GNUPGHOME="$(mktemp -d)" \
#	&& gpg --keyserver ha.pool.sks-keyservers.net --recv-keys B42F6819007F00F88E364FD4036A9C25BF357DD4 \
#	&& gpg --batch --verify /usr/local/bin/gosu.asc /usr/local/bin/gosu \
#	&& rm -rf "$GNUPGHOME" /usr/local/bin/gosu.asc \
#	&& chmod +x /usr/local/bin/gosu \
#	&& gosu nobody true
#
#	&& apt-get purge -y --auto-remove ca-certificates wget

mkdir /docker-entrypoint-initdb.d

apt-get update && apt-get install -y --no-install-recommends \
		pwgen \
		openssl \
		perl


set -ex; \
	key='A4A9406876FCBD3C456770C88C718D3B5072E1F5'; \
	export GNUPGHOME="$(mktemp -d)"; \
	gpg --keyserver ha.pool.sks-keyservers.net --recv-keys "$key"; \
	gpg --export "$key" > /etc/apt/trusted.gpg.d/mysql.gpg; \
	rm -rf "$GNUPGHOME"; \
	apt-key list > /dev/null
	
export MYSQL_MAJOR=8.0
export MYSQL_VERSION=8.0.11-1debian9


echo "deb http://repo.mysql.com/apt/debian/ stretch mysql-${MYSQL_MAJOR}" > /etc/apt/sources.list.d/mysql.list

{ \
		echo mysql-community-server mysql-community-server/data-dir select ''; \
		echo mysql-community-server mysql-community-server/root-pass password ''; \
		echo mysql-community-server mysql-community-server/re-root-pass password ''; \
		echo mysql-community-server mysql-community-server/remove-test-db select false; \
} | debconf-set-selections

apt-get update && apt-get install -y mysql-community-client-core="${MYSQL_VERSION}" mysql-community-server-core="${MYSQL_VERSION}" && rm -rf /var/lib/apt/lists/* \
	&& rm -rf /var/lib/mysql && mkdir -p /var/lib/mysql /var/run/mysqld \
	&& chown -R mysql:mysql /var/lib/mysql /var/run/mysqld \
	&& chmod 777 /var/run/mysqld

sudo mkdir /var/lib/mysql-files
sudo chmod 700 /var/lib/mysql-files

sudo mysqld --user=root --default-authentication-plugin=mysql_native_password --initialize-insecure

sudo mysqld --user=root &
sleep 5
echo "ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'root'" | mysql -uroot

kill -9 %1

echo 'create database `hipe`' | mysql -uroot -proot
mysql -uroot -proot -h localhost hipe < /home/hipe-group/mysql_db/hipe.sql

sleep 5



