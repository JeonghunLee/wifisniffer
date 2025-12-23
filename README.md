#  WiFi Sniffer with USB Gadget

<br/>

A WiFi packet sniffer for ESP32-S3 based on ESP-IDF 5.4 that captures WiFi packets in promiscuous mode and streams them via USB CDC-ACM interface.

<br/>

## Features

- **WiFi Packet Sniffing**: Captures WiFi management, control, and data frames in promiscuous mode
- **USB Gadget Interface**: Outputs captured packets via USB CDC-ACM (appears as a serial port)
- **Channel Hopping**: Automatically cycles through WiFi channels 1-13   ---> 좀 후에 HT20/40 확인 후    
- **ESP-IDF 5.4**: Built with the latest ESP-IDF framework
- **ESP32-S3 Support**: Optimized for ESP32-S3 with native USB support


## Github-Manual   

<br/>

* WIFI Sniffer Manaul   
    * [ESP32-S3 WIFI Sniffer](docs/index_mkdocs.md)      

* Wireshark Example 
    * [Wireshark WIFI Sniffer](docs/wireshark_0.md)      

<br/>

## Documentation  


<br/>

* **Manual-1 (ReadTheDocs)**  
    * Github and ReadTheDocs 연결사용     
    * Shpinx only 
    * [DOC: wifisniffer Manaul](https://wifisniffer.readthedocs.io/ko/latest/)          

> [!NOTE]  
> Sometimes, Not working properly ReadTheDocs Site (HTTP Server Problem) 
>

현재 아래와 같이 2가지를 다 지원하지만, 현재는 Sphinx로 동작    
```
.readthedocs.yml  // Currently, Sphinx Mode
.readthedocs_mkdocs.yml  // Backup Mkdocs Mode 
.readthedocs_sphinx.yml // Backup Sphinx Mode 
```

```
docs/index.rst // Sphinx Mode 일때만 사용   
docs/conf.py   // Sphinx Mode 일때만 사용 (반드시 index.rst 과 같이 확인, 다양한 기능을 넣기 위해 좀 복잡함)     
```

<br/>
<br/>

* **Manual-2 (Mkdocs based on Github Pages)**            
    * Github Pages only 
    * Mkdocs only (Material)        
    * [DOC: Github Pages](https://jeonghunlee.github.io/wifisniffer/)   

<br/>

sphinx 와 동시에 지원하다보니, index.rst 충돌문제로 index_mkdoc.md -> index.md 변경           
```
mkdocs_githubaction.yml // Github Action 에서 실제 사용하는 mkdocs.yml 
mkdocs.yml          // Local 에서만 사용하며, TEST 용   
```

<br/>
<br/>

* ReadME TEST  
    * 좋은 것 같은데, 무료에서는 좀 한계가 있는 듯해서 아래에서 멈춤      
    * Github 연결은 되나 Sync는 유료    
    * Landing Page 하려면 유료   
    * [Web: wifisniffer 실패](https://wifi-sniffer.readme.io/)

<br/>

## Github Code Review and Pull Request 

<br/>

Github Project 기반의 Issue 와 Code Review/Pull Request 진행    

<br/>

* Github Template Manual   
    * [Issue and Pull Request templates](https://github.blog/developer-skills/github/issue-and-pull-request-templates/)
        * github/ISSUE_TEMPLATE   
        * github/PULL_REQUEST_TEMPLATE or PULL_REQUEST_TEMPLATE.md
    * [Copilot Code Review](https://docs.github.com/ko/copilot/tutorials/review-ai-generated-code)    


Github Manual 반드시 확인   

<br/>


* Check Repository    
    * [Discussions](https://github.com/JeonghunLee/wifisniffer/discussions)    
    * [Insights->Community Standards](https://github.com/JeonghunLee/wifisniffer/community)     
        * check your templates   
    * [Security->dependabot](https://github.com/JeonghunLee/wifisniffer/security)   

<br/>
   
Discussion TEST 와 각 새로 나온 Github 기능 및 기본 Template 확인    

<br/>



* Github Copilot Manual     
    [Github Copilot](https://docs.github.com/ko/copilot)  
    [Templates](https://docs.github.com/ko/copilot/tutorials/copilot-chat-cookbook/communicate-effectively/creating-templates)
 


<br/>

* Good Opensource Example   
    * https://github.com/vllm-project/vllm        
    * ISSUE_TEMPLATE : https://github.com/vllm-project/vllm/tree/main/.github/ISSUE_TEMPLATE   
    * PULL_REQUEST_TEMPLATE: https://github.com/vllm-project/vllm/blob/main/.github/PULL_REQUEST_TEMPLATE.md
    * CODEOWNERS : https://github.com/vllm-project/vllm/blob/main/.github/CODEOWNERS   
    * Options: 
        * dependabot.yml : https://github.com/vllm-project/vllm/blob/main/.github/dependabot.yml   
        
<br/>


<br/>



<br/>


## Branchs 

<br/>

- [Main Branch](https://github.com/JeonghunLee/wifisniffer/tree/main)

<br/>

* Copilot Branch TEST  
    [Copilot test1](https://github.com/JeonghunLee/wifisniffer/tree/copilot/create-wifi-sniffer-esp32-s3)               
    [Copilot test2](https://github.com/JeonghunLee/wifisniffer/tree/copilot/create-wifi-sniffer-usb-gadget)                       

<br/>



