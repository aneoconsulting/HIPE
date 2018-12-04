#!/bin/bash

openssl req -x509 -nodes -days 730 -newkey rsa:2048 -keyout mylaptop.key -out mylaptop.crt -config sancert.conf -extensions 'v3_req'

openssl x509 -in mylaptop.crt -noout -text
