def compilers = ["gcc", "clang"]

def stepsForParallel = compilers.collectEntries {
    ["echoing ${it}" : transformIntoStage(it)]
}

parallel stepsForParallel


def transformIntoStage(compiler) {
    return {
        node {
            checkout scm

            def docker_image = docker.build("my-" + compiler, "./docker/" + compiler)
            
            docker_image.inside {
                cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DENABLE_TESTS=ON', installation: 'InSearchPath', steps: [[withCmake: true]]
                ctest installation: 'InSearchPath', workingDir: 'build'
            }
        }
    }
}