# ReadTheDocs Mkdocs

<br/>

* Mkdocs 의 전체구조
    * mkdocs.yml 관리    
    * docs/readthedoc_requirements.txt  Python Package 관리 
    * Doxyfile_mkdocs   Doxygen 설정 별도 관리 

**주의 Markdown 만 지원**

<br/>


## Python Package Mkdocs

<br/>

* docs/readthedoc_requirements.txt 
```
mkdocs
mkdocs-material
mkdocs-mermaid2-plugin
pymdown-extensions
```

<br/>

## ReadtheDocs Mkdocs 설정 

<br/>



* ReadtheDocs Mkdocs 설정관리 

.readthedocs_mkdocs.yml 
```
# .readthedocs.yml
version: 2

build:
  os: ubuntu-22.04
  tools:
    python: "3.12"
  apt_packages:
    - doxygen    
    - graphviz
  jobs:
    pre_build:
      - doxygen Doxyfile_mkdocs  
    post_install:
      - pip install -r docs/readthedoc_requirements.txt
  
mkdocs:
  configuration: mkdocs.yml

formats:
  - htmlzip
  - pdf
  - epub
```

<br/>

* Readthedocs 최종  변경  

.readthedocs_mkdocs.yml -> .readthedocs.yml 변경 

<br/>


## ReadTheDocs 확인   

<br/>

[ReadTheDocs](https://app.readthedocs.org/dashboard/)

<br/>

* ReadTheDocs 이용   
    * [CFG: wifisniffer](https://app.readthedocs.org/projects/wifisniffer/)             
    * [Web: wifisniffer KR](https://wifisniffer.readthedocs.io/ko/latest/)                 



<br/>



 



