Claro! Aqui está a versão do seu manual com os **comentários traduzidos para inglês**, mantendo todo o código intacto:

---

# Complete NetPanzer Server Installation and Configuration Manual (Ubuntu)

---

## 1️⃣ System Update

```bash
sudo apt update && sudo apt upgrade -y
```

---

## 2️⃣ Clone the game repository

For testing purposes, we clone the **dev** branch:

```bash
git clone -b dev https://github.com/ph2708/netpanzer_server.git
cd netpanzer_server
```

---

## 3️⃣ Install dependencies

### Step by step:

```bash
sudo apt install scons -y
sudo apt-get install subversion -y
sudo apt-get install g++ -y
sudo apt-get install libsdl2-dev -y
sudo apt-get install libsdl2-ttf-dev -y
sudo apt-get install libsdl2-mixer-dev -y
```

### Alternative with two chained commands:

```bash
sudo apt install scons -y
yes | sudo apt-get install subversion && \
yes | sudo apt-get install libsdl2-dev && \
yes | sudo apt-get install g++ && \
yes | sudo apt-get install libsdl2-ttf-dev && \
yes | sudo apt-get install libsdl2-mixer-dev
```

---

## 4️⃣ Build NetPanzer

Compile the game inside the project folder using **SCons**:

```bash
cd ~/netpanzer_server
scons
```

The server executable (`netpanzer`) will be generated in the project folder.

---

## 5️⃣ Configure units and server

### Modify units (e.g., Titan)

```bash
nano units/Titan.upf
```

Example attributes:

```text
hitpoints = 1300;
attack = 140;
reload = 10;
range = 31;
regen = 60;
defend_range = 28;
speed_rate = 16;
speed_factor = 2;
```

> **NOTE:** To change the Titan's attack speed, modify `server.command_burst_limit` in `server.cfg`:
>
> * `server.command_burst_limit = 16` → reload 12
> * `server.command_burst_limit = 14` → reload 10

---

### Server configuration (`server.cfg`)

File location:

```bash
~/.netpanzer/config/server.cfg
```

* If it doesn't exist, run the server once to create the file:

```bash
./netpanzer -d

# To run in debug mode
./netpanzer -d -g
```

* Edit with:

```bash
nano ~/.netpanzer/config/server.cfg
```

---

## 6️⃣ Start the server

### Standard command:

```bash
./netpanzer -d
```

### Using **tmux** to keep the server running after closing the terminal:

```bash
tmux new -s netpanzer_session
./netpanzer -d
```

* To detach from tmux: `Ctrl + B`, then `D`
* To reconnect:

```bash
tmux attach -t netpanzer_session
```

* To list sessions:

```bash
tmux ls
```

* To kill the server process (optional):

```bash
pgrep netpanzer
kill <PID>
```

---

## 7️⃣ Open firewall ports

### Using **firewalld**:

```bash
sudo apt install firewalld -y
sudo systemctl enable firewalld
sudo systemctl start firewalld

# Open TCP and UDP ports 3031
sudo firewall-cmd --zone=public --permanent --add-port=3031/tcp
sudo firewall-cmd --zone=public --permanent --add-port=3031/udp
sudo firewall-cmd --reload

# Check open ports
sudo firewall-cmd --zone=public --list-ports
```

### Using **iptables**:

```bash
sudo iptables -A INPUT -m state --state NEW -p tcp --dport 3031 -j ACCEPT
sudo iptables -A INPUT -m state --state NEW -p udp --dport 3031 -j ACCEPT
sudo netfilter-persistent save
sudo iptables -L INPUT --line-numbers
```

---

## 8️⃣ Create auto-start service

### Step 1: Create init.d script

```bash
sudo nano /etc/init.d/start-autonetpanzer.sh
```

Script content (replace `/home/ubuntu/netpanzer_server/` with your actual path):

```bash
#!/bin/bash

# Navigate to the NetPanzer directory
cd /home/ubuntu/netpanzer_server/

# Run inside tmux using user "ubuntu"
su -c 'tmux new-session -d -s netpanzer_session "/home/ubuntu/netpanzer_server/netpanzer -d"' ubuntu

exit 0
```

### Step 2: Permissions and registration

```bash
sudo chmod +x /etc/init.d/start-autonetpanzer.sh
sudo update-rc.d start-autonetpanzer.sh defaults
sudo ln -s /etc/init.d/start-autonetpanzer.sh /etc/rc2.d/S99start-autonetpanzer.sh
sudo chmod +x /etc/rc2.d/S99start-autonetpanzer.sh
```

---

## 9️⃣ `server.cfg` templates

### 9.1 Normal Server

```text
player.name = "NETPANZER.COM.BR 02 (NORMAL)"
server.port = 3031
server.public = true
server.masterservers = "netpanzer.io"
server.command_burst_limit = 16
game.unit_profiles = "Manta, Panther1, Titan, Stinger, Bobcat, Bear, Archer, Wolf, Drake, Spanzer"
game.unit_spawnlist = "1, 1, 10, 1, 1, 1, 1, 1, 1, 1"
game.maxplayers = 8
game.gametype = 0
...
```

### 9.2 Fights Server

```text
player.name = "NETPANZER.COM.BR 01 (FIGHTS)"
server.command_burst_limit = 16
game.unit_profiles = "Titan, Archer, Spanzer"
game.unit_spawnlist = "30, 0, 0"
game.gametype = 1
...
```

### 9.3 Walhalla Server (Extreme Fights)

```text
player.name = "NETPANZER.COM.BR 03 (EXTREME FIGHTS)"
server.command_burst_limit = 16
game.unit_profiles = "Titan"
game.unit_spawnlist = "30"
game.gametype = 1
...
```

> Fill the remaining parameters as you wish.

---

## 🔧 10️⃣ Free up server space

### Expand logical volume:

```bash
sudo lvextend -L +10G /dev/ubuntu-vg/ubuntu-lv
sudo resize2fs /dev/ubuntu-vg/ubuntu-lv
```

### Clean old logs:

```bash
cd /var/log
sudo du -h --max-depth=1   # Check which folders/logs take up the most space
sudo rm -rf <large_file.log>
sudo journalctl --vacuum-size=50M
```

---

Se você quiser, eu posso criar **uma versão completamente formatada em Markdown pronta para publicar**, mantendo os comentários em inglês e com destaques de código mais limpos para leitura.

Quer que eu faça isso?



-----------------------MODELO SERVER.CFG SERVER NORMAL------------------------------------
```bash
radar.playeroutpostcolor = 1
radar.alliedoutpostcolor = 11
radar.enemyoutpostcolor = 4
radar.unitsize = 0
player.name = "NETPANZER.COM.BR 02 (NORMAL)"
server.port = 3031
server.bindaddress = ""
server.motd = "Bem vindo! (Visite: Netpanzer.com.br)"
server.logging = false
server.public = true
server.masterservers = "netpanzer.io"
server.name = "NetPanzer Server"
server.interactive_console = true
server.authserver = "authserver.netpanzer.info"
server.authentication = false
server.command_burst_limit = 16
game.enable_bases = true
game.base_capture_mode = 1
game.base_limit = 0
game.autokicktime = 20
game.allowmultiip = true
game.unit_profiles = "Manta, Panther1, Titan, Stinger, Bobcat, Bear, Archer, Wolf, Drake, Spanzer>
game.unit_spawnlist = "1, 1, 10, 1, 1, 1, 1, 1, 1, 1"
game.adminpass = "2410"
game.gamepass = ""
game.changeflagtime = 1
game.gametype = 0
game.maxplayers = 8
game.maxunits = 240
game.timelimit = 30
game.fraglimit = 3000
game.powerups = true
game.occupationpercentage = 75
game.allowallies = true
game.cloudcoverage = 0
game.respawntype = 0
game.windspeed = 30
game.lowscorelimit = -45
game.map = "Two clans"
game.mapcycle = "Two clans"
game.mapstyle = "SummerDay"
game.units_styles = "original, danisch, desert, metro, night, nva, platane, surpat, tiger, woodland"
sound.enable = true
sound.music = true
sound.musicvol = 100
sound.effects = true
sound.effectsvol = 100
interface.show_health = true
interface.show_flags = true
interface.show_names = true
interface.attacknotificationime = 5
interface.vehicleselectioncolor = 1
interface.unitselectionmode = 1
interface.unitinfodrawlayer = 0
interface.scrollrate = 1000
interface.rankposition_x = 100
interface.rankposition_y = 100
interface.viewdrawbackgroundmode = 0
```

-----------------------MODELO SERVER.CFG SERVER FIGHTS ------------------------------------

```bash
radar.playeroutpostcolor = 1
radar.alliedoutpostcolor = 11
radar.enemyoutpostcolor = 4
radar.unitsize = 0
player.name = "NETPANZER.COM.BR 01 (FIGHTS)"
server.port = 3031
server.bindaddress = ""
server.motd = "Bem vindo! (Visite: Netpanzer.com.br)"
server.logging = false
server.public = true
server.masterservers = "netpanzer.io"
server.name = "NetPanzer Server"
server.interactive_console = true
server.authserver = "authserver.netpanzer.info"
server.authentication = false
server.command_burst_limit = 16
game.enable_bases = true
game.base_capture_mode = 1
game.base_limit = 0
game.autokicktime = 20
game.allowmultiip = true
game.unit_profiles = "Titan, Archer, Spanzer"
game.unit_spawnlist = "30, 0, 0"
game.adminpass = "2410"
game.gamepass = ""
game.changeflagtime = 1
game.gametype = 1
game.maxplayers = 8
game.maxunits = 240
game.timelimit = 30
game.fraglimit = 3000
game.powerups = true
game.occupationpercentage = 75
game.allowallies = true
game.cloudcoverage = 0
game.respawntype = 0
game.windspeed = 30
game.lowscorelimit = -45
game.map = "Two clans"
game.mapcycle = "Two clans"
game.mapstyle = "SummerDay"
game.units_styles = "original, danisch, desert, metro, night, nva, platane, surpat, tiger, woodland"
sound.enable = true
sound.music = true
sound.musicvol = 100
sound.effects = true
sound.effectsvol = 100
interface.show_health = true
interface.show_flags = true
interface.show_names = true
interface.attacknotificationime = 5
interface.vehicleselectioncolor = 1
interface.unitselectionmode = 1
interface.unitinfodrawlayer = 0
interface.scrollrate = 1000
interface.rankposition_x = 100
interface.rankposition_y = 100
interface.viewdrawbackgroundmode = 0
```

```bash
radar.playeroutpostcolor = 1
radar.alliedoutpostcolor = 11
radar.enemyoutpostcolor = 4
radar.unitsize = 0
player.name = "NETPANZER.COM.BR 03 (EXTREME FIGHTS)"
server.port = 3031
server.bindaddress = ""
server.motd = "Bem vindo! (Visite: Netpanzer.com.br)"
server.logging = false
server.public = true
server.masterservers = "netpanzer.io"
server.name = "NetPanzer Server"
server.interactive_console = true
server.authserver = "authserver.netpanzer.info"
server.authentication = false
server.command_burst_limit = 16
game.enable_bases = true
game.base_capture_mode = 1
game.base_limit = 0
game.autokicktime = 20
game.allowmultiip = true
game.unit_profiles = "Titan"
game.unit_spawnlist = "30"
game.adminpass = "2410"
game.gamepass = ""
game.changeflagtime = 1
game.gametype = 1
game.maxplayers = 6
game.maxunits = 240
game.timelimit = 30
game.fraglimit = 3000
game.powerups = true
game.occupationpercentage = 75
game.allowallies = true
game.cloudcoverage = 0
game.respawntype = 0
game.windspeed = 30
game.lowscorelimit = -45
game.map = "Walhalla"
game.mapcycle = "Walhalla"
game.mapstyle = "SummerDay"
game.units_styles = "original, danisch, desert, metro, night, nva, platane, surpat, tiger, woodland"
sound.enable = true
sound.music = true
sound.musicvol = 100
sound.effects = true
sound.effectsvol = 100
interface.show_health = true
interface.show_flags = true
interface.show_names = true
interface.attacknotificationime = 5
interface.vehicleselectioncolor = 1
interface.unitselectionmode = 1
interface.unitinfodrawlayer = 0
interface.scrollrate = 1000
interface.rankposition_x = 100
interface.rankposition_y = 100
interface.viewdrawbackgroundmode = 0
```

