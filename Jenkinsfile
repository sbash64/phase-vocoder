def docker_files = ["./docker/gcc", "./docker/clang"]
def compilers = ["gcc", "clang"]
node('master') {
    checkout scm

    def stages = [:]

    for (int i = 0; i < docker_files.size(); i++) {
        def docker_file = docker_files[i]
        def docker_image = docker.build(compilers[i], docker_file)
        stages[i] = get_stages(docker_image)
        
    }

    parallel stages[1]
}


def get_stages(docker_image) {
    stages = {
        docker_image.inside {
            stage ('Build') {
                    cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DENABLE_TESTS=ON', installation: 'InSearchPath', steps: [[withCmake: true]]
            }

            stage ('Test') {
                    ctest installation: 'InSearchPath', workingDir: 'build'
            }
        }
    }
    return stages
}