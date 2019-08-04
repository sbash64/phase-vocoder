def docker_files = ["./docker/gcc/Dockerfile", "./docker/clang/Dockerfile"]

node('master') {
    def jobs = [:]

    for (int i = 0; i < docker_files.size(); i++) {
        def docker_file = docker_files[i]
        jobs[docker_file] = job(docker_file)
    }

    parallel jobs
}


def job(docker_file) {
    return {
        stages {
            agent { 
                dockerfile { 
                    filename docker_file
                } 
            }
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
}