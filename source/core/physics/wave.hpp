//
// Created by dlwog on 25. 10. 30..
//

#ifndef MYPROJECT_WAVE_HPP
#define MYPROJECT_WAVE_HPP

struct WaveField {
  int width, height;
  std::vector<float> u;    // current
  std::vector<float> uPrev; // previous
  float c; // wave speed

  void step(float dt) {
    for(int y=1;y<height-1;y++){
      for(int x=1;x<width-1;x++){
        int i = y*width + x;
        u[i] = 2*u[i] - uPrev[i] +
               c*c*dt*dt*(u[i+1]+u[i-1]+u[i+width]+u[i-width]-4*u[i]);
      }
    }
    uPrev = u;
  }
};

struct QuantumField {
  int size;
  std::vector<std::complex<float>> psi; // amplitude at each point

  void step() {
    // 간단하게 인접점과 상호작용
    std::vector<std::complex<float>> newPsi = psi;
    for(int i=1;i<size-1;i++){
      newPsi[i] = 0.5f*(psi[i-1] + psi[i+1]);
    }
    psi = newPsi;
  }

  int measure(int i) {
    float p = std::norm(psi[i]); // 확률 계산
    // 실제 확률 샘플링 (랜덤)
    return (rand()/float(RAND_MAX) < p) ? 1 : 0;
  }
};



#endif //MYPROJECT_WAVE_HPP