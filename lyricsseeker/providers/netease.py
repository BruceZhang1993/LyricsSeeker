from typing import List

from lyricsseeker.models import SearchResult
from lyricsseeker.providers import BaseProvider
from lyricsseeker.providers.api.netease import NeteaseApi


class NeteaseProvider(BaseProvider):
    def __init__(self):
        self._api = NeteaseApi()

    @staticmethod
    def id() -> str:
        return 'netease'

    @staticmethod
    def name() -> str:
        return 'Netease Cloud Music'

    def search(self, keyword: str) -> List[SearchResult]:
        resp = self._api.search(keyword)
        return [SearchResult(id=str(song['id']), title=song['name'], artists=','.join([
            artist['name'] for artist in song['artists']
        ]), album=song['album']['name'] or '') for song in resp['result']['songs']]

    def full_lyrics(self, id_: str) -> str:
        resp = self._api.lyrics(id_)
        return resp['lrc']['lyric'] or ''
