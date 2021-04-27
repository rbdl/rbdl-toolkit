pipeline {
  agent any
  stages {
    stage('Build Toolkit') {
      steps {
        sh 'git submodule update --init --recursive'
        cmakeBuild(installation: 'InSearchPath', buildDir: 'build', buildType: 'Release', cleanBuild: true, cmakeArgs: '-DCUSTOM_QT_PATH=~/Qt/5.14.2/gcc_64 -DTOOLKIT_BUILD_OPTIONAL_PLUGINS=ON -DCUSTOM_RBDL_PATH=/home/builder')
        dir(path: 'build') {
          sh 'make -j 8'
        }
      }
    }
  }
}
