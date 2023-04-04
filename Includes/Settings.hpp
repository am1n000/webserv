/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Settings.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/29 15:34:07 by hchakoub          #+#    #+#             */
/*   Updated: 2023/04/04 15:56:27 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>
#include <map>

enum Request_Method_e {
  GET,
  POST,
  DELETE
};

class Settings {
  public:
  typedef Request_Method_e request_method_type;
  private:
    Settings();
    ~Settings();
    static Settings *object_;
    std::map<std::string, Request_Method_e> request_methods_;
    void setRequestMethods_();

  public:
    static Settings* boot();
    static Settings* get();
    Request_Method_e indexOfRequestMethod(const std::string& method);
};
#endif
