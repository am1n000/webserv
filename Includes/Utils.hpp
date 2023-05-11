/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-rhai <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/21 10:45:16 by ael-rhai          #+#    #+#             */
/*   Updated: 2023/03/21 10:45:17 by ael-rhai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include "Exceptions.hpp"
#include <fstream>
#include <sys/socket.h>



char *my_tostring(int num);
std::string get_time();
void  displayStatusCodePage(statusCodeExceptions &e, int sock);

#endif