
# Manual Completo de Instalação e Configuração do Servidor NetPanzer (Ubuntu)

---

## 1️⃣ Atualização do sistema

```bash
sudo apt update && sudo apt upgrade -y
```

---

## 2️⃣ Clonar repositório do jogo

Para fins de teste, clonamos a branch **dev**:

```bash
git clone -b dev https://github.com/ph2708/netpanzer_server.git
cd netpanzer_server
```

---

## 3️⃣ Instalação de dependências

### Passo a passo individual:

```bash
sudo apt install scons -y
sudo apt-get install subversion -y
sudo apt-get install g++ -y
sudo apt-get install libsdl2-dev -y
sudo apt-get install libsdl2-ttf-dev -y
sudo apt-get install libsdl2-mixer-dev -y
```

### Alternativa com dois comandos encadeados:

```bash
sudo apt install scons -y
yes | sudo apt-get install subversion && \
yes | sudo apt-get install libsdl2-dev && \
yes | sudo apt-get install g++ && \
yes | sudo apt-get install libsdl2-ttf-dev && \
yes | sudo apt-get install libsdl2-mixer-dev
```

---

## 4️⃣ Build do NetPanzer

Compile o jogo dentro da pasta do projeto usando **SCons**:

```bash
cd ~/netpanzer_server
scons
```

O executável do servidor (`netpanzer`) será gerado na pasta do projeto.

---

## 5️⃣ Configuração de unidades e servidor

### Modificar unidades (ex: Titan)

```bash
nano units/Titan.upf
```

Exemplo de atributos:

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

> **OBS:** Para alterar a velocidade de ataque dos Titans, modifique `server.command_burst_limit` no `server.cfg`:
>
> * `server.command_burst_limit = 16` → reload 12
> * `server.command_burst_limit = 14` → reload 10

---

### Configuração do servidor (`server.cfg`)

Arquivo localizado em:

```bash
~/.netpanzer/config/server.cfg
```

* Caso não exista, execute o servidor uma vez para criar o arquivo:

```bash
./netpanzer -d

# Para executar com modo debug
./netpanzer -d -g
```

* Edite com:

```bash
nano ~/.netpanzer/config/server.cfg
```

---

## 6️⃣ Iniciar o servidor

### Comando padrão:

```bash
./netpanzer -d
```

### Usando **tmux** para manter o servidor aberto após fechar o terminal:

```bash
tmux new -s netpanzer_session
./netpanzer -d
```

* Para desconectar do tmux: `Ctrl + B`, depois `D`
* Para reconectar:

```bash
tmux attach -t netpanzer_session
```

* Para listar sessões:

```bash
tmux ls
```

* Para matar processo do servidor (opcional):

```bash
pgrep netpanzer
kill <PID>
```

---

## 7️⃣ Abrir portas no firewall

### Usando **firewalld**:

```bash
sudo apt install firewalld -y
sudo systemctl enable firewalld
sudo systemctl start firewalld

# Abrir portas TCP e UDP 3031
sudo firewall-cmd --zone=public --permanent --add-port=3031/tcp
sudo firewall-cmd --zone=public --permanent --add-port=3031/udp
sudo firewall-cmd --reload

# Conferir portas abertas
sudo firewall-cmd --zone=public --list-ports
```

### Usando **iptables**:

```bash
sudo iptables -A INPUT -m state --state NEW -p tcp --dport 3031 -j ACCEPT
sudo iptables -A INPUT -m state --state NEW -p udp --dport 3031 -j ACCEPT
sudo netfilter-persistent save
sudo iptables -L INPUT --line-numbers
```

---

## 8️⃣ Criar serviço de inicialização automática

### Passo 1: Criar script init.d

```bash
sudo nano /etc/init.d/start-autonetpanzer.sh
```

Conteúdo do script (substitua `/home/ubuntu/netpanzer_server/` pelo caminho real):

```bash
#!/bin/bash

# Navegue até o diretório do NetPanzer
cd /home/ubuntu/netpanzer_server/

# Execute dentro do tmux usando usuário "ubuntu"
su -c 'tmux new-session -d -s netpanzer_session "/home/ubuntu/netpanzer_server/netpanzer -d"' ubuntu

exit 0
```

### Passo 2: Permissões e registro

```bash
sudo chmod +x /etc/init.d/start-autonetpanzer.sh
sudo update-rc.d start-autonetpanzer.sh defaults
sudo ln -s /etc/init.d/start-autonetpanzer.sh /etc/rc2.d/S99start-autonetpanzer.sh
sudo chmod +x /etc/rc2.d/S99start-autonetpanzer.sh
```

---

## 9️⃣ Modelos de `server.cfg`

### 9.1 Server Normal

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

### 9.2 Server Fights

```text
player.name = "NETPANZER.COM.BR 01 (FIGHTS)"
server.command_burst_limit = 16
game.unit_profiles = "Titan, Archer, Spanzer"
game.unit_spawnlist = "30, 0, 0"
game.gametype = 1
...
```

### 9.3 Server Walhalla (Extreme Fights)

```text
player.name = "NETPANZER.COM.BR 03 (EXTREME FIGHTS)"
server.command_burst_limit = 16
game.unit_profiles = "Titan"
game.unit_spawnlist = "30"
game.gametype = 1
...
```

> Preencha os demais parâmetros conforme seu gosto.

---

## 🔧 10️⃣ Liberar espaço no servidor

### Expandir volume lógico:

```bash
sudo lvextend -L +10G /dev/ubuntu-vg/ubuntu-lv
sudo resize2fs /dev/ubuntu-vg/ubuntu-lv
```

### Limpar logs antigos:

```bash
cd /var/log
sudo du -h --max-depth=1   # Verificar quais pastas/logs ocupam mais espaço
sudo rm -rf <arquivo_grande.log>
sudo journalctl --vacuum-size=50M
```

---


-----------------------MODELO SERVER.CFG SERVER NORMAL------------------------------------
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

-----------------------MODELO SERVER.CFG SERVER FIGHTS ------------------------------------


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

