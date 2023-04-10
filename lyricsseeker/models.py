from pydantic.main import BaseModel


class SearchResult(BaseModel):
    id: str  # 歌曲唯一标识
    title: str  # 歌曲标题
    artists: str  # 歌手
    album: str  # 专辑
