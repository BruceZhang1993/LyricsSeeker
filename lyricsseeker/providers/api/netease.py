import httpx


class NeteaseApi:
    def __init__(self):
        self._client = httpx.Client()

    def __del__(self):
        self._client.close()

    def search(self, keyword: str):
        resp = self._client.post('http://music.163.com/api/search/get', data={
            's': keyword,
            'type': 1,
            'limit': 10,
            'offset': 0
        })
        return resp.json()

    def lyrics(self, id_: str):
        resp = self._client.post('https://music.163.com/api/song/lyric?_nmclfl=1', data={
            'id': int(id_),
            'tv': -1,
            'lv': -1,
            'rv': -1,
            'kv': -1
        })
        return resp.json()


if __name__ == '__main__':
    print(NeteaseApi().lyrics('2033902938'))
