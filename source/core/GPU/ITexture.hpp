//
// Created by ljh on 25. 10. 12..
//

#ifndef MYPROJECT_TEXTURE_HPP
#define MYPROJECT_TEXTURE_HPP
#include <string>

class ITexture{
  std::string name;
  std::string path;
  virtual ~ITexture() =0 ;

};


#endif //MYPROJECT_TEXTURE_HPP