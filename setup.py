import os, sys
import shutil
import tempfile
import urllib.request
import zipfile
import json

class Setup:
    def init(this, config_path):
        print("load config")
        with open(config_path, "r") as json_file:
            this.data = json.load(json_file)
        print("load config finished")

    def download_sdk(this):
        print("downloading sdk")
        download_info = this.data["origin"]
        base_url = download_info["url"]
        version = download_info["version"]

        url = "https://" + base_url + version + "/ts3_sdk_" + version + ".zip"
        with urllib.request.urlopen(url) as response:
            with tempfile.NamedTemporaryFile(delete=True) as tmp_file:
                shutil.copyfileobj(response, tmp_file)
                archive = zipfile.ZipFile(tmp_file, "r")
                with tempfile.TemporaryDirectory() as tmpdirname:
                    archive.extractall(tmpdirname)
                    for entry in this.data["deploy"]:
                        to_path = os.path.join(".", "TeamSpeak_SDK", "ThirdParty", entry)
                        if os.path.exists(to_path):
                            shutil.rmtree(to_path)
                        sourceDir = os.path.join(tmpdirname, "ts3_sdk_" + this.data["origin"]["version"], entry)
                        shutil.copytree(sourceDir, to_path)

        print("added TeamSpeak SDK to plugin.")

setup = Setup()

setup.init("setup_config.json")
setup.download_sdk()
print("done")