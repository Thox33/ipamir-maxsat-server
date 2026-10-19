#!/bin/bash

# Kompilieren der C++-Dateien
g++ example_client/client.cc -o run_client

# Starten des Clients
./run_client
