def compilers = ["gcc", "clang"]

def jobs = compilers.collectEntries {
    ["${it}" : job(it)]
}

parallel jobs

def job(compiler) {
    return {
        node {
            checkout scm

            docker_image(compiler).inside {
                def directory = 'build'
                build(directory)
                test(directory)
            }
        }
    }
}

def docker_image(compiler) {
    return docker.build(compiler, "./docker/" + compiler)
}

def build(directory) {
    cmakeBuild \
        buildDir: directory, \ 
        cleanBuild: true, \
        cmakeArgs: '-DENABLE_TESTS=ON', \ 
        installation: 'InSearchPath', \
        steps: [[withCmake: true]]
}

def test(directory) {
    ctest \
        installation: 'InSearchPath', \ 
        workingDir: directory
}