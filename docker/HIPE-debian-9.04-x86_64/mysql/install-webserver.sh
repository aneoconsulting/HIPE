#! /usr/bin/env bash


#
# Provision script for Ubuntu 14.06 LTS
# ------------
#
# Inspired and modified from: https://gist.github.com/maoosi/d3a4c5670d4f014fe0082f88e48e89af
# Includes: Apache, PHP7, MySql, Git, Composer, NodeJS
#

echo  "\n--- Installing now... ---\n"
echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections
export DEBIAN_FRONTEND="noninteractive"

DBNAME=hipe

DBUSER=root

DBPASSWD="root"


USERNAME=hipe-group
PASSWORD="hipe-group"

#sudo useradd -m -g www-data $USERNAME -s /bin/bash
#echo $USERNAME:$PASSWORD | chpasswd


echo  "\n\n--- Updating packages list ---\n"
sudo apt-get -qq update

sudo apt-get -y install dialog apt-utils lsb-release gdebi-core


#echo  "\n--- Installing Apache2 ---\n"
#sudo apt-get -y install apache2 > /dev/null 2>&1


#echo  "\n--- Installing PHP7 ---\n"
#sudo apt-get -y install php7.0 php7.0-fpm php7.0-mysql -y > /dev/null 2>&1


#echo  "\n--- Installing PHP packages ---\n"
#sudo apt-get -y install php7.0-cli libapache2-mod-php7.0 php-curl php-mcrypt php-json php-gd php-common php-imagick php-apc sudo apt-get install php-dom > /dev/null 2>&1

#echo "\n--- Installing sendmail ---\n"
#sudo sudo apt-get -y install sendmail

#echo "phpmyadmin phpmyadmin/dbconfig-install boolean true" | debconf-set-selections
#echo "phpmyadmin phpmyadmin/app-password-confirm password $DBPASSWD" | debconf-set-selections
#echo "phpmyadmin phpmyadmin/mysql/admin-pass password $DBPASSWD" | debconf-set-selections
#echo "phpmyadmin phpmyadmin/mysql/app-pass password $DBPASSWD" | debconf-set-selections
#echo "phpmyadmin phpmyadmin/reconfigure-webserver multiselect none" | debconf-set-selections

#wget https://dev.mysql.com/get/mysql-apt-config_0.8.10-1_all.deb
#sudo debconf-set-selections <<< 'mysql-apt-config        mysql-apt-config/select-product select  Ok'
#sudo debconf-set-selections <<< 'mysql-apt-config        mysql-apt-config/select-preview select  Disabled'
#sudo debconf-set-selections <<< 'mysql-apt-config        mysql-apt-config/repo-distro    select  debian'
#sudo debconf-set-selections <<< 'mysql-apt-config        mysql-apt-config/select-tools   select  Enabled'
#sudo debconf-set-selections <<< 'mysql-apt-config        mysql-apt-config/tools-component        string  mysql-tools'
#sudo debconf-set-selections <<< 'mysql-apt-config        mysql-apt-config/unsupported-platform   select  abort'
#sudo debconf-set-selections <<< 'mysql-apt-config        mysql-apt-config/select-server  select  mysql-8.0'
#sudo debconf-set-selections <<< 'mysql-apt-config        mysql-apt-config/dmr-warning    note'
#sudo debconf-set-selections <<< 'mysql-apt-config        mysql-apt-config/preview-component      string'
#sudo debconf-set-selections <<< 'mysql-apt-config        mysql-apt-config/repo-codename  select  stretch'
#sudo debconf-set-selections <<< 'mysql-apt-config        mysql-apt-config/repo-url       string  http://repo.mysql.com/apt'

#echo 'y' | gdebi mysql-apt-config_0.8.10-1_all.deb

#sudo apt-get update

#echo  "\n--- Installing MySql ---\n"
#sudo debconf-set-selections <<< 'mysql-community-server mysql-community-server/root-pass password $DBPASSWD'
#sudo debconf-set-selections <<< 'mysql-community-server mysql-community-server/re-root-pass password $DBPASSWD'
#sudo debconf-set-selections <<< "mysql-community-server mysql-server/default-auth-override select Use Legacy Authentication Method (Retain MySQL 5.x Compatibility)"

#echo "mysql-server mysql-server/root_password password $DBPASSWD" | debconf-set-selections
#echo "mysql-server mysql-server/root_password_again password $DBPASSWD" | debconf-set-selections

#sudo apt-get -y install mysql-community-server 
#sudo apt-get -y install mysql-server >> /dev/null 2>&1

#echo  "\n--- Enabling mod-rewrite ---\n"
#a2enmod rewrite > /dev/null 2>&1


#echo  "\n--- Setting up our DirectoryIndex specification ---\n"
#cat > /etc/apache2/mods-enabled/dir.conf <<EOF
#<IfModule mod_dir.c>
#    DirectoryIndex index.php index.html index.cgi index.pl index.xhtml index.htm
#</IfModule>
#EOF

#echo "\n--- Setting up permissions ---"
#sudo usermod $USERNAME -g www-data
#sudo chown -R www-data:www-data /var/www
#sudo chmod -R g+rwX /var/www



#echo  "\n--- Restarting Apache ---\n"
#sudo service apache2 restart 2>&1
#sudo service mysql restart 2>&1

echo 'create database `hipe`' | mysql -uroot
mysql -uroot -h localhost hipe < /home/hipe-group/mysql_db/hipe.sql

#echo  "\n--- Installing Composer ---\n"
#curl -sS https://getcomposer.org/installer | sudo php -- --install-dir=/usr/local/bin --filename=composer

curl -sL https://deb.nodesource.com/setup_6.x | sudo bash -

echo  "\n--- Installing NodeJS and NPM ---\n"
apt-get -y install nodejs rpm 2>&1


echo  "\n--- Webserver Installation done ---\n"


