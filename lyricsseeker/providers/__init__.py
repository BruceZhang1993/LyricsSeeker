from abc import ABCMeta, abstractmethod
from typing import List

from lyricsseeker.models import SearchResult


class BaseProvider(metaclass=ABCMeta):
    @staticmethod
    @abstractmethod
    def id() -> str:
        """ Returns provider identifier """
        pass

    @staticmethod
    @abstractmethod
    def name() -> str:
        """ Returns provider name """
        pass

    @abstractmethod
    def search(self, keyword: str) -> List[SearchResult]:
        pass

    @abstractmethod
    def full_lyrics(self, id_: str) -> str:
        pass
