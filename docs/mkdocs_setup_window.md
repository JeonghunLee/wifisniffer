# Window mkdocs Setup

<br/>

* Window Mkdocs   
  * doxygent/grphvis 까지 설치안함 
  * WSL에서 만 생성하여 이를 연결   


<br/>

## Setup venv

<br/>

* PowerShell (관리자 권한 변경)
```
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

<br/>

* Setup Python (venv)          
WSL와 구분하기 위해서 venv 변경   
```
python3 -m venv venv
```

<br/>


* PowerShell (관리자 권한 변경)
```
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

<br/>

* venv activate 
```
Windows (PowerShell)	.\venv\Scripts\Activate.ps1  // 관리자 권한으로 실행 
Windows (CMD)	venv\Scripts\activate
```

<br/>

* venv deactivate
```
deactivate
```

<br/>

## Setup Python     

<br/>

* Mkdocs Package
```
pip install -r docs/readthedoc_requirements.txt
```

<br/>

## TEST mkdocs 

<br/>

* mkdocs.yml 파일확인 
```
...

    - toc.integrate         # 이 줄이 우측 TOC(Table of Contents)를 왼쪽 내비(sidebar)로 통합해줌 
```

<br/>

**mkdocs serve 를 이용하여 TEST 진행**   


* mkdocs serve
mkdocs test 해서 살펴보고 수정 build 나중에  
```
(venv) PS D:\works\projects\wifisniffer> mkdocs serve
INFO    -  Building documentation...
INFO    -  MERMAID2  - Initialization arguments: {}
INFO    -  MERMAID2  - Using javascript library (10.4.0):
              https://unpkg.com/mermaid@10.4.0/dist/mermaid.esm.min.mjs
INFO    -  Cleaning site directory
INFO    -  MERMAID2  - Found superfences config: {'custom_fences': [{'name': 'mermaid', 'class': 'mermaid', 'format': <function fence_code_format at 0x0000018B0C5847C0>}]}
INFO    -  Documentation built in 1.17 seconds
INFO    -  [18:00:29] Serving on http://127.0.0.1:8000/
```


 



