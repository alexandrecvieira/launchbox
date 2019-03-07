### Launchbox - Application Launcher for Openbox

Launchbox é idealizado para permitir a busca rápida e execução dos programas instalados no ambiente Openbox.

O aspecto visual e funcionamento são baseados no conceito do [Slingscold do Ubuntu](https://sourceforge.net/projects/slingscold/).

O código foi adaptado do plugin LaunchApps para LXPanel para ser usado como uma aplicação comum através de um atalho configurado pelo usuário como, por exemplo: **Super + w**

A linguagem é C.

##### Referências:
* <https://developer.gnome.org/gio/stable/GAppInfo.html>

##### Dependências:
	Para Ubuntu e Debian:
	sudo apt install libglib2.0-dev libgtk2.0-dev libmagickwand-dev libX11-dev 
	
##### Para instalar pelo source:
	git clone https://github.com/alexandrecvieira/launchbox.git
	cd launchbox
	autoreconf -f
	./configure --prefix=/usr
	make
	sudo make install

##### Screenshot

<img src="http://alexandrecvieira.droppages.com/images/launchbox.png" width="600">

**Teclas de atalho:**

* Esc &rArr; Fecha

* Enter &rArr; Executa a busca

* &uarr; &rArr; Avança uma página

* &darr; &rArr; Retrocede uma página

* Roda do mouse &rArr; Para frente: avança uma página, para trás: retrocede uma página

### Launchbox 2.0.0(Unreleased)
**New Features**
* Recent Applications
* Entry Completion
* Compatibility with most used screen resolutions(1024x768 | 1280x800 | 1280x1024 | 1366x768 | 1440x900 | 1600x900 | 1680x1050 | 1920x1080)

## CHANGELOG
### [Unreleased]
#### Added
- Recent Applications
- Entry Completion
- Compatibility with most used screen resolutions(1024x768 | 1280x800 | 1280x1024 | 1366x768 | 1440x900 | 1600x900 | 1680x1050 | 1920x1080)

#### Fixed
- Memory consumption

### [1.0.2] - 2018-01-27
Funcional apenas na resolução de tela 1920x1080(ISSUE #2)
#### Fixed
- Alterado ícone para "system-run"

#### Added
- Tip "LaunchApps"

### [1.0.1] - 2017-11-16
Funcional apenas na resolução de tela 1920x1080(ISSUE #2)
#### Fixed
- Dynamic calculation of pages

### [1.0.0] - 2017-11-14
Funcional apenas na resolução de tela 1920x1080(ISSUE #2)
- Release 1.0.0
