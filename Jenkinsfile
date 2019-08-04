def docker_files = ["./docker/gcc/Dockerfile", "./docker/clang/Dockerfile"]

node('master') {
    def stages = [:]

    for (int i = 0; i < docker_files.size(); i++) {
        def docker_file = docker_files[i]
        docker_image = docker.build("test-image", docker_file)
        stages[docker_file] = get_stages(docker_image)
    }

    parallel stages
}


def get_stages(docker_image) {
    stages = {
        docker_image.inside {
            stage ('Build') {
                steps {
                    cmakeBuild buildDir: 'build', cleanBuild: true, cmakeArgs: '-DENABLE_TESTS=ON', installation: 'InSearchPath', steps: [[withCmake: true]]
                }
            }

            stage ('Test') {
                steps {
                    ctest installation: 'InSearchPath', workingDir: 'build'
                }
            }
        }
    }
    return stages
}