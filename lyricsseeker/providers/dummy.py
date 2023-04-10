from typing import List

from lyricsseeker.models import SearchResult
from lyricsseeker.providers import BaseProvider


class DummyProvider(BaseProvider):

    @staticmethod
    def id() -> str:
        return 'dummy'

    @staticmethod
    def name() -> str:
        return 'Dummy'

    def search(self, keyword: str) -> List[SearchResult]:
        return [SearchResult(id='dummy01', title='测试歌曲', artists='测试歌手', album='测试专辑')]

    def full_lyrics(self, id_: str) -> str:
        return '[00:00.000] Dummy lyrics'
