#!/bin/bash

# Starten des Servers in einem neuen Terminalfenster
gnome-terminal -- ./start_server.sh &

# Warten Sie einen Moment, um sicherzustellen, dass der Server gestartet wurde
sleep 1

# Starten des Clients im aktuellen Terminalfenster
./start_client.sh