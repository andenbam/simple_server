#!/bin/sh
mkdir sslgencrt
cd sslgencrt
#Step 1: Generate a Private Key
openssl genrsa -des3 -out server.key 1024

#Step 2: Generate a CSR (Certificate Signing Request)
#Common Name (eg, your name or your server's hostname) []:example.com
openssl req -new -key server.key -out server.csr

#Step 3: Remove Passphrase from Key
cp server.key server.key.org
openssl rsa -in server.key.org -out server.key

#Step 4: Generating a Self-Signed Certificate
openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt
