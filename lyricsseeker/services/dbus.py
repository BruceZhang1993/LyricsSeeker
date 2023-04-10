import asyncio
from typing import List

from dbus_next.aio import MessageBus
from dbus_next.service import ServiceInterface, method

from lyricsseeker.services.lyrics import LyricsService
from lyricsseeker.services.mpris import MprisInterface
from lyricsseeker.utils import parse_lrc


class AppInterface(ServiceInterface):
    NAME = 'io.github.brucezhang1993.LyricsSeeker'
    PATH = '/'

    def __init__(self, service):
        super().__init__(self.NAME)
        self._service: DBusService = service

    @method()
    def providers(self) -> 'aa{ss}':
        return self._service.providers()

    @method()
    def search(self, provider_id: 's', keyword: 's') -> 'aa{ss}':
        return self._service.search(provider_id, keyword)

    @method()
    def fullLyrics(self, provider_id: 's', identify: 's') -> 's':
        return self._service.full_lyrics(provider_id, identify)

    @method()
    async def fullLyricsCurrent(self, provider_id: 's') -> 's':
        return await self._service.full_lyrics_current_song(provider_id)

    @method()
    async def lyricsCurrentLine(self, provider_id: 's') -> 's':
        return await self._service.full_lyrics_current_line(provider_id)


class DBusService:
    def __init__(self):
        self._interface = AppInterface(self)
        self._bus = None
        self._lyrics_service = LyricsService()
        self._mpris_interface = MprisInterface()

    def providers(self) -> List[dict]:
        return self._lyrics_service.providers()

    def search(self, provider_id: str, keyword: str) -> List[dict]:
        result = self._lyrics_service.search(provider_id, keyword)
        return list(map(lambda r: r.dict(), result))

    def full_lyrics(self, provider_id: str, id_: str) -> str:
        return self._lyrics_service.full_lyrics(provider_id, id_)

    async def full_lyrics_current_song(self, provider_id: str) -> str:
        metadata = await self._mpris_interface.get_metadata_dict()
        song_list = self._lyrics_service.search(provider_id, f"{metadata.get('title')} {metadata.get('artist')}")
        id_ = song_list[0].id
        return self._lyrics_service.full_lyrics(provider_id, id_)

    async def full_lyrics_current_line(self, provider_id: str) -> str:
        microseconds = await self._mpris_interface.get_position()
        lrc = await self.full_lyrics_current_song(provider_id)
        lrc_data = parse_lrc(lrc)
        for line in reversed(lrc_data):
            if microseconds >= line['timestamp']:
                return line['text']
        return ''

    async def serve(self):
        self._bus = await MessageBus().connect()
        self._bus.export(self._interface.PATH, self._interface)
        await self._bus.request_name(self._interface.NAME)
        await self._mpris_interface.init()
        await self._bus.wait_for_disconnect()


if __name__ == '__main__':
    asyncio.get_event_loop().run_until_complete(DBusService().serve())
