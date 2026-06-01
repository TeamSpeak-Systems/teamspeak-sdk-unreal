import json
import os
import shutil
import tarfile
import tempfile
import urllib.request


THIRD_PARTY_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "TeamSpeak_SDK", "ThirdParty")


def load_config(config_path):
    with open(config_path, "r") as f:
        return json.load(f)


def build_url(origin):
    base = origin["url"].rstrip("/")
    version = origin["version"]
    return f"{base}/{version}/teamspeak-sdk-{version}.tar.gz"


def download(url, dest):
    print(f"downloading {url}")
    with urllib.request.urlopen(url) as response, open(dest, "wb") as f:
        shutil.copyfileobj(response, f)


def deploy(archive_path, version, platforms):
    archive_root = f"teamspeak-sdk-{version}/client_sdk"

    with tarfile.open(archive_path, "r:gz") as archive:
        with tempfile.TemporaryDirectory() as tmpdir:
            archive.extractall(tmpdir, filter="data")

            for platform in platforms:
                src = os.path.join(tmpdir, archive_root, platform)
                dst = os.path.join(THIRD_PARTY_DIR, platform)

                if not os.path.isdir(src):
                    print(f"  WARN: {platform} missing in archive — skipping")
                    continue

                if os.path.exists(dst):
                    shutil.rmtree(dst)
                shutil.copytree(src, dst)
                print(f"  deployed {platform}")


def main():
    config = load_config(os.path.join(os.path.dirname(os.path.abspath(__file__)), "bootstrap_config.json"))
    url = build_url(config["origin"])
    platforms = config["platforms"]

    with tempfile.NamedTemporaryFile(suffix=".tar.gz", delete=False) as tmp:
        archive_path = tmp.name
    try:
        download(url, archive_path)
        deploy(archive_path, config["origin"]["version"], platforms)
    finally:
        try:
            os.unlink(archive_path)
        except OSError:
            pass

    print("done")


if __name__ == "__main__":
    main()
