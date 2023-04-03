/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hchakoub <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/29 15:35:39 by hchakoub          #+#    #+#             */
/*   Updated: 2023/03/30 17:15:28 by hchakoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define  WEBSERV_HPP

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

#include "Client.hpp"
#include "Config.hpp"
#include "helpers.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "Settings.hpp"
#include "Tockenizer.hpp"
#include "Utils.hpp"

/*
  * external function prototypes
  */

void app();

#endif
