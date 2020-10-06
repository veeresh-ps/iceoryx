#!/bin/bash

#sshpass -p "WelcomeBosch@01" ssh -o StrictHostKeyChecking=no vhe3kor@10.0.2.4 -i  'uptime; hostname -f'

cd perf_app

./remote_client.expect '10.0.2.4' 'vhe3kor' 'WelcomeBosch@01'
