from echo_cache_client import RemoteCache

rc = RemoteCache("127.0.0.1", 8000)
rc.set("name", "tanson")
res = rc.get("name")

print(res)