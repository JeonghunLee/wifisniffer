# mkdocs Setup

<br/>

> [!IMPORTANT]  
> * Window 11 의 WSL 문제사항  
>   1. WSL2 사용시, Window 의 directory mount 하면 속도가 너무 느려짐       
>   2. WSL2 기반에 venv를 사용시 더 느려짐 (이유 Window NTFS 기반으로 venv 만들기 때문)    

**Window 11에서 각 NTFS 기반으로 WSL 사용시 심각한 성능 저하가 있으며, venv까지 이용한다면 속도는 감안하고 사용**

<br/>


## Window 의 WSL Setup  

<br/>

> [!NOTE]  
> **Linux 기반으로 mkdocs 사용을 권장**
> Window로도 쉽게 가능하며 venv기반으로 권장   

<br/>

* WSL 설치 
Ubuntu 이외 별도 Linux를 원한다면, 뒤에 정의해서 설치   
```text
> wsl --install
or 
> wsl --install -d <Distribution Name>
```

> [!NOTE]  
> **반드시 Window Reboot 해야함** 

<br/>


* WSL 설치 Linux 확인 
```text
> wsl --list --online   
NAME                            FRIENDLY NAME
AlmaLinux-8                     AlmaLinux OS 8
AlmaLinux-9                     AlmaLinux OS 9
AlmaLinux-Kitten-10             AlmaLinux OS Kitten 10
AlmaLinux-10                    AlmaLinux OS 10
Debian                          Debian GNU/Linux
FedoraLinux-43                  Fedora Linux 43
FedoraLinux-42                  Fedora Linux 42
SUSE-Linux-Enterprise-15-SP7    SUSE Linux Enterprise 15 SP7
SUSE-Linux-Enterprise-16.0      SUSE Linux Enterprise 16.0
Ubuntu                          Ubuntu
Ubuntu-24.04                    Ubuntu 24.04 LTS
archlinux                       Arch Linux
kali-linux                      Kali Linux Rolling
openSUSE-Tumbleweed             openSUSE Tumbleweed
openSUSE-Leap-16.0              openSUSE Leap 16.0
Ubuntu-20.04                    Ubuntu 20.04 LTS
Ubuntu-22.04                    Ubuntu 22.04 LTS
OracleLinux_7_9                 Oracle Linux 7.9
OracleLinux_8_10                Oracle Linux 8.10
OracleLinux_9_5                 Oracle Linux 9.5
openSUSE-Leap-15.6              openSUSE Leap 15.6
SUSE-Linux-Enterprise-15-SP6    SUSE Linux Enterprise 15 SP6
```

<br/>

* wsl 상태 확인 
```text
> wsl --status
기본 배포: Ubuntu
기본 버전: 2
현재 컴퓨터 구성에서는 WSL1이 지원되지 않습니다.
```
<br/>


* WSL Version 설정 
경우에 따라 WSL Version을 변경을 해도 좋음 
```
wsl --set-default-version 2
```

<br/>

## Ubuntu Setup after Install WSL  

<br/>

* Linux Package Install 
```
sudo apt update && sudo apt upgrade -y
```

<br/>

* Setup Python 
```
sudo apt install python3
sudo apt install python3-pip
sudo apt install -y python3-venv
sudo apt install python3.12-venv
sudo apt install -y doxygen graphviz
```

<br/>

* Setup venv
```
python3 -m venv .venv
```

<br/>

### Setup venv (Python) 

<br/>

* venv activate 
```
source .venv/bin/activate
```
<br/>

* venv deactivate
```
deactivate
```

<br/>

### Python Packages (venv)

<br/>

* Mkdocs Package
```
pip install mkdocs mkdocs-material mkdocs-mermaid2-plugin 
```

* Mkdocs Plugin
```
pip install mkdocstrings
pip install mkdocstrings-python
```

<br/>

## Doxygen

<br/>

* Doxyfile 생성 
```
doxygen -g Doxyfile
```

<br/>

* Doxyfile 설정 변경 
```
PROJECT_NAME           = "WiFi Sniffer Firmware"

# 소스 경로 (네 프로젝트에 맞게 수정!)
INPUT                  = src \
                         include

RECURSIVE              = YES

GENERATE_HTML          = YES
GENERATE_LATEX         = NO

# MkDocs의 docs 폴더 안에 생성 (WSL에서는 경로 구분 '/' 중요)
OUTPUT_DIRECTORY       = docs/doxygen

# HTML 페이지에서 코드 보기 가능하게
SOURCE_BROWSER         = YES
INLINE_SOURCES         = YES

# Graphviz 사용(Flow/Call Graph)
HAVE_DOT               = YES
CALL_GRAPH             = YES
CALLER_GRAPH           = YES
```

<br/>


* Doxyfile 실행 
```
(.venv) ahyuo@jhlee:/mnt/d/works/projects/wifisniffer$ doxygen Doxyfile
```

<br/>

## Doxygen 과 mkdocs 연결  

<br/>

* mkdocs.md 생성 
```
# API Reference (Doxygen)

Doxygen으로 생성된 전체 API 문서.

<iframe
  src="doxygen/html/index.html"
  style="width: 100%; height: 80vh; border: none;"
>
</iframe>
```

<br/>

* mkdocs.yml 수정 
```
nav:
.....
  - API (Doxygen): doxygen_api.md   
```

<br/>

## mkdocs 설정 확인 

<br/>

* mkdocs.yml 파일확인 
```
...

  features:
    - toc.follow
    - navigation.instant
    - navigation.instant.progress
    - navigation.path
    - navigation.top
    - navigation.footer
    - search.suggest
    - search.highlight
    - content.tabs.link
    - content.code.annotation
    - content.tooltips
    - content.code.copy
    - toc.integrate         # 이 줄이 우측 TOC(Table of Contents)를 왼쪽 내비(sidebar)로 통합해줌 
```

<br/>

* mkdocs serve
mkdocs test 해서 살펴보고 수정 build 나중에  
```
(.venv) ahyuo@jhlee:/mnt/d/works/projects/wifisniffer$ mkdocs serve
INFO    -  Building documentation...
INFO    -  MERMAID2  - Initialization arguments: {}
INFO    -  MERMAID2  - Using javascript library (10.4.0):
              https://unpkg.com/mermaid@10.4.0/dist/mermaid.esm.min.mjs
INFO    -  Cleaning site directory
INFO    -  MERMAID2  - Found superfences config: {'custom_fences': [{'name': 'mermaid', 'class': 'mermaid', 'format': <function fence_code_format at 0x71a334530fe0>}]}
INFO    -  Documentation built in 3.74 seconds
INFO    -  [15:48:47] Serving on http://127.0.0.1:8000/
```


 



