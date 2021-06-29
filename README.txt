Files: 
=> run_tunslip2.sh 
tunslip6 Cooja -> PC
=> run_tunslip.sh
tunslip6 /dev/zolertia -> PC
=> bridge.py
Cooja <-> /dev/zolertia
RE-Mote (Client) -> coap.me


=======================================================================
Cooja (Client) -> Cooja (Server)
1. (Node 1) Border Router
2. Mote tools -> Serial Socket (SERVER) Start
3. (Node 2) Server
4. sudo./tunslip6 -a 127.0.0.1 -p60001 fd00::1/64 (./run_tunslip2.sh)
5. (Node 3) Client

---------------------------------
Copper -> Cooja (Server)
1. (Node 1) Border Router
2. Mote tools -> Serial Socket (SERVER) Start
3. (Node 2) Server
4. sudo./tunslip6 -a 127.0.0.1 -p60001 fd00::1/64 (./run_tunslip.sh)
5. coap://[IPV6_global_fd00_server]:5683/.well-known/core

IPV6_global_fd00_server: Mote tools -> Mote Interface Viewer -> IP Addresses

=======================================================================

Cooja (Client) -> coap.me
1. (Node 1) Border Router
2. Mote tools -> Serial Socket (SERVER) Start
3. sudo./tunslip6 -a 127.0.0.1 -p60001 fd00::1/64 (./run_tunslip2.sh)
4. python deec_coap_bridge_v2.py (localCoAPClient = "IPV6_global_fd00_server")
IPV6_global_fd00_server: (Client) Mote tools -> Mote Interface Viewer -> IP Addresses

=======================================================================

RE-Mote (Client) -> coap.me
1. flash BorderRouterClient/border-router.bin
2. sudo ./tunslip6 -s /dev/zolertia fd00::1 (./run_tunslip.sh)
3. python deec_coap_bridge_v2.py (localCoAPClient = "fd00::212:4b00:89ab:cdef")

=======================================================================

Cooja(Client) -> RE-Mote (Server)
1. flash BorderRouterServer/BorderRouterServer.bin
2. sudo ./tunslip6 -s /dev/zolertia aaaa::1/64 (./run_tunslip.sh)
3. (Node 1) Border Router
4. Mote tools -> Serial Socket (SERVER) Start
5. (Node 2) Client
6. sudo ./tunslip6 -a localhost -p 60001 fd00::1/64 (/.run_tunslip2.sh)
7. python bridge.py

RE-Mote new address: aaaa...

