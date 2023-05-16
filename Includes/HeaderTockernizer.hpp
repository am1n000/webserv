/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderTockernizer.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 11:17:55 by hchakoub          #+#    #+#             */
/*   Updated: 2023/05/16 13:12:17 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_TOCKERNIZER_HPP
#define HEADER_TOCKERNIZER_HPP

#include "./Tockenizer.hpp"

class HeaderTockernizer: public Tockenizer {
public:
  HeaderTockernizer();
  HeaderTockernizer(const_reference);
  ~HeaderTockernizer();

  data_type getKey();
  data_type getHeaders();
  data_type getNoneEmptyLine();
  std::pair<std::string, std::string> getNextHeader();
};
#endif // !HEADER_TOCKERNIZER_HPP
