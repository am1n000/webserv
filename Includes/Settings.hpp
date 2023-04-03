/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Settings.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/29 15:34:07 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/29 17:42:00 by hchakoub         ###   ########.fr       */
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
    std::map<std::string, int> request_methods_;
    void setRequestMethods_();

  public:
    static Settings* boot();
    static Settings* get();
    int indexOfRequestMethod(const std::string& method);
};
#endif
