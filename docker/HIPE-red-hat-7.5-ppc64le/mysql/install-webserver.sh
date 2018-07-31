#! /usr/bin/env bash


#
# Provision script for Ubuntu 14.06 LTS
# ------------
#
# Inspired and modified from: https://gist.github.com/maoosi/d3a4c5670d4f014fe0082f88e48e89af
# Includes: Apache, PHP7, MySql, Git, Composer, NodeJS
#

echo  "\n--- Installing now... ---\n"


echo  "\n>> Please enter a name for the database:\n"
DBNAME=hipe
echo  "\n>> Please enter an username for accessing the database:\n"
DBUSER=root
echo  "\n>> Please enter a password for your database:\n"
DBPASSWD="root"


USERNAME=hipe-group
PASSWORD="hipe-group"

sudo useradd -m -g www-data $USERNAME -s /bin/bash
#echo $USERNAME:$PASSWORD | chpasswd


echo  "\n\n--- Updating packages list ---\n"
sudo apt-get -qq update


echo  "\n--- Installing Apache2 ---\n"
sudo apt-get -y install apache2 > /dev/null 2>&1


echo  "\n--- Installing PHP7 ---\n"
sudo apt-get -y install php7.0 php7.0-fpm php7.0-mysql -y > /dev/null 2>&1


echo  "\n--- Installing PHP packages ---\n"
sudo apt-get -y install php7.0-cli libapache2-mod-php7.0 php-curl php-mcrypt php-json php-gd php-common php-imagick php-apc sudo apt-get install php-dom > /dev/null 2>&1

#echo "\n--- Installing sendmail ---\n"
#sudo sudo apt-get -y install sendmail

echo  "\n--- Installing MySql ---\n"
echo "mysql-server mysql-server/root_password password $DBPASSWD" | debconf-set-selections
echo "mysql-server mysql-server/root_password_again password $DBPASSWD" | debconf-set-selections
echo "phpmyadmin phpmyadmin/dbconfig-install boolean true" | debconf-set-selections
echo "phpmyadmin phpmyadmin/app-password-confirm password $DBPASSWD" | debconf-set-selections
echo "phpmyadmin phpmyadmin/mysql/admin-pass password $DBPASSWD" | debconf-set-selections
echo "phpmyadmin phpmyadmin/mysql/app-pass password $DBPASSWD" | debconf-set-selections
echo "phpmyadmin phpmyadmin/reconfigure-webserver multiselect none" | debconf-set-selections
sudo apt-get -y install mysql-server > /dev/null 2>&1


echo  "\n--- Enabling mod-rewrite ---\n"
a2enmod rewrite > /dev/null 2>&1


echo  "\n--- Setting up our DirectoryIndex specification ---\n"
cat > /etc/apache2/mods-enabled/dir.conf <<EOF
<IfModule mod_dir.c>
    DirectoryIndex index.php index.html index.cgi index.pl index.xhtml index.htm
</IfModule>
EOF

echo "\n--- Setting up permissions ---"
sudo usermod $USERNAME -g www-data
sudo chown -R www-data:www-data /var/www
sudo chmod -R g+rwX /var/www



echo  "\n--- Restarting Apache ---\n"
sudo service apache2 restart 2>&1
sudo service mysql restart 2>&1

echo 'create database `hipe`' | mysql -uroot -proot
mysql -uroot -proot -h localhost hipe < /home/hipe-group/mysql_db/hipe.sql

#echo  "\n--- Installing Composer ---\n"
#curl -sS https://getcomposer.org/installer | sudo php -- --install-dir=/usr/local/bin --filename=composer

#curl -sL https://deb.nodesource.com/setup_6.x | sudo bash -

echo  "\n--- Installing NodeJS and NPM ---\n"
apt-get -y install nodejs rpm npm


echo  "\n--- Webserver Installation done ---\n"


