#
# This rakefile defines CI steps.
#

def is_windows?
    ENV["TRAVIS_OS_NAME"] == "windows"
end

def is_osx?
    ENV["TRAVIS_OS_NAME"] == "osx"
end

def should_deploy?
    !ENV["TRAVIS_TAG"].strip.empty?
end

def nuget_api_token
    !ENV["NUGET_API_TOKEN"]
end

alias cmd system

task :install do
    if is_windows?
        cmd "choco install qbs"
        cmd "choco install nuget.commandline"
    elsif is_osx?
        cmd "brew install qbs"
    end
end

task :configure do
    cmd "qbs setup-toolchains --detect"

    if is_windows?
        cmd "qbs config defaultProfile clang-cl-x86_64"
    end
end

task :build do
    cmd "qbs install -d ./build/bin/debug --install-root ./build/debug config:debug"
    cmd "qbs install -d ./build/bin/release --install-root ./build/release config:release"
    cmd "nuget pack ./tools/fundament.nuspec"
end

task :deploy do
    if is_windows? and should_deploy?
        cmd "nuget push fundament.0.0.1.nupkg -k #{nuget_api_token()}-Source https://api.nuget.org/v3/index.json"
    end
end
