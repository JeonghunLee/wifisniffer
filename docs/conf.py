import os
import sys

# 필요하다면 프로젝트 루트 경로 추가
sys.path.insert(0, os.path.abspath(".."))

project = "WiFi Sniffer based on ESP32-S3"
author = "Jeonghun Lee"
release = "0.1.0"

extensions = [
    "breathe",            # Doxygen XML 문서화용
    "myst_parser",        # Markdown도 쓰려면
    "exhale",             # Doxygen XML Control 사용 시 주석 해제
]

# exhale 사용 시 추가
import exhale

# Exhale 설정 (필요 시 조정)
# index.rst api/api_root
exhale_args = {
    # API Reference Root 
    "containmentFolder":     "./api",
    "rootFileName":          "api_root.rst",
    "rootFileTitle":         "API Referenc(Exhale + Breathe)",
    
    # Doxygen XML 위치
    "doxygenStripFromPath":  "..",    
    
    # Exhale 옵션
    "createTreeView":        True,
    # Exhale 문서 상단 설명 넣기
    "afterTitleDescription":
        "This section is automatically generated from Doxygen XML using Exhale.",

    # Exhale 빌드 방식 제어 (Exhale Version 0.3.3 이상에서 사용 가능)
 #   "exhaleExecutesDoxygen": True,
 #   "forceDoxygenBuild":     True,
 #   "treeOutputSubdir":      "api_tree",

    # Exhale 빌드 로그 보기
    "verboseBuild":          False,    
}


#
# Footer Info — Commit / Author / GitHub Link
#

from datetime import datetime
import subprocess

year = datetime.now().year
# copyright HMTL Tags 안됨 주의 
copyright = f"{year}, {author} "

def get_git_info():
    try:
        commit = subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"]
        ).strip().decode("utf-8")

        date = subprocess.check_output(
            ["git", "show", "-s", "--format=%ci", "HEAD"]
        ).strip().decode("utf-8")

        return f"{commit} ({date})"
    except:
        return "unknown"

def get_git_hash():
    try:
        commit = subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"]
        ).strip().decode("utf-8")
        return f"{commit}"
    except:
        return "unknown"

_GIT_INFO = get_git_info()  
_GIT_HASH = get_git_hash()
_GITHUB_COMMIT_URL = (
    f"https://github.com/JeonghunLee/wifisniffer/"
    f"commit/{_GIT_HASH}"
)

html_last_updated_fmt = None
# Footer Information 설정 예, HTML Tags가 안됨 
#html_last_updated_fmt = (
#    f"Updated | commit: {_GIT_INFO} | "
#    f"<a href='{_GITHUB_COMMIT_URL}' target='_blank'>View on GitHub Commit</a>"
#)


# ---------------------
# 템플릿 / 정적 파일
templates_path = ["_templates"]
exclude_patterns = []

# 테마 (sphinx-rtd-theme 사용하는 예)
html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]

#
# GitHub 연동 설정 (삭제시 raw 링크로 표시됨)
#
html_context = {
    "display_github": True,        # GitHub 링크 표시
    "github_user": "JeonghunLee",  # GitHub 사용자명
    "github_repo": "wifisniffer",  # 리포지토리 이름
    "github_version": "main",      # main 브랜치 기준
    "conf_py_path": "/docs/",      # docs 폴더가 위치한 경로
    "commit": get_git_info(),      # 현재 커밋 해시 (footer용)
}


# 
# Breathe XML 연결 설정
# Doxyfile XML 설정
#       GENERATE_XML = YES
#       XML_OUTPUT = xml 
#       OUTPUT_DIRECTORY = docs/_doxygen
breathe_projects = {
    "wifisniffer": os.path.join(os.path.dirname(__file__), "_doxygen", "xml"),
}
#
# Doxyfile HTML 설정 (Not Working)
#       GENERATE_HTML = YES
#       HTML_OUTPUT = html 
#       OUTPUT_DIRECTORY = docs/_doxygen
html_extra_path = ["_doxygen/html"]


breathe_default_project = "wifisniffer"

# ---------------------
# MyST (Markdown) 설정 (선택)
# ---------------------
myst_enable_extensions = [
    "deflist",
    "linkify",
    "colon_fence",
]
source_suffix = {
    ".rst": "restructuredtext",
    ".md": "markdown",
}
