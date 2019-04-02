import os, sys
import shutil
import yaml
import urllib2
import zipfile

class Setup:
    def init(this, config_path):
        print("load config")
        with open(config_path, 'r') as input:
            this.data = yaml.safe_load(input)
        print("load config finished")

    def download_sdk(this):
        print("download sdk")
        download_info = this.data["origin"]
        base_url = download_info["url"]
        version = download_info["version"]

        url = "https://" + base_url + version + "/ts3_sdk_" + version + ".zip"
        if not os.path.exists(this.data["path"]["temp_name"]):
            os.mkdir(this.data["path"]["temp_name"], 0755)

        download = urllib2.urlopen(url)
        sdk = download.read()

        with open(this.data["path"]["temp_name"] + this.data["path"]["temp_file"], "wb") as out_file:
            out_file.write(sdk)
        print("download sdk finished")

    def unzip(this):
        print("unzip sdk")
        archive = zipfile.ZipFile(this.data["path"]["temp_name"] + this.data["path"]["temp_file"], 'r')
        archive.extractall(this.data["path"]["temp_name"] + this.data["path"]["temp_folder"])
        archive.close()
        print("unzip sdk finished")

    def deploy(this):
        print("deploy sdk")
        for entry in this.data["deploy"]["list"]:
            to_path = "./TeamSpeak_SDK/ThirdParty" + entry
            if os.path.exists(to_path):
                shutil.rmtree(to_path)
            shutil.copytree(this.data["path"]["temp_name"] + this.data["path"]["temp_folder"] + "ts3_sdk_" + this.data["origin"]["version"] + entry, to_path)
        print("deploy sdk finished")

    def finalize(this):
        print("clean up")
        shutil.rmtree(this.data["path"]["temp_name"])
        print("clean up finished")


setup = Setup()

setup.init("setup_config.yaml")
setup.download_sdk()
setup.unzip()
setup.deploy()
setup.finalize()
