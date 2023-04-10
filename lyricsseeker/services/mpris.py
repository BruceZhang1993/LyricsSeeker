import asyncio
from typing import Optional, Dict, Any

from dbus_next import Variant
from dbus_next.aio import MessageBus


class MprisInterface:
    def __init__(self):
        self._bus = None
        self._introspection = None
        self._proxy_object = None
        self._player = None

    async def init(self):
        with open('mpris.xml', 'r') as f:
            self._introspection = f.read()
            self._bus = await MessageBus().connect()
            player_name = await self.get_player_object()
            if player_name is None:
                return
            self._proxy_object = self._bus.get_proxy_object(player_name,
                                                            '/org/mpris/MediaPlayer2',
                                                            self._introspection)
            self._player = self._proxy_object.get_interface('org.mpris.MediaPlayer2.Player')

    async def get_player_object(self) -> Optional[str]:
        with open('dbus.xml', 'r') as f:
            introspection = f.read()
            dbus_object = self._bus.get_proxy_object('org.freedesktop.DBus', '/org/freedesktop/DBus', introspection)
            dbus_interface = dbus_object.get_interface('org.freedesktop.DBus')
            player_names = list(filter(lambda s: 'org.mpris.MediaPlayer2' in s, await dbus_interface.call_list_names()))
            return player_names[0] or None

    async def get_metadata(self):
        if self._player is None:
            return None
        return await self._player.get_metadata()

    @staticmethod
    def _get_variant_value_or_none(variant: Variant) -> Optional[Any]:
        if variant is None:
            return None
        return variant.value

    async def get_metadata_dict(self) -> dict:
        metadata: Dict[str, Variant] = await self.get_metadata()
        return {
            'id': self._get_variant_value_or_none(metadata.get('mpris:trackid')),
            'title': self._get_variant_value_or_none(metadata.get('xesam:title')),
            'artist': self._get_variant_value_or_none(metadata.get('xesam:artist')),
            'album': self._get_variant_value_or_none(metadata.get('xesam:album')),
            'length': self._get_variant_value_or_none(metadata.get('mpris:length')),
            'cover': self._get_variant_value_or_none(metadata.get('mpris:artUrl'))
        }


async def main():
    interface = MprisInterface()
    await interface.init()
    print(await interface.get_metadata_dict())


if __name__ == '__main__':
    asyncio.run(main())
