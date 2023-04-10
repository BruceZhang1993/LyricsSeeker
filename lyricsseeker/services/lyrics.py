from typing import Dict, List

from lyricsseeker.models import SearchResult
from lyricsseeker.providers import BaseProvider
from lyricsseeker.providers.dummy import DummyProvider
from lyricsseeker.providers.netease import NeteaseProvider


class LyricsService:
    def __init__(self):
        self._providers: Dict[str, BaseProvider] = dict()
        self._load_providers()
        self._lyrics_cache = dict()
        self._search_cache = dict()

    def _load_providers(self):
        self._providers[DummyProvider.id()] = DummyProvider()
        self._providers[NeteaseProvider.id()] = NeteaseProvider()

    def get_provider(self, provider_id: str) -> BaseProvider:
        if provider_id == '' or provider_id is None:
            # todo: add config file to set default provider
            provider_id = 'netease'
        provider = self._providers.get(provider_id)
        if provider is None:
            raise RuntimeError(f'provider {provider_id} not found')
        return provider

    def providers(self):
        return [{'id': provider.id(), 'name': provider.name()} for provider in self._providers.values()]

    def search(self, provider_id: str, keyword: str) -> List[SearchResult]:
        data = self._search_cache.get(f'{provider_id}:{keyword}')
        if data is None:
            provider = self.get_provider(provider_id)
            data = provider.search(keyword)
            self._search_cache[f'{provider_id}:{keyword}'] = data
        return data

    def full_lyrics(self, provider_id: str, id_: str) -> str:
        lrc = self._lyrics_cache.get(f'{provider_id}:{id_}')
        if lrc is not None:
            return lrc
        provider = self.get_provider(provider_id)
        lrc = provider.full_lyrics(id_)
        self._lyrics_cache[f'{provider.id()}:{id_}'] = lrc
        return lrc
