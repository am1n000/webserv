/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Settings.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/29 15:32:20 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/23 00:10:39 by otossa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Includes/Settings.hpp"
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>

/*
  * to not repeat our selfs we make the program scalable
  * so when we add a support for a new method 
  * we can just add the method to this array 
  * so when you use settings class from other place 
  * you will be able to use the new added request method 
  * ps: also you need to add the new method to the enum on Settings.hpp
  */

const char* request_methods[] = {
  "GET",
  "POST",
  "DELETE",
  NULL
};

Settings* Settings::object_ = NULL;

Settings::Settings(){};

Settings::~Settings(){};

void Settings::setRequestMethods_() {
  int i(0);
  // const char **begin = std::begin(request_methods);
  // const char **end = std::end(request_methods);
  // while(begin != end) {
  //   this->request_methods_.insert(std::make_pair(std::string(*begin), static_cast<Request_Method_e>(i)));
  //   begin++;
  //   i++;
  // }
  //
  while(request_methods[i]) {
    this->request_methods_.insert(std::make_pair(request_methods[i], static_cast<Request_Method_e>(i)));
    i++;
  }
}

Settings* Settings::boot() {
  if (!Settings::object_) {
    Settings::object_ = new Settings();
    Settings::object_->setRequestMethods_();
  }
  return Settings::object_;
}

Settings* Settings::get(){
  if(!Settings::object_)
    throw std::runtime_error("Settings not bootstrapped");
  return Settings::object_;
}

Request_Method_e Settings::indexOfRequestMethod(const std::string &method) {
  std::map<std::string, Request_Method_e>::iterator it;
  it = this->request_methods_.find(method);
  if(it == this->request_methods_.end())
    throw MethodNotAllowedException();
  return it->second;
}

std::string Settings::methodString(int emethod) {
  return request_methods[emethod];
}
