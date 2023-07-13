#!/bin/bash
# Generate test certs for the scifserver
#

openssl req -new -newkey rsa:4096 -days 365 -nodes -x509 \
    -subj "/C=US/ST=Denial/L=Springfield/O=Dis/CN=www.example.com" \
    -keyout www.example.com.key  -out www.example.com.cert

