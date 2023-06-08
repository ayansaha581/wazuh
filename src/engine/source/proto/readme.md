# Intro Engine Message Protocol Buffers (eMessage)

The `generateCode.sh` script formats and generates the code for all the *.proto files in the src folder and saves them in the include folder. It is important to note that every time a change is made to any *.proto file in the src folder, this script must be run to update the generated code in the include folder.
CMake takes care of finding all the *.cpp/hpp files in the include folder and generating the corresponding static library. However, if you delete or rename *.proto files, you need to manually delete the corresponding auto-generated files in the include folder. This is important to keep in mind to avoid adding deprecated files to the library.

Translated with www.DeepL.com/Translator (free version)

Style guide: https://protobuf.dev/programming-guides/style/ </br>
syntax: Proto3  </br>
proto version: 12.21  </br>
clang-format.style: "Engine proyect style"  </br>
## Protocol Compiler Installation

### Ubuntu 22.04


```bash
apt-get update
# Instal protobuff compiler dependencies
apt-get install g++ git curl gnupg clang-format
# Install bazel (https://bazel.build/install/ubuntu?hl=en)
# Add Bazel distribution URI as a package source
curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel.gpg
mv bazel.gpg /etc/apt/trusted.gpg.d/
echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list
# Install and update Bazel 5.4.0 (Bazel 6 is not supported by protobuff yet)
apt update && apt install bazel-5.4.0 && apt full-upgrade
# Remove the symbolic link to the old version
rm /usr/bin/bazel
# Create a symbolic link to the new version
ln -s /usr/bin/bazel-5.4.0 /usr/bin/bazel
# Check the version
bazel --version

# Install protoc (https://github.com/protocolbuffers/protobuf/blob/main/src/README.md)
cd ~
# Get the source code
git clone https://github.com/protocolbuffers/protobuf.git
cd protobuf
# Checkout the version
git checkout v21.12
# Update submodules
git submodule update --init --recursive
# Build
bazel build :protoc :protobuf
# Install
cp bazel-bin/protoc /usr/local/bin
```