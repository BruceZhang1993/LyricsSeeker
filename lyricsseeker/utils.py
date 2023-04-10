import re
from datetime import timedelta
from typing import List, Dict, Any


def parse_time(format_str: str) -> int:
    minutes, seconds, micros = re.split(r'[:.]', format_str.lstrip('[').rstrip(']'))
    delta = timedelta(minutes=int(minutes), seconds=int(seconds), microseconds=int(micros))
    return int(delta.total_seconds() * 1000) + delta.microseconds


def parse_lrc(text: str) -> List[Dict[str, Any]]:
    lyrics_data = list()
    time_pattern = re.compile(r'(\[\d+:\d+\.\d+])+(.*)')
    for line in re.split(r'[\r\n]', text):
        matched = time_pattern.match(line)
        if matched is None:
            continue
        for group in matched.groups():
            group = group.strip()
            if group.startswith('[') and group.endswith(']'):
                lyrics_data.append({
                    'timestamp': parse_time(group),
                    'time': group.lstrip('[').rstrip(']'),
                    'text': matched.groups()[-1]
                })
    return sorted(lyrics_data, key=lambda x: x['timestamp'])


if __name__ == '__main__':
    content = '''[00:07.850]出てゆこう
[00:11.742]君を守る首輪を捨てよう
[00:09.783]終わらないコントロール
'''
    print(parse_lrc(content))
