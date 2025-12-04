import os
import sys

# 필요하다면 프로젝트 루트 경로 추가
sys.path.insert(0, os.path.abspath(".."))

project = "WiFi Sniffer Firmware"
author = "Jeonghun Lee"
release = "0.1.0"

extensions = [
    "breathe",
    "myst_parser",          # Markdown도 쓰려면
    # "exhale",             # 사용 시 주석 해제
]

# 템플릿 / 정적 파일
templates_path = ["_templates"]
exclude_patterns = []

# 테마 (sphinx-rtd-theme 사용하는 예)
html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]

# ---------------------
# Breathe 설정
# ---------------------
# Doxyfile 에서 GENERATE_XML = YES, XML_OUTPUT = xml 기준
# Doxyfile에서 OUTPUT_DIRECTORY = docs/_doxygen
breathe_projects = {
    "wifisniffer": os.path.join(os.path.dirname(__file__), "_doxygen", "xml"),
}
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
