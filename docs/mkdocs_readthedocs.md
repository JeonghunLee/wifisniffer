# ReadTheDocs 

<br/>

* ReadtheDocs 설정 

.readthedocs.yml
```
# .readthedocs.yml
version: 2

build:
  os: ubuntu-22.04
  tools:
    python: "3.12"
  jobs:
    post_install:
      - pip install -r docs/readthedoc_requirements.txt
  
mkdocs:
  configuration: mkdocs.yml

formats:
  - htmlzip
  - pdf
  - epub
```

* readthedoc_requirements.txt 
```
mkdocs
mkdocs-material
mkdocs-mermaid2-plugin
pymdown-extensions
```

## ReadTheDocs 

<br/>

* ReadTheDocs 이용   
  [CFG: wifisniffer](https://app.readthedocs.org/projects/wifisniffer/)
  [Web: wifisniffer KR](https://wifisniffer.readthedocs.io/ko/latest/)



<br/>



 



