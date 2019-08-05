def compilers = ["gcc", "clang"]

def jobs = compilers.collectEntries {
    ["${it}" : job(it)]
}

parallel jobs

def job(compiler) {
    return {
        node {
            checkout scm

            def docker_image = docker.build(compiler, "./docker/" + compiler)
            
            docker_image.inside {
                cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DENABLE_TESTS=ON', installation: 'InSearchPath', steps: [[withCmake: true]]
                ctest installation: 'InSearchPath', workingDir: 'build'
            }
        }
    }
}