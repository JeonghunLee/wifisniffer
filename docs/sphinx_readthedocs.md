# ReadTheDocs 

<br/>

* ReadtheDocs 설정 

..readthedocs_sphinx.yml 
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
      # Doxygen XML 생성 -> Breath -> Sphinx 
      - doxygen Doxyfile_sphinx  
    post_install:
      - pip install -r docs/sphinx_requirements.txt
  
sphinx:
  configuration: docs/conf.py   # Sphinx 설정 파일 위치

formats:
  - htmlzip
  - pdf
  - epub
```

.readthedocs_sphinx.yml -> .readthedocs.yml 변경 


* docs/sphinx_requirements.txt 
```
sphinx>=7.0
breathe>=4.35.0
# 선택: C/C++ 구조 자동 트리 생성하고 싶으면
# exhale>=0.4.3
# 테마(선호에 따라)
sphinx-rtd-theme>=2.0
# Markdown도 쓰고 싶으면 (index.rst + 나머지 md 가능하게)
myst-parser>=3.0
# Sphinx 확장 기능들
linkify-it-py>=2.0.0
```

## ReadTheDocs 

<br/>

[ReadTheDocs](https://app.readthedocs.org/dashboard/)

<br/>

* ReadTheDocs 이용   
    * [CFG: wifisniffer](https://app.readthedocs.org/projects/wifisniffer/)             
    * [Web: wifisniffer KR](https://wifisniffer.readthedocs.io/ko/latest/)                 



<br/>



 



