Import('env')
from pathlib import Path

src = Path(env['PROJECT_DIR']) / 'include' / 'lv_conf.h'
if not src.exists():
    raise FileNotFoundError(f'Missing source LVGL config: {src}')

dst = Path(env['PROJECT_DIR']) / '.pio' / 'libdeps' / env['PIOENV'] / 'lvgl' / 'lv_conf.h'
dst.parent.mkdir(parents=True, exist_ok=True)
dst.write_text(src.read_text())
print(f'Copied LVGL config to {dst}')
